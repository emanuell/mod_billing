#include <unistd.h>
#include <time.h>
#include "apr_want.h"
#include "apr_general.h"    /* for APR_OFFSETOF                */
#include "apr_network_io.h"

#include "httpd.h"
#include "http_config.h"
#include "http_log.h"
#include "http_protocol.h"  /* for ap_hook_post_read_request */


static void register_pid(apr_pool_t *p, server_rec *s)
{
	pid_t pid;
	
	pid = getpid();
	
	ap_log_error(APLOG_MARK, APLOG_ALERT, 0, s, "CLOUD-UFPE: Process created - pid: %d", (int)pid);
}

static int init_time(request_rec *r)
{
	apr_table_set(r->notes, "clockinit", clock());	
	ap_log_error(APLOG_MARK, APLOG_ALERT, 0, NULL, "CLOUD-UFPE: init time process %d", clock());

	return OK;
}

static int end_time(request_rec *r)
{
	clock_t init_time = (clock_t) apr_table_get(r->notes, "clockinit");
	ap_log_error(APLOG_MARK, APLOG_ALERT, 0, NULL, "CLOUD-UFPE: end time process GETTING FROM TABLE: %d", init_time);
	
	clock_t end_time = clock();	
	double elapsed_time = ((double) end_time - init_time) / CLOCKS_PER_SEC;

	ap_log_error(APLOG_MARK, APLOG_ALERT, 0, NULL, "CLOUD-UFPE: end time process - Request duration: %f", elapsed_time);

	return OK;
}

static void register_hooks(apr_pool_t *p)
{
	//ap_hook_post_config(unique_id_global_init, NULL, NULL, APR_HOOK_MIDDLE);
	ap_hook_child_init(register_pid, NULL, NULL, APR_HOOK_MIDDLE);
	ap_hook_post_read_request(init_time, NULL, NULL, APR_HOOK_MIDDLE);
	ap_hook_log_transaction(end_time, NULL, NULL, APR_HOOK_MIDDLE);
}

module AP_MODULE_DECLARE_DATA billing_module = {
    STANDARD20_MODULE_STUFF,
    NULL,                       /* dir config creater */
    NULL,                       /* dir merger --- default is to override */
    NULL,                       /* server config */
    NULL,                       /* merge server configs */
    NULL,                       /* command apr_table_t */
    register_hooks              /* register hooks */
};
