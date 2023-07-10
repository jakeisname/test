#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <grpc-c/grpc-c.h>
#include "pon.grpc-c.h"

#define CNT 4

static grpc_c_client_t *client;
static int done = 0;

static void *test_check (void *arg)
{
    while (done < CNT) {};

    grpc_c_client_free(client);
    grpc_c_shutdown();

    return NULL;
}

/* invoked:
 *    thread: gc_thread_func
 *            -> gc_run_rpc
 *               -> gc_handle_client_event_internal 
 *                  -> (*cb)
 */
static void onu_activate_cb(grpc_c_context_t *context, void *tag, int success)
{
    Pon__PonReply *r;
    int i = (int) (long) tag;

    do {
        if (context->gcc_stream->read(context, (void **)&r, 0, -1)) {
            gpr_log(GPR_ERROR, "Failed to read\n");
            exit(1);
        }

        if (r)
	    gpr_log(GPR_ERROR, "onu_activate_cb: end - %s, i=%d", 
		    r->message, i);
    } while(r);

    /* gc_client_stream_finish */
    int status = context->gcc_stream->finish(context, NULL, 0);
    done++;
}

int main (int argc, char **argv)
{
    Pon__Onu onu;
    Pon__PonReply *reply = NULL;
    char *host = "172.18.0.7:3000"; // for c
    grpc_status_code status;
    int max_threads = 2;
    int i;

    if (argc < 2) {
        fprintf(stderr, "usage) client <ip:port>\n");
        fprintf(stderr, "try to %s\n", host);
    } else {
        host = argv[1];
    }

    grpc_c_init(GRPC_THREADS, &max_threads);
    client = grpc_c_client_init_by_host(host, "unary_async", NULL, NULL);

    pon__onu__init(&onu);
    onu.has_portid = 1;
    onu.has_onuid = 1;

    for (i = 0; i < CNT; i++) {
	onu.portid = rand() % 8 + 1;
	onu.onuid = rand() % 128 + 1;

	gpr_log(GPR_ERROR, "onu_activate: request - portid=%d, onuid=%d", 
		onu.portid, onu.onuid);

	status = pon__pon_service__onu_activate__async(client, NULL,
		0, &onu, onu_activate_cb, (void *)(long)i);
	if (status != GRPC_STATUS_OK) {
	    fprintf(stderr, "Failed to make OnuActivate gRPC call\n");
	    return 1;
	}

	usleep(500000);
    }

    pthread_t thr;
    pthread_create(&thr, NULL, test_check, NULL);

    grpc_c_client_wait(client);

    gpr_log(GPR_ERROR, "exit normally.\n");

    return 0;
}


