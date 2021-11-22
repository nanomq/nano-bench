#ifndef NNB_OPT_H
#define NNB_OPT_H
#include <assert.h>
#include <getopt.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include <nng/mqtt/mqtt_client.h>
#include <nng/nng.h>
#include <nng/supplemental/util/platform.h>

typedef struct {
	char *host;
	char *username;
	char *password;
	int   port;
	int   version;
	int   count;
	int   startnumber;
	int   interval;
	int   keepalive;
	bool  clean;
	// TODO future
	// bool	ssl;
	// char	certfile[64];
	// char	keyfile[64];
	// char	ifaddr[64];
	// char	prefix[64];
} nnb_conn_opt;

typedef struct {
	char *host;
	char *username;
	char *password;
	char *topic;
	int   port;
	int   version;
	int   count;
	int   startnumber;
	int   interval;
	int   keepalive;
	int   qos;
	bool  clean;
	// TODO future
	// bool	ws;
	// bool	ssl;
	// char	certfile[64];
	// char	keyfile[64];
	// char	ifaddr[64];
	// char	prefix[64];
} nnb_sub_opt;

typedef struct {
	char *host;
	char *username;
	char *password;
	char *topic;
	int   port;
	int   version;
	int   count;
	int   startnumber;
	int   interval;
	int   interval_of_msg;
	int   size;
	int   limit;
	int   keepalive;
	int   qos;
	bool  retain;
	bool  clean;
	// TODO future
	// bool	ws;
	// bool	ssl;
	// char	certfile[64];
	// char	keyfile[64];
	// char	ifaddr[64];
	// char	prefix[64];
} nnb_pub_opt;

static struct option long_options[] = { { "host", required_argument, NULL, 0 },
	{ "port", required_argument, NULL, 0 },
	{ "version", required_argument, NULL, 0 },
	{ "count", required_argument, NULL, 0 },
	{ "startnumber", required_argument, NULL, 0 },
	{ "interval", required_argument, NULL, 0 },
	{ "username", required_argument, NULL, 0 },
	{ "password", required_argument, NULL, 0 },
	{ "keepalive", required_argument, NULL, 0 },
	{ "clean", required_argument, NULL, 0 },
	{ "limit", required_argument, NULL, 0 },
	//  { "ssl", 		required_argument, NULL, 0 },
	//  { "certfile", 	required_argument, NULL, 0 },
	//  { "ketfile", 	required_argument, NULL, 0 },
	//  { "ifaddr", 	required_argument, NULL, 0 },
	//  { "prefix", 	required_argument, NULL, 0 },
	{ "help", no_argument, NULL, 0 }, { NULL, 0, NULL, 0 } };

nnb_conn_opt *nnb_conn_opt_init(int argc, char **argv);

void nnb_conn_opt_destory(nnb_conn_opt *opt);

nnb_sub_opt *nnb_sub_opt_init(int argc, char **argv);

void nnb_sub_opt_destory(nnb_sub_opt *opt);

nnb_pub_opt *nnb_pub_opt_init(int argc, char **argv);

void nnb_pub_opt_destory(nnb_pub_opt *opt);

#endif
