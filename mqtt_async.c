#include "nnb_opt.h"
#include <limits.h>
#include <nng/nng.h>
#include <nng/supplemental/util/platform.h>
#include <stdatomic.h>

#ifndef PARALLEL
#define PARALLEL 8
#endif

static atomic_int acnt          = 0;
static atomic_int recv_cnt      = 0;
static atomic_int last_recv_cnt = 0;
static atomic_int send_cnt      = 0;
static atomic_int send_limit    = 0;
static atomic_int last_send_cnt = 0;

typedef enum { INIT, RECV, WAIT, SEND } nnb_state_flag_t;

typedef enum {
	CONN,
	SUB,
	PUB,
} nnb_opt_flag_t;

struct work {
	nng_aio *        aio;
	nng_msg *        msg;
	nng_ctx          ctx;
	nnb_state_flag_t state;
};

static nnb_opt_flag_t opt_flag = CONN;
static nnb_sub_opt *  sub_opt  = NULL;
static nnb_pub_opt *  pub_opt  = NULL;

void
fatal(const char *msg, int rv)
{
	fprintf(stderr, "%s: %s\n", msg, nng_strerror(rv));
	exit(1);
}

void
sub_cb(void *arg)
{
	struct work *work = arg;
	nng_msg *    msg;
	int          rv;

	switch (work->state) {
	case INIT:
		// subscribe to topics
		nng_mqtt_msg_alloc(&msg, 0);
		nng_mqtt_msg_set_packet_type(msg, NNG_MQTT_SUBSCRIBE);
		nng_mqtt_topic_qos topic[] = {
			{ .qos     = sub_opt->qos,
			    .topic = { .buf = (uint8_t *) sub_opt->topic,
			        .length     = strlen(sub_opt->topic) } },
		};

		nng_mqtt_msg_set_subscribe_topics(msg, topic, 1);
		nng_mqtt_msg_encode(msg);
		nng_aio_set_msg(work->aio, msg);
		work->state = SEND;
		nng_ctx_send(work->ctx, work->aio);
		break;

	case SEND:
		// we are done with subscribing
		if ((rv = nng_aio_result(work->aio)) != 0) {
			nng_msg_free(work->msg);
			fatal("nng_send_aio", rv);
		}
		work->state = RECV;
		nng_ctx_recv(work->ctx, work->aio);
		break;

	case RECV:
		// forever receiving
		if ((rv = nng_aio_result(work->aio)) != 0) {
			fatal("nng_recv_aio", rv);
		}
		++recv_cnt;
		msg         = nng_aio_get_msg(work->aio);
		work->state = RECV;
		nng_ctx_recv(work->ctx, work->aio);
		break;
	}
}

void
pub_cb(void *arg)
{
	struct work *work = arg;
	nng_msg *    msg;
	int          rv;

	switch (work->state) {
	case INIT:

		if (++send_cnt > send_limit) {
			break;
		}
		nng_mqtt_msg_alloc(&work->msg, 0);
		nng_mqtt_msg_set_packet_type(work->msg, NNG_MQTT_PUBLISH);
		nng_mqtt_msg_set_publish_topic(work->msg, pub_opt->topic);
		nng_mqtt_msg_set_publish_qos(work->msg, pub_opt->qos);
		nng_mqtt_msg_set_publish_retain(work->msg, pub_opt->retain);
		char *payload =
		    (char *) nng_alloc(sizeof(char) * pub_opt->size);
		memset(payload, 'A', pub_opt->size);
		nng_mqtt_msg_set_publish_payload(
		    work->msg, (uint8_t *) payload, pub_opt->size);
		nng_mqtt_msg_encode(work->msg);

		nng_msg_dup(&msg, work->msg);
		nng_aio_set_msg(work->aio, msg);
		msg         = NULL;
		work->state = WAIT;
		nng_ctx_send(work->ctx, work->aio);
		break;

	case WAIT:
		work->state = SEND;
		// nng_sleep_aio(pub_opt->interval_of_msg, work->aio);
		nng_msleep(pub_opt->interval_of_msg);
		// break;

	case SEND:
		// send packets
		if ((rv = nng_aio_result(work->aio)) != 0) {
			nng_msg_free(work->msg);
			fatal("nng_send_aio", rv);
		}

		if (++send_cnt > send_limit) {
			break;
		}
		nng_msg_dup(&msg, work->msg);
		nng_aio_set_msg(work->aio, msg);
		msg         = NULL;
		work->state = WAIT;
		nng_ctx_send(work->ctx, work->aio);
		break;
	}
}

struct work *
alloc_work(nng_socket sock, void cb(void *))
{
	struct work *w;
	int          rv;

	if ((w = nng_alloc(sizeof(*w))) == NULL) {
		fatal("nng_alloc", NNG_ENOMEM);
	}
	if ((rv = nng_aio_alloc(&w->aio, cb, w)) != 0) {
		fatal("nng_aio_alloc", rv);
	}
	if ((rv = nng_ctx_open(&w->ctx, sock)) != 0) {
		fatal("nng_ctx_open", rv);
	}
	w->state = INIT;
	return (w);
}

// Connack message callback function
static void
connect_cb(void *arg, nng_msg *msg)
{
	switch (opt_flag) {
	case SUB:
		if (nng_mqtt_msg_get_connack_return_code(msg) == 0) {
			nnb_sub_opt *opt = (nnb_sub_opt *) arg;
			if (arg != NULL) {
				printf("connected: %d. Topics: [\"%s\"]\n",
				    ++acnt, opt->topic);
			}
		}
		break;
	case PUB:
		printf("connected: %d.\n", ++acnt);
		break;
	case CONN:
		printf("connected: %d.\n", ++acnt);
		break;
	}

	nng_msg_free(msg);
}

int
nnb_connect(nnb_conn_opt *opt)
{
	if (opt == NULL) {
		fprintf(stderr, "Connection parameters init failed!\n");
	}

	char       url[128];
	nng_socket sock;
	nng_dialer dialer;
	int        i;
	int        rv;

	sprintf(url, "mqtt-tcp://%s:%d", opt->host, opt->port);
	if ((rv = nng_mqtt_client_open(&sock)) != 0) {
		fatal("nng_socket", rv);
	}

	if ((rv = nng_dialer_create(&dialer, sock, url)) != 0) {
		fatal("nng_dialer_create", rv);
	}

	// Mqtt connect message
	nng_msg *msg;
	nng_mqtt_msg_alloc(&msg, 0);
	nng_mqtt_msg_set_packet_type(msg, NNG_MQTT_CONNECT);
	nng_mqtt_msg_set_connect_keep_alive(msg, opt->keepalive);
	nng_mqtt_msg_set_connect_clean_session(msg, opt->clean);

	if (opt->username) {
		nng_mqtt_msg_set_connect_user_name(msg, opt->username);
	}
	if (opt->password) {
		nng_mqtt_msg_set_connect_password(msg, opt->password);
	}

	nng_dialer_set_ptr(dialer, NNG_OPT_MQTT_CONNMSG, msg);
	nng_dialer_set_cb(dialer, connect_cb, NULL);
	nng_dialer_start(dialer, NNG_FLAG_NONBLOCK);
}

int
nnb_subscribe(nnb_sub_opt *opt)
{
	if (opt == NULL) {
		fprintf(stderr, "Connection parameters init failed!\n");
	}

	char         url[128];
	nng_socket   sock;
	nng_dialer   dialer;
	struct work *works[PARALLEL];
	int          i;
	int          rv;

	sprintf(url, "mqtt-tcp://%s:%d", opt->host, opt->port);
	if ((rv = nng_mqtt_client_open(&sock)) != 0) {
		fatal("nng_socket", rv);
	}

	for (i = 0; i < PARALLEL; i++) {
		works[i] = alloc_work(sock, sub_cb);
	}

	if ((rv = nng_dialer_create(&dialer, sock, url)) != 0) {
		fatal("nng_dialer_create", rv);
	}

	opt_flag = SUB;
	sub_opt  = opt;

	// Mqtt connect message
	nng_msg *msg;
	nng_mqtt_msg_alloc(&msg, 0);
	nng_mqtt_msg_set_packet_type(msg, NNG_MQTT_CONNECT);
	nng_mqtt_msg_set_connect_keep_alive(msg, opt->keepalive);
	nng_mqtt_msg_set_connect_clean_session(msg, opt->clean);
	if (opt->username) {
		nng_mqtt_msg_set_connect_user_name(msg, opt->username);
	}

	if (opt->password) {
		nng_mqtt_msg_set_connect_password(msg, opt->password);
	}

	if ((rv = nng_mqtt_msg_encode(msg)) != 0) {
		fprintf(stderr, "nng_mqtt_msg_encode failed: %d\n", rv);
	}

	nng_dialer_set_ptr(dialer, NNG_OPT_MQTT_CONNMSG, msg);
	nng_dialer_set_cb(dialer, connect_cb, (void *) opt);
	nng_dialer_start(dialer, NNG_FLAG_NONBLOCK);

	// printf("dialer start after\n");
	for (i = 0; i < PARALLEL; i++) {
		sub_cb(works[i]);
	}
}

int
nnb_publish(nnb_pub_opt *opt)
{
	if (opt == NULL) {
		fprintf(stderr, "Connection parameters init failed!\n");
	}

	char         url[128];
	nng_socket   sock;
	nng_dialer   dialer;
	struct work *work;
	int          i;
	int          rv;

	sprintf(url, "mqtt-tcp://%s:%d", opt->host, opt->port);
	if ((rv = nng_mqtt_client_open(&sock)) != 0) {
		fatal("nng_socket", rv);
	}

	work = alloc_work(sock, pub_cb);

	if ((rv = nng_dialer_create(&dialer, sock, url)) != 0) {
		fatal("nng_dialer_create", rv);
	}

	opt_flag = PUB;
	pub_opt  = opt;
	// send_cnt = opt->count;

	// Mqtt connect message
	nng_msg *msg;
	nng_mqtt_msg_alloc(&msg, 0);
	nng_mqtt_msg_set_packet_type(msg, NNG_MQTT_CONNECT);
	// nng_mqtt_msg_set_connect_keep_alive(msg, opt->keepalive);
	// nng_mqtt_msg_set_connect_clean_session(msg, opt->clean);
	// if (opt->username) {
	// 	nng_mqtt_msg_set_connect_user_name(msg, opt->username);
	// }
	// if (opt->password) {
	// 	nng_mqtt_msg_set_connect_password(msg, opt->password);
	// }

	// if ((rv = nng_mqtt_msg_encode(msg)) != 0) {
	// 	fprintf(stderr, "nng_mqtt_msg_encode failed: %d\n", rv);
	// }

	nng_dialer_set_ptr(dialer, NNG_OPT_MQTT_CONNMSG, msg);
	nng_dialer_set_cb(dialer, connect_cb, (void *) opt);
	nng_dialer_start(dialer, NNG_FLAG_NONBLOCK);

	pub_cb(work);
}

int
main(int argc, char **argv)
{
	if (argc < 2) {
		fprintf(
		    stderr, "Usage: nano_bench pub | sub | conn [--help]\n");
		exit(EXIT_FAILURE);
	}

	if (!strcmp(argv[1], "pub")) {
		nnb_pub_opt *opt = nnb_pub_opt_init(argc - 1, ++argv);
		if (0 == opt->limit) {
			send_limit = INT_MAX;
			// printf("send_limit = ulimited\n");
		} else {
			send_limit = opt->limit;
			// printf("send_limit = %d\n", send_limit);
		}
		for (int i = 0; i < opt->count; i++) {
			nnb_publish(opt);
			nng_msleep(opt->interval);
		}
		// nnb_pub_opt_destory(opt);
	} else if (!strcmp(argv[1], "sub")) {
		nnb_sub_opt *opt = nnb_sub_opt_init(argc - 1, ++argv);
		for (int i = 0; i < opt->count; i++) {
			nnb_subscribe(opt);
			nng_msleep(opt->interval);
		}
		nnb_sub_opt_destory(opt);
	} else if (!strcmp(argv[1], "conn")) {
		nnb_conn_opt *opt = nnb_conn_opt_init(argc - 1, ++argv);
		for (int i = 0; i < opt->count; i++) {
			nnb_connect(opt);
			nng_msleep(opt->interval);
		}
		nnb_conn_opt_destory(opt);
	} else {
		fprintf(
		    stderr, "Usage: nano_bench pub | sub | conn [--help]\n");
		exit(EXIT_FAILURE);
	}

	for (;;) {
		nng_msleep(1000); // neither pause() nor sleep() portable
		switch (opt_flag) {
		case SUB:;
			int c         = recv_cnt;
			int l         = last_recv_cnt;
			last_recv_cnt = c;
			if (c != l) {
				printf("recv: total=%d, rate=%d(msg/sec)\n", c,
				    c - l);
			}
			break;
		case PUB:;
			c             = send_cnt;
			l             = last_send_cnt;
			last_send_cnt = c;
			if (c != l) {
				printf("sent: total=%d, rate=%d(msg/sec)\n",
				    c - pub_opt->count, c - l);
			}
			break;
		}
	}

	return 0;
}
