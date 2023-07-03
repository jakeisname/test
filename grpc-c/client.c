#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <grpc-c/grpc-c.h>
#include "pon.grpc-c.h"

static int done = 0;

static void onu_indication_cb(grpc_c_context_t *context, void *tag, int success)
{
    Pon__Onu *r;

    do {
        if (context->gcc_stream->read(context, (void **)&r, 0, -1)) {
            gpr_log(GPR_ERROR, "Failed to read\n");
            exit(1);
        }

        if (r) {
            gpr_log(GPR_ERROR, "Got back: portid=%d, onuid=%d", r->portid, r->onuid);
        }
    } while(r);

    int status = context->gcc_stream->finish(context, NULL, 0);
    gpr_log(GPR_ERROR, "Finished with %d\n", status);

    done = 1;
}

static void onu_activate_cb(grpc_c_context_t *context, void *tag, int success)
{
    Pon__PonReply *r;
    int i = (int) (long) tag;

    do {
        if (context->gcc_stream->read(context, (void **)&r, 0, -1)) {
            gpr_log(GPR_ERROR, "Failed to read\n");
            exit(1);
        }

        if (r) {
            gpr_log(GPR_ERROR, "onu_activate_cb: end - %s, i=%d", r->message, i);
        }
    } while(r);

    int status = context->gcc_stream->finish(context, NULL, 0);
}

int main (int argc, char **argv)
{
    grpc_c_client_t *client;
    Pon__Onu onu;
    Pon__PonReply *reply = NULL;
    char *host = "172.18.0.7:3000"; // for c
    int i;
    grpc_status_code status;
    int max_threads = 40;

    if (argc < 2) {
        fprintf(stderr, "usage) client <ip:port>\n");
        fprintf(stderr, "try to %s\n", host);
    } else {
        host = argv[1];
    }

    grpc_c_init(GRPC_THREADS, &max_threads);

    client = grpc_c_client_init_by_host(host, "unary_sync", NULL, NULL);

    pon__onu__init(&onu);
    onu.has_portid = 1;
    onu.has_onuid = 1;

    for (i = 0; i < 50; i++) {

	    onu.portid = rand() % 8 + 1;
	    onu.onuid = rand() % 128 + 1;

	    gpr_log(GPR_ERROR, "onu_activate: begin - portid=%d, onuid=%d, i=%d", 
		    onu.portid, onu.onuid, i);

#if 0
	    status = pon__pon_service__onu_activate(client, NULL,
			    0, &onu, &reply, NULL, -1);
#else
	    status = pon__pon_service__onu_activate__async(client, NULL,
		    0, &onu, onu_activate_cb, (void *)(long)i);
#endif
	    if (status != GRPC_STATUS_OK) {
		    fprintf(stderr, "Failed to make OnuActivate gRPC call\n");
		    return 1;
	    }

#if 0
	    gpr_log(GPR_ERROR, "onu_activate: end   - %s\n", reply->message);
#endif
    }

    sleep(2);

    gpr_log(GPR_ERROR, "client fin.\n");

    return 0;
}


