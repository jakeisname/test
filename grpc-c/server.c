#include <signal.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/time.h>
#include "pon.grpc-c.h"

static grpc_c_server_t *server;

static void sigint_handler (int x) {
    grpc_c_server_destroy(server);
    exit(0);
}

void pon__pon_service__onu_activate_cb(grpc_c_context_t *context)
{
    Pon__Onu *o;
    grpc_c_status_t status;
    Pon__PonReply r;
    pon__pon_reply__init(&r);

    if (context->gcc_stream->read(context, (void **)&o, 0, -1)) {
        gpr_log(GPR_ERROR, "Failed to read data from client\n");
        return;
    }

    if (o == NULL) {
        gpr_log(GPR_ERROR, "Failed to read data from client. o=NULL\n");
        return;
    }

    char buf[1024];
    buf[0] = '\0';
    gpr_log(GPR_ERROR, "onu_activate_cb: begin - portid=%d, onuid=%d", o->portid, o->onuid);
    snprintf(buf, 1024, "portid=%d, onuid=%d", o->portid, o->onuid);
    r.message = buf;

    sleep(1);

    if (!context->gcc_stream->write(context, &r, 0, -1)) {
        gpr_log(GPR_ERROR, "onu_activate_cb: end   - portid=%d, onuid=%d", o->portid, o->onuid);
    } else {
        gpr_log(GPR_ERROR, "Failed to write\n");
        return;
    }

    status.gcs_code = 0;

    if (context->gcc_stream->finish(context, &status, 0)) {
        gpr_log(GPR_ERROR, "Failed to write status\n");
    }
}

void pon__pon_service__onu_deactivate_cb(grpc_c_context_t *context)
{
    Pon__Onu *o;

    if (context->gcc_stream->read(context, (void **)&o, 0, -1)) {
        gpr_log(GPR_ERROR, "Failed to read data from client\n");
        return;
    }

    Pon__Empty r;
    pon__empty__init(&r);

    if (!context->gcc_stream->write(context, &r, 0, -1)) {
        gpr_log(GPR_ERROR, "onu_deactivate: Wrote ok to %s", grpc_c_get_client_id(context));
    } else {
        gpr_log(GPR_ERROR, "Failed to write\n");
        return;
    }

    grpc_c_status_t status;
    status.gcs_code = 0;

    if (context->gcc_stream->finish(context, &status, 0)) {
        gpr_log(GPR_ERROR, "Failed to write status\n");
        return;
    }
}

void pon__pon_service__onu_indication_cb(grpc_c_context_t *context)
{
    Pon__Onu *o;
    int i;

    if (context->gcc_stream->read(context, (void **)&o, 0, 0)) {
        gpr_log(GPR_ERROR, "Failed to read input from client\n");
        return;
    }

    Pon__Onu r;
    pon__onu__init(&r);
    r.has_portid = 1;
    r.has_onuid = 1;

    while (1) {
        r.portid = rand() % 8 + 1;
        r.onuid = rand() % 128 + 1;

        if (context->gcc_stream->write(context, &r, 0, -1)) {
            gpr_log(GPR_ERROR, "Failed to write\n");
            return;
        }

        sleep(1);
    }

    grpc_c_status_t status;
    status.gcs_code = 0;
    if (context->gcc_stream->finish(context, &status, 0)) {
        gpr_log(GPR_ERROR, "Failed to write status\n");
        return;
    }
}

int main (int argc, char **argv)
{
    int i = 0;
    char *host = "172.18.0.7:3000";
    int max_threads = 40;

    if (argc < 2) {
        fprintf(stderr, "usage) server <ip:port>\n");
    } else {
        host = argv[1];
    }

    gpr_log(GPR_ERROR, "Server listening on %s\n", host);

    signal(SIGINT, sigint_handler);

    grpc_c_init(GRPC_THREADS, (void *)&max_threads);

    server = grpc_c_server_create_by_host(host, NULL, NULL);
    if (server == NULL) {
        gpr_log(GPR_ERROR, "Failed to create server\n");
        exit(1);
    }

    pon__pon_service__service_init(server);

    grpc_c_server_start(server);

    grpc_c_server_wait(server);
}


