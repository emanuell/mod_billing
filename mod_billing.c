//many includes...
/*
TODO

- Segmentation fault when more than one request is done.
*/



#include <unistd.h>
#include <time.h>
#include <string.h>
#include "apr_want.h"
#include "apr_general.h"
#include "apr_network_io.h"

#include "httpd.h"
#include "http_config.h"
#include "http_log.h"
#include "http_protocol.h"
#include "apr_uri.h"

module AP_MODULE_DECLARE_DATA billing_module;

typedef struct billing_t {
	const char* url;
} billing_t;


static int init_time(request_rec *r)
{
	billing_t *bil = ap_get_module_config(r->per_dir_config, &billing_module);	
	
	/*
		If the first path level match specified uri. 
		Ex: billing for /app1; uri=/app1/images/picture.png
	*/
	char *path_found = strstr(r->uri, bil->url);
	if(path_found && (path_found - r->uri) == 0) {
		apr_table_set(r->notes, "clockinit", (clock_t) clock());
		apr_table_set(r->notes, "billing", "y");		
	} else {
		apr_table_set(r->notes, "billing", "n");
	}
	
	return OK;
}

static int end_time(request_rec *r)
{
	if(strcmp(apr_table_get(r->notes, "billing"), "y") == 0) {

		clock_t init_time = (clock_t) apr_table_get(r->notes, "clockinit");
		
		billing_t *bil = ap_get_module_config(r->per_dir_config, &billing_module);
		
		clock_t end_time = clock();
		double elapsed_time = ((double) (end_time - init_time));
		
		ap_log_error(APLOG_MARK, APLOG_NOTICE, 0, NULL, "[Billing %s][URI: %s]: Request CPU time: %f", bil->url, r->uri, elapsed_time);
		//ap_log_error(APLOG_MARK, APLOG_ALERT, 0, NULL, "CLOUD-UFPE: End request process - Request CPU time in secs: %6.9f", ((end_time - init_time) / (double) CLOCKS_PER_SEC));
	}

	return OK;
}

static const char* billing_set(cmd_parms* cmd, void* cfg, const char* val) {
	billing_t *bil = ((billing_t *)cfg);
	bil->url = val;

	int len = strlen(bil->url);
	if(bil->url[len-1] != '/') {
		strncat(bil->url, "/", 1);
	}

	return NULL;
}

static void* billing_create_cfg(apr_pool_t *pool, char *x) {
	billing_t *st = apr_pcalloc(pool, sizeof(billing_t));
	st->url = "/";	

	return st;
}

static const command_rec billing_cmds[] = {
	AP_INIT_TAKE1("Billing", billing_set, NULL, OR_ALL, "Billing for a specific URL"),
	{NULL}
};

static void register_hooks(apr_pool_t *p)
{
	//ap_hook_child_init(register_pid, NULL, NULL, APR_HOOK_MIDDLE);
	ap_hook_post_read_request(init_time, NULL, NULL, APR_HOOK_MIDDLE);
	ap_hook_log_transaction(end_time, NULL, NULL, APR_HOOK_MIDDLE);
}

AP_MODULE_DECLARE_DATA module billing_module = {
    STANDARD20_MODULE_STUFF,
    billing_create_cfg,         /* dir config creater */
    NULL,                       /* dir merger --- default is to override */
    NULL,                       /* server config */
    NULL,                       /* merge server configs */
    billing_cmds,               /* command apr_table_t */
    register_hooks              /* register hooks */
};
