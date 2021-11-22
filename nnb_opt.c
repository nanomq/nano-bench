#include "dbg.h"
#include "nnb_opt.h"
#include "nnb_help.h"
#include <stdlib.h>

static int conn_opt_set(int argc, char **argv, nnb_conn_opt *opt);
static int sub_opt_set(int argc, char **argv, nnb_sub_opt *opt);
static int pub_opt_set(int argc, char **argv, nnb_pub_opt *opt);

nnb_conn_opt *
nnb_conn_opt_init(int argc, char **argv)
{
	nnb_conn_opt *opt = nng_alloc(sizeof(nnb_conn_opt));
	if (opt == NULL) {
		fprintf(stderr, "Memory alloc failed\n");
		exit(EXIT_FAILURE);
	}

	opt->port        = 1883;
	opt->version     = 4;
	opt->count       = 200;
	opt->startnumber = 0;
	opt->interval    = 10;
	opt->keepalive   = 300;
	opt->clean       = true;
	opt->username    = NULL;
	opt->password    = NULL;
	opt->host        = NULL;

	conn_opt_set(argc, argv, opt);
	if (opt->host == NULL) {
		opt->host = nng_strdup("localhost");
	}

	return opt;
}

void
nnb_conn_opt_destory(nnb_conn_opt *opt)
{
	if (opt) {
		if (opt->host) {
			nng_free(opt->host, strlen(opt->host));
			opt->host = NULL;
		}

		if (opt->username) {
			nng_free(opt->username, strlen(opt->username));
			opt->username = NULL;
		}

		if (opt->password) {
			nng_free(opt->password, strlen(opt->password));
			opt->password = NULL;
		}

		nng_free(opt, sizeof(nnb_conn_opt));
		opt = NULL;
	}
}

nnb_pub_opt *
nnb_pub_opt_init(int argc, char **argv)
{
	nnb_pub_opt *opt = nng_alloc(sizeof(nnb_pub_opt));
	if (opt == NULL) {
		fprintf(stderr, "Memory alloc failed\n");
		exit(EXIT_FAILURE);
	}

	opt->port            = 1883;
	opt->version         = 4;
	opt->count           = 200;
	opt->size            = 256;
	opt->limit           = 0;
	opt->startnumber     = 0;
	opt->interval        = 10;
	opt->keepalive       = 300;
	opt->interval_of_msg = 1000;
	opt->retain          = false;
	opt->clean           = true;
	opt->username        = NULL;
	opt->password        = NULL;
	opt->host            = NULL;
	opt->topic           = NULL;

	pub_opt_set(argc, argv, opt);
	if (opt->host == NULL) {
		opt->host = nng_strdup("localhost");
	}

	return opt;
}

void
nnb_pub_opt_destory(nnb_pub_opt *opt)
{
	if (opt) {
		if (opt->host) {
			nng_free(opt->host, strlen(opt->host));
			opt->host = NULL;
		}

		if (opt->username) {
			nng_free(opt->username, strlen(opt->username));
			opt->username = NULL;
		}

		if (opt->password) {
			nng_free(opt->password, strlen(opt->password));
			opt->password = NULL;
		}

		if (opt->topic) {
			nng_free(opt->topic, strlen(opt->topic));
			opt->topic = NULL;
		}


		nng_free(opt, sizeof(nnb_pub_opt));
		opt = NULL;
	}
}

nnb_sub_opt *
nnb_sub_opt_init(int argc, char **argv)
{
	nnb_sub_opt *opt = nng_alloc(sizeof(nnb_sub_opt));
	if (opt == NULL) {
		fprintf(stderr, "Memory alloc failed\n");
		exit(EXIT_FAILURE);
	}

	opt->port        = 1883;
	opt->version     = 4;
	opt->count       = 200;
	opt->startnumber = 0;
	opt->interval    = 10;
	opt->keepalive   = 300;
	opt->qos         = 0;
	opt->clean       = true;
	opt->username    = NULL;
	opt->password    = NULL;
	opt->host        = NULL;
	opt->topic       = NULL;

	sub_opt_set(argc, argv, opt);
	if (opt->topic == NULL) {
		fprintf(stderr, "Error: topic required!\n");
		fprintf(stderr, "Usage: %s\n", sub_info);
		exit(EXIT_FAILURE);
	}
	if (opt->host == NULL) {
		opt->host = nng_strdup("localhost");
	}

	return opt;
}

void
nnb_sub_opt_destory(nnb_sub_opt *opt)
{
	if (opt) {
		if (opt->host) {
			nng_free(opt->host, strlen(opt->host));
			opt->host = NULL;
		}

		if (opt->username) {
			nng_free(opt->username, strlen(opt->username));
			opt->username = NULL;
		}

		if (opt->password) {
			nng_free(opt->password, strlen(opt->password));
			opt->password = NULL;
		}

		nng_free(opt, sizeof(nnb_sub_opt));
		opt = NULL;
	}
}

int
conn_opt_set(int argc, char **argv, nnb_conn_opt *opt)
{

	if (argc < 2) {
		fprintf(stderr, "Usage: %s\n", conn_info);
		exit(EXIT_FAILURE);
	}

	int c;
	int digit_optind = 0;
	int option_index = 0;

	while ((c = getopt_long(argc, argv, "h:p:V:c:n:i:u:P:k:C:S:0",
	            long_options, &option_index)) != -1) {
		int this_option_optind = optind ? optind : 1;
		switch (c) {
		case 0:
			// printf ("option %s",
			// long_options[option_index].name); if (optarg)
			// printf
			// (" with value %s", optarg); printf ("\n");
			if (!strcmp(long_options[option_index].name, "help")) {
				fprintf(stderr, "Usage: %s\n", conn_info);
				exit(EXIT_FAILURE);
			} else if (!strcmp(long_options[option_index].name, "host")) {
				opt->host = nng_strdup(optarg);
			} else if (!strcmp(long_options[option_index].name, "port")) {
				opt->port = atoi(optarg);
			} else if (!strcmp(long_options[option_index].name, "version")) {
				opt->version = atoi(optarg);
			} else if (!strcmp(long_options[option_index].name, "count")) {
				opt->count = atoi(optarg);
			} else if (!strcmp(long_options[option_index].name, "startnumber")) {
				opt->startnumber = atoi(optarg);
			} else if (!strcmp(long_options[option_index].name, "interval")) {
				opt->interval = atoi(optarg);
			} else if (!strcmp(long_options[option_index].name, "username")) {
				opt->username = nng_strdup(optarg);
			} else if (!strcmp(long_options[option_index].name, "password")) {
				opt->password = nng_strdup(optarg);
			} else if (!strcmp(long_options[option_index].name, "keepalive")) {
				opt->keepalive = atoi(optarg);
			} else if (!strcmp(long_options[option_index].name, "clean")) {
				if (!strcmp(optarg, "true")) {
					opt->clean = true;
				} else if (!strcmp(optarg, "true")) {
					opt->clean = false;
				} else {
					fprintf(stderr, "Usage: %s\n", conn_info);
					exit(EXIT_FAILURE);
				}
			}

			break;
		case 'h':
			opt->host = nng_strdup(optarg);
			break;
		case 'p':
			opt->port = atoi(optarg);
			break;
		case 'V':
			opt->version = atoi(optarg);
			break;
		case 'c':
			opt->count = atoi(optarg);
			break;
		case 'n':
			opt->startnumber = atoi(optarg);
			break;
		case 'i':
			opt->interval = atoi(optarg);
			break;
		case 'u':
			opt->username = nng_strdup(optarg);
			break;
		case 'P':
			opt->password = nng_strdup(optarg);
			break;
		case 'k':
			opt->keepalive = atoi(optarg);
			break;
		case 'C':
			if (!strcmp(optarg, "true")) {
				opt->clean = true;
			} else if (!strcmp(optarg, "true")) {
				opt->clean = false;
			} else {
				fprintf(stderr, "Usage: %s\n", conn_info);
				exit(EXIT_FAILURE);
			}
			break;
		case 'S':
			// TODO
			printf("option S with value '%s'\n", optarg);
			break;
		case '?':
			fprintf(stderr, "Usage: %s\n", conn_info);
			exit(EXIT_FAILURE);
			break;
		default:
			fprintf(stderr, "Usage: %s\n", conn_info);
			exit(EXIT_FAILURE);
			printf(
			    "?? getopt returned character code 0%o ??\n", c);
		}
	}
	if (optind < argc) {
		fprintf(stderr, "Usage: %s\n", conn_info);
		exit(EXIT_FAILURE);
		while (optind < argc)
			printf("%s ", argv[optind++]);
		printf("\n");
	}
}

int
pub_opt_set(int argc, char **argv, nnb_pub_opt *opt)
{

	if (argc < 2) {
		fprintf(stderr, "Usage: %s\n", pub_info);
		exit(EXIT_FAILURE);
	}

	int c;
	int digit_optind = 0;
	int option_index = 0;

	while (
	    (c = getopt_long(argc, argv, "q:l:r:s:t:I:h:p:V:c:n:i:u:P:k:C:L:S:0",
	         long_options, &option_index)) != -1) {
		int this_option_optind = optind ? optind : 1;
		switch (c) {
		case 0:
			printf ("option %s",
			long_options[option_index].name); if (optarg)
			printf
			(" with value %s", optarg); printf ("\n");
			if (!strcmp(long_options[option_index].name, "help")) {
				fprintf(stderr, "Usage: %s\n", pub_info);
				exit(EXIT_FAILURE);
			} else if (!strcmp(long_options[option_index].name, "topic")) {
				opt->topic = nng_strdup(optarg);
			} else if (!strcmp(long_options[option_index].name, "host")) {
				opt->host = nng_strdup(optarg);
			} else if (!strcmp(long_options[option_index].name, "port")) {
				opt->port = atoi(optarg);
			} else if (!strcmp(long_options[option_index].name, "version")) {
				opt->version = atoi(optarg);
			} else if (!strcmp(long_options[option_index].name, "count")) {
				opt->count = atoi(optarg);
			} else if (!strcmp(long_options[option_index].name, "startnumber")) {
				opt->startnumber = atoi(optarg);
			} else if (!strcmp(long_options[option_index].name, "interval")) {
				opt->interval = atoi(optarg);
			} else if (!strcmp(long_options[option_index].name, "username")) {
				opt->username = nng_strdup(optarg);
			} else if (!strcmp(long_options[option_index].name, "password")) {
				opt->password = nng_strdup(optarg);
			} else if (!strcmp(long_options[option_index].name, "keepalive")) {
				opt->keepalive = atoi(optarg);
			} else if (!strcmp(long_options[option_index].name, "clean")) {
				if (!strcmp(optarg, "true")) {
					opt->clean = true;
				} else if (!strcmp(optarg, "true")) {
					opt->clean = false;
				} else {
					fprintf(stderr, "Usage: %s\n", pub_info);
					exit(EXIT_FAILURE);
				}
			} else if (!strcmp(long_options[option_index].name, "qos")) {
				opt->qos = atoi(optarg);
				if (opt->qos < 0 || opt->qos > 2) {
					fprintf(stderr, "Error: qos invalided!\n");
					fprintf(stderr, "Usage: %s\n", pub_info);
					exit(EXIT_FAILURE);
				}
			} else if (!strcmp(long_options[option_index].name, "limit")) {
				opt->limit = atoi(optarg);
			} else if (!strcmp(long_options[option_index].name, "retain")) {
				if (!strcmp(optarg, "true")) {
					opt->retain = true;
				} else if (!strcmp(optarg, "true")) {
					opt->retain = false;
				} else {
					fprintf(stderr, "Usage: %s\n", pub_info);
					exit(EXIT_FAILURE);
				}
			} else if (!strcmp(long_options[option_index].name, "size")) {
				opt->size = atoi(optarg);
			} else if (!strcmp(long_options[option_index].name, "interval_of_msg")) {
				opt->interval_of_msg = atoi(optarg);
			}

			break;
		case 'l':
			opt->limit = atoi(optarg);
			break;
		case 't':
			opt->topic = nng_strdup(optarg);
			break;
		case 'q':
			opt->qos = atoi(optarg);
			if (opt->qos < 0 || opt->qos > 2) {
				fprintf(stderr, "Error: qos invalided!\n");
				fprintf(stderr, "Usage: %s\n", pub_info);
				exit(EXIT_FAILURE);
			}
			break;
		case 's':
			opt->size = atoi(optarg);
			break;
		case 'I':
			opt->interval_of_msg = atoi(optarg);
			break;
		case 'h':
			opt->host = nng_strdup(optarg);
			break;
		case 'p':
			opt->port = atoi(optarg);
			break;
		case 'V':
			opt->version = atoi(optarg);
			break;
		case 'c':
			opt->count = atoi(optarg);
			break;
		case 'n':
			opt->startnumber = atoi(optarg);
			break;
		case 'i':
			opt->interval = atoi(optarg);
			break;
		case 'u':
			opt->username = nng_strdup(optarg);
			break;
		case 'P':
			opt->password = nng_strdup(optarg);
			break;
		case 'k':
			opt->keepalive = atoi(optarg);
			break;
		case 'r':
			if (!strcmp(optarg, "true")) {
				opt->retain = true;
			} else if (!strcmp(optarg, "true")) {
				opt->retain = false;
			} else {
				fprintf(stderr, "Usage: %s\n", pub_info);
				exit(EXIT_FAILURE);
			}
			break;
		case 'C':
			if (!strcmp(optarg, "true")) {
				opt->clean = true;
			} else if (!strcmp(optarg, "true")) {
				opt->clean = false;
			} else {
				fprintf(stderr, "Usage: %s\n", pub_info);
				exit(EXIT_FAILURE);
			}
			break;
		case 'L':
			opt->limit = atoi(optarg);
			if (opt->limit < 0) {
				fprintf(stderr, "Usage: %s\n", pub_info);
				exit(EXIT_FAILURE);
			}
			break;
		case 'S':
			// TODO
			printf("option S with value '%s'\n", optarg);
			break;
		case '?':
			fprintf(stderr, "Usage: %s\n", pub_info);
			exit(EXIT_FAILURE);
			break;
		default:
			fprintf(stderr, "Usage: %s\n", pub_info);
			exit(EXIT_FAILURE);
			printf(
			    "?? getopt returned character code 0%o ??\n", c);
		}
	}
	if (optind < argc) {
		fprintf(stderr, "Usage: %s\n", pub_info);
		exit(EXIT_FAILURE);
		while (optind < argc)
			printf("%s ", argv[optind++]);
		printf("\n");
	}


	if (opt->topic == NULL) {
		fprintf(stderr, "Error: topic required\n");
		fprintf(stderr, "Usage: %s\n", pub_info);
		exit(EXIT_FAILURE);


	}
}

int
sub_opt_set(int argc, char **argv, nnb_sub_opt *opt)
{

	if (argc < 2) {
		fprintf(stderr, "Usage: %s\n", sub_info);
		exit(EXIT_FAILURE);
	}

	int c;
	int digit_optind = 0;
	int option_index = 0;

	while ((c = getopt_long(argc, argv, "q:t:h:p:V:c:n:i:u:P:k:C:S:0",
	            long_options, &option_index)) != -1) {
		int this_option_optind = optind ? optind : 1;
		switch (c) {
		case 0:
			printf ("option %s",
			long_options[option_index].name); if (optarg)
			printf
			(" with value %s", optarg); printf ("\n");
			if (!strcmp(long_options[option_index].name, "help")) {
				fprintf(stderr, "Usage: %s\n", sub_info);
				exit(EXIT_FAILURE);
			} else if (!strcmp(long_options[option_index].name, "topic")) {
				opt->topic = nng_strdup(optarg);
			} else if (!strcmp(long_options[option_index].name, "host")) {
				opt->host = nng_strdup(optarg);
			} else if (!strcmp(long_options[option_index].name, "port")) {
				opt->port = atoi(optarg);
			} else if (!strcmp(long_options[option_index].name, "version")) {
				opt->version = atoi(optarg);
			} else if (!strcmp(long_options[option_index].name, "count")) {
				opt->count = atoi(optarg);
			} else if (!strcmp(long_options[option_index].name, "startnumber")) {
				opt->startnumber = atoi(optarg);
			} else if (!strcmp(long_options[option_index].name, "interval")) {
				opt->interval = atoi(optarg);
			} else if (!strcmp(long_options[option_index].name, "username")) {
				opt->username = nng_strdup(optarg);
			} else if (!strcmp(long_options[option_index].name, "password")) {
				opt->password = nng_strdup(optarg);
			} else if (!strcmp(long_options[option_index].name, "keepalive")) {
				opt->keepalive = atoi(optarg);
			} else if (!strcmp(long_options[option_index].name, "clean")) {
				if (!strcmp(optarg, "true")) {
					opt->clean = true;
				} else if (!strcmp(optarg, "true")) {
					opt->clean = false;
				} else {
					fprintf(stderr, "Usage: %s\n", sub_info);
					exit(EXIT_FAILURE);
				}
			} else if (!strcmp(long_options[option_index].name, "qos")) {
				opt->qos = atoi(optarg);
				if (opt->qos < 0 || opt->qos > 2) {
					fprintf(stderr, "Error: qos invalided!\n");
					fprintf(stderr, "Usage: %s\n", sub_info);
					exit(EXIT_FAILURE);
				}
			}
			break;

		case 't':
			opt->topic = nng_strdup(optarg);
			break;
		case 'q':
			opt->qos = atoi(optarg);
			if (opt->qos < 0 || opt->qos > 2) {
				fprintf(stderr, "Error: qos invalided!\n");
				fprintf(stderr, "Usage: %s\n", sub_info);
				exit(EXIT_FAILURE);
			}

			break;
		case 'h':
			opt->host = nng_strdup(optarg);
			break;
		case 'p':
			opt->port = atoi(optarg);
			break;
		case 'V':
			opt->version = atoi(optarg);
			break;
		case 'c':
			opt->count = atoi(optarg);
			break;
		case 'n':
			opt->startnumber = atoi(optarg);
			break;
		case 'i':
			opt->interval = atoi(optarg);
			break;
		case 'u':
			opt->username = nng_strdup(optarg);
			break;
		case 'P':
			opt->password = nng_strdup(optarg);
			break;
		case 'k':
			opt->keepalive = atoi(optarg);
			break;
		case 'C':
			if (!strcmp(optarg, "true")) {
				opt->clean = true;
			} else if (!strcmp(optarg, "true")) {
				opt->clean = false;
			} else {
				fprintf(stderr, "Usage: %s\n", sub_info);
				exit(EXIT_FAILURE);
			}
			break;
		case 'S':
			// TODO
			printf("option S with value '%s'\n", optarg);
			break;
		case '?':
			fprintf(stderr, "Usage: %s\n", sub_info);
			exit(EXIT_FAILURE);
			break;
		default:
			fprintf(stderr, "Usage: %s\n", sub_info);
			exit(EXIT_FAILURE);
			printf(
			    "?? getopt returned character code 0%o ??\n", c);
		}
	}
	if (optind < argc) {
		fprintf(stderr, "Usage: %s\n", sub_info);
		exit(EXIT_FAILURE);
		while (optind < argc)
			printf("%s ", argv[optind++]);
		printf("\n");
	}
}
