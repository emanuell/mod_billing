/*
TODO
 - many includes...
 - many warnings
*/



#include <unistd.h>
#include <time.h>
#include <string.h>
#include "apr_strings.h" 
#include "apr_want.h"
#include "apr_general.h"
#include "apr_network_io.h"
#include "apr_pools.h"

#include "httpd.h"
#include "http_config.h"
#include "http_log.h"
#include "http_protocol.h"
#include "apr_uri.h"

module AP_MODULE_DECLARE_DATA billing_module;

typedef struct billing_t {
	char* url;
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
		
		clock_t init_time = clock();
		apr_pool_userdata_set((clock_t*)init_time, "clockinit", NULL, r->pool);
		apr_pool_userdata_set("y", "billing", NULL, r->pool);
	
	} else {
		apr_pool_userdata_set("n", "billing", NULL, r->pool);
	}
	
	return OK;
}

static int end_time(request_rec *r)
{
	void* tax;
	apr_pool_userdata_get(&tax, "billing", r->pool);	

	if(strcmp(tax, "y") == 0) {

		clock_t init_time;
		apr_pool_userdata_get(&init_time, "clockinit", r->pool);	
		
		billing_t *bil = ap_get_module_config(r->per_dir_config, &billing_module);
		
		clock_t end_time = clock();
		double elapsed_time = ((double) (end_time - init_time));
		
		ap_log_error(APLOG_MARK, APLOG_NOTICE, 0, NULL, "[Billing %s][URI: %s]: Request CPU time: %6.3f", bil->url, r->uri, (elapsed_time / (double) CLOCKS_PER_SEC));
	
	}

	return OK;
}

static const char* billing_set(cmd_parms* cmd, void* cfg, const char* val) {
	billing_t *bil = ((billing_t *)cfg);
	
	if (val != NULL) {
	
		//char* tmp = apr_palloc(cmd->pool, sizeof(char));
		char* tmp = malloc((strlen(val) + 1) * sizeof(char));
		apr_cpystrn(tmp, val, strlen(val) + 1); // +1 because null terminator.
		
		bil->url = tmp;
		
		int len = strlen(bil->url);
		if(bil->url[len-1] != '/') {
			strncat(bil->url, "/", 1);
		}

	} else {
		bil->url = "/";
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
