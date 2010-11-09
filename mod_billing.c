#include <string.h>
#include "httpd.h"
#include "http_config.h"
#include "http_log.h"
#include "http_protocol.h"


module AP_MODULE_DECLARE_DATA billing_module;

typedef struct billing_t {
	char *url;
} billing_t;

typedef struct rcpu_t {
	clock_t init;
	clock_t end;
} rcpu_t;


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
		
		rcpu_t *consumption = apr_palloc(r->pool, sizeof(rcpu_t));
		consumption->init = init_time;

		char key[35];
		sprintf(key, "%f", (float)r->request_time);

		apr_pool_userdata_set((const void *)consumption, key, NULL, r->pool);
		apr_pool_userdata_set("y", "billing", NULL, r->pool);
	} else {
		apr_pool_userdata_set("n", "billing", NULL, r->pool);
	}
	
	return OK;
}

static int end_time(request_rec *r)
{
	void *tax;
	apr_pool_userdata_get(&tax, "billing", r->pool);

	if(strcmp(tax, "y") == 0) {

		billing_t *bil;
		rcpu_t *consumption;
		void *data;
		char key[35];

		sprintf(key, "%f", (float)r->request_time);
		apr_pool_userdata_get(&data, key, r->pool);

		consumption = (rcpu_t *)data;
		bil = ap_get_module_config(r->per_dir_config, &billing_module);
		
		consumption->end = clock();
		double elapsed_time = ((double) (consumption->end - consumption->init));
		
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
