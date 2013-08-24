/*
  +----------------------------------------------------------------------+
  | PHP Version 5                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 1997-2009 The PHP Group                                |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.01 of the PHP license,      |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.php.net/license/3_01.txt.                                 |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Author: Guruswamy Basavaiah                                          |
  +----------------------------------------------------------------------+
  Please contact support@ip2location.com  with any comments
*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php_ip2location.h"

int le_ip2location_ptr;

#define IP2LOCATION_RECORD 0

static zend_function_entry ip2location_functions_entry[] = {
	PHP_FE(ip2location_open, NULL)
	PHP_FE(ip2location_open_mem, NULL)
	PHP_FE(ip2location_get_country_short, NULL)
	PHP_FE(ip2location_get_country_long, NULL)
	PHP_FE(ip2location_get_region, NULL)
	PHP_FE(ip2location_get_city, NULL)
	PHP_FE(ip2location_get_isp, NULL)
	PHP_FE(ip2location_get_latitude, NULL)
	PHP_FE(ip2location_get_longitude, NULL)
	PHP_FE(ip2location_get_domain, NULL)
	PHP_FE(ip2location_get_zipcode, NULL)
	PHP_FE(ip2location_get_timezone, NULL)
	PHP_FE(ip2location_get_netspeed, NULL)
	PHP_FE(ip2location_get_iddcode, NULL)
	PHP_FE(ip2location_get_areacode, NULL)
	PHP_FE(ip2location_get_weatherstationcode, NULL)
	PHP_FE(ip2location_get_weatherstationname, NULL)
	PHP_FE(ip2location_get_mcc, NULL)
	PHP_FE(ip2location_get_mnc, NULL)
	PHP_FE(ip2location_get_mobilebrand, NULL)
	PHP_FE(ip2location_get_elevation, NULL)
	PHP_FE(ip2location_get_usagetype, NULL)
	PHP_FE(ip2location_get_all, NULL)
	PHP_FE(ip2location_close, NULL)
	PHP_FE(ip2location_delete_shm, NULL)
	{NULL, NULL, NULL}
};

/* the following code creates an entry for the module and registers it with Zend.*/
zend_module_entry ip2location_module_entry = {
	STANDARD_MODULE_HEADER,
	PHP_IP2LOCATION_EXTNAME,
	ip2location_functions_entry,
	PHP_MINIT(ip2location),
	PHP_MSHUTDOWN(ip2location),
	NULL, 
	NULL, 
	PHP_MINFO(ip2location), 
	PHP_IP2LOCATION_VERSION,
	STANDARD_MODULE_PROPERTIES
};

#ifdef COMPILE_DL_IP2LOCATION
ZEND_GET_MODULE(ip2location)
#endif

#define PHP_IP2LOCATION_INVALID_HANDLE_CHECK \
	if( ip2location_ptr == NULL ) { \
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "ip2location db file is not opened or handle passed is invalid"); \
		RETURN_FALSE; \
	}

/* {{{ php_ip2location_ptr_persist_dtor
 *  */
static void php_ip2location_ptr_persist_dtor(zend_rsrc_list_entry *rsrc TSRMLS_DC)
{
	IP2Location *ip2location_ptr = (IP2Location*)rsrc->ptr;

	if (ip2location_ptr) {
		IP2Location_close(ip2location_ptr);
	}
}
/* }}}*/

/* {{{ PHP_MINIT_FUNCTION
 *  */
PHP_MINIT_FUNCTION(ip2location)
{
	le_ip2location_ptr = zend_register_list_destructors_ex(NULL, php_ip2location_ptr_persist_dtor, PHP_IP2LOCATION_PTR_RES_NAME, module_number);
	
	/* For memory access type constants */
	REGISTER_LONG_CONSTANT("IP2LOCATION_FILE_IO", IP2LOCATION_FILE_IO, CONST_CS | CONST_PERSISTENT); 
	REGISTER_LONG_CONSTANT("IP2LOCATION_CACHE_MEMORY", IP2LOCATION_CACHE_MEMORY, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("IP2LOCATION_SHARED_MEMORY", IP2LOCATION_SHARED_MEMORY, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("IP2LOCATION_RECORD", IP2LOCATION_RECORD, CONST_CS | CONST_PERSISTENT);

	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MSHUTDOWN_FUNCTION
 *  */
PHP_MSHUTDOWN_FUNCTION(ip2location)
{
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MINFO_FUNCTION
 *  */
PHP_MINFO_FUNCTION(ip2location)
{
	php_info_print_table_start();
	php_info_print_table_header(2, "ip2location support", "enabled");
	php_info_print_table_row(2, "version", PHP_IP2LOCATION_VERSION);
	php_info_print_table_end();
}
/* }}} */

/* {{{ ip2location_open("db_filepath")
 * Returns the success or failure */
PHP_FUNCTION(ip2location_open)
{
	char * file_path = NULL;
	int path_len; 
	long handle;
	IP2Location *ip2location_ptr;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &file_path, &path_len) == FAILURE) { 
		return;
	}

	ip2location_ptr = IP2Location_open(file_path);

	if (ip2location_ptr) {
		handle = ZEND_REGISTER_RESOURCE(return_value, ip2location_ptr, le_ip2location_ptr);
		RETVAL_LONG(handle); 
	} else {
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ ip2location_open_mem(handle, method)
 * Returns the success or failure */
PHP_FUNCTION(ip2location_open_mem)
{
	int method;
	long handle;
	IP2Location *ip2location_ptr;
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ll", &handle, &method) == FAILURE) { 
		return;
	}
	
	ip2location_ptr = (IP2Location*)zend_fetch_resource(NULL, handle, PHP_IP2LOCATION_PTR_RES_NAME,NULL,1, le_ip2location_ptr);
	PHP_IP2LOCATION_INVALID_HANDLE_CHECK

	if( method == IP2LOCATION_FILE_IO || method == IP2LOCATION_CACHE_MEMORY || method == IP2LOCATION_SHARED_MEMORY ){
		if(IP2Location_open_mem(ip2location_ptr, method) == -1){
			RETURN_FALSE;
		}
		else{
			RETURN_TRUE;
		}
	}
	else{
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ ip2location_get_country_short(handle, "ip_address")
 * Returns ip address's country in short */
PHP_FUNCTION(ip2location_get_country_short)
{
	char *ip_address;
	int ip_len;
	IP2LocationRecord *record = NULL;
	IP2Location *ip2location_ptr;
	long handle;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ls", &handle, &ip_address, &ip_len) == FAILURE) { 
		return;
	}
	
	ip2location_ptr = (IP2Location*)zend_fetch_resource(NULL, handle, PHP_IP2LOCATION_PTR_RES_NAME,NULL,1, le_ip2location_ptr);
	PHP_IP2LOCATION_INVALID_HANDLE_CHECK

	record = IP2Location_get_country_short(ip2location_ptr, ip_address);
	RETVAL_STRING(record->country_short, 1);
	IP2Location_free_record(record);
}
/* }}} */

/* {{{ ip2location_get_country_long(handle, "ip_address") 
 * Returns ip address's country in long */
PHP_FUNCTION(ip2location_get_country_long)
{
	char *ip_address;
	int ip_len;
	IP2LocationRecord *record = NULL;
	IP2Location *ip2location_ptr;
	long handle;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ls", &handle, &ip_address, &ip_len) == FAILURE) {
		return;
	} 
	
	ip2location_ptr = (IP2Location*)zend_fetch_resource(NULL, handle, PHP_IP2LOCATION_PTR_RES_NAME,NULL,1, le_ip2location_ptr);
	PHP_IP2LOCATION_INVALID_HANDLE_CHECK
	
	record = IP2Location_get_country_long(ip2location_ptr, ip_address);
	RETVAL_STRING(record->country_long, 1);
	IP2Location_free_record(record);
}
/* }}} */

/* {{{ ip2location_get_region(handle, "ip_address") 
 * Returns ip address's region*/
PHP_FUNCTION(ip2location_get_region)
{
	char *ip_address;
	int ip_len;
	IP2LocationRecord *record = NULL;
	IP2Location *ip2location_ptr;
	long handle;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ls", &handle, &ip_address, &ip_len) == FAILURE) {
		return;
	}
	ip2location_ptr = (IP2Location*)zend_fetch_resource(NULL, handle, PHP_IP2LOCATION_PTR_RES_NAME,NULL,1, le_ip2location_ptr);
	PHP_IP2LOCATION_INVALID_HANDLE_CHECK
	
	record = IP2Location_get_region(ip2location_ptr, ip_address);
	RETVAL_STRING(record->region, 1);
	IP2Location_free_record(record);
}
/* }}} */

/* {{{ ip2location_get_city(handle, "ip_address") 
 * Returns ip address's city*/
PHP_FUNCTION(ip2location_get_city)
{
	char *ip_address;
	int ip_len;
	IP2LocationRecord *record = NULL;
	IP2Location *ip2location_ptr;
	long handle;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ls", &handle, &ip_address, &ip_len) == FAILURE) {
		return;
	}
	ip2location_ptr = (IP2Location*)zend_fetch_resource(NULL, handle, PHP_IP2LOCATION_PTR_RES_NAME,NULL,1, le_ip2location_ptr);
	PHP_IP2LOCATION_INVALID_HANDLE_CHECK
	
	record = IP2Location_get_city(ip2location_ptr, ip_address);
	RETVAL_STRING(record->city, 1);
	IP2Location_free_record(record);
}
/* }}} */

/* {{{ ip2location_get_isp(handle, "ip_address") 
 * Returns ip address's isp information */
PHP_FUNCTION(ip2location_get_isp)
{
	char *ip_address;
	int ip_len;
	IP2LocationRecord *record = NULL;
	IP2Location *ip2location_ptr;
	long handle;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ls", &handle, &ip_address, &ip_len) == FAILURE) {
		return;
	}
	ip2location_ptr = (IP2Location*)zend_fetch_resource(NULL, handle, PHP_IP2LOCATION_PTR_RES_NAME,NULL,1, le_ip2location_ptr);
	PHP_IP2LOCATION_INVALID_HANDLE_CHECK

	record = IP2Location_get_isp(ip2location_ptr, ip_address);
	RETVAL_STRING(record->isp, 1);
	IP2Location_free_record(record);
}
/* }}} */

/* {{{ ip2location_get_latitude(handle, "ip_address") 
 * Returns ip address's latitude */
PHP_FUNCTION(ip2location_get_latitude)
{
	char *ip_address;
	int ip_len;
	IP2LocationRecord *record = NULL;
	IP2Location *ip2location_ptr;
	long handle;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ls", &handle, &ip_address, &ip_len) == FAILURE) {
		return;
	}
	ip2location_ptr = (IP2Location*)zend_fetch_resource(NULL, handle, PHP_IP2LOCATION_PTR_RES_NAME,NULL,1, le_ip2location_ptr);
	PHP_IP2LOCATION_INVALID_HANDLE_CHECK

	record = IP2Location_get_latitude(ip2location_ptr, ip_address);
	RETVAL_DOUBLE(record->latitude);
	IP2Location_free_record(record);
}
/* }}} */

/* {{{ ip2location_get_longitude(handle, "ip_address") 
 * Returns ip address's longitude  information */
PHP_FUNCTION(ip2location_get_longitude)
{
	char *ip_address;
	int ip_len;
	IP2LocationRecord *record = NULL;
	IP2Location *ip2location_ptr;
	long handle;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ls", &handle, &ip_address, &ip_len) == FAILURE) {
		return;
	}
	ip2location_ptr = (IP2Location*)zend_fetch_resource(NULL, handle, PHP_IP2LOCATION_PTR_RES_NAME,NULL,1, le_ip2location_ptr);
	PHP_IP2LOCATION_INVALID_HANDLE_CHECK

	record = IP2Location_get_longitude(ip2location_ptr, ip_address);
	RETVAL_DOUBLE(record->longitude);
	IP2Location_free_record(record);
}
/* }}} */

/* {{{ ip2location_get_domain(handle, "ip_address") 
 * Returns ip address's domain information */
PHP_FUNCTION(ip2location_get_domain)
{
	char *ip_address;
	int ip_len;
	IP2LocationRecord *record = NULL;
	IP2Location *ip2location_ptr;
	long handle;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ls", &handle, &ip_address, &ip_len) == FAILURE) {
		return;
	}
	ip2location_ptr = (IP2Location*)zend_fetch_resource(NULL, handle, PHP_IP2LOCATION_PTR_RES_NAME,NULL,1, le_ip2location_ptr);
	PHP_IP2LOCATION_INVALID_HANDLE_CHECK

	record = IP2Location_get_domain(ip2location_ptr, ip_address);
	RETVAL_STRING(record->domain, 1);
	IP2Location_free_record(record);
}
/* }}} */

/* {{{ ip2location_get_zipcode(handle, "ip_address") 
 * Returns ip address's zipcode information */
PHP_FUNCTION(ip2location_get_zipcode)
{
	char *ip_address;
	int ip_len;
	IP2LocationRecord *record = NULL;
	IP2Location *ip2location_ptr;
	long handle;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ls", &handle, &ip_address, &ip_len) == FAILURE) {
		return;
	}
	ip2location_ptr = (IP2Location*)zend_fetch_resource(NULL, handle, PHP_IP2LOCATION_PTR_RES_NAME,NULL,1, le_ip2location_ptr);
	PHP_IP2LOCATION_INVALID_HANDLE_CHECK

	record = IP2Location_get_zipcode(ip2location_ptr, ip_address);
	RETVAL_STRING(record->zipcode, 1);
	IP2Location_free_record(record);
}
/* }}} */

/* {{{ ip2location_get_timezone(handle, "ip_address") 
 * Returns ip address's timezone information */
PHP_FUNCTION(ip2location_get_timezone)
{
	char *ip_address;
	int ip_len;
	IP2LocationRecord *record = NULL;
	IP2Location *ip2location_ptr;
	long handle;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ls", &handle, &ip_address, &ip_len) == FAILURE) {
		return;
	}
	ip2location_ptr = (IP2Location*)zend_fetch_resource(NULL, handle, PHP_IP2LOCATION_PTR_RES_NAME,NULL,1, le_ip2location_ptr);
	PHP_IP2LOCATION_INVALID_HANDLE_CHECK

	record = IP2Location_get_timezone(ip2location_ptr, ip_address);
	RETVAL_STRING(record->timezone, 1);
	IP2Location_free_record(record);
}
/* }}} */

/* {{{ ip2location_get_netspeed(handle, "ip_address") 
 * Returns ip address's netspeed information */
PHP_FUNCTION(ip2location_get_netspeed)
{
	char *ip_address;
	int ip_len;
	IP2LocationRecord *record = NULL;
	IP2Location *ip2location_ptr;
	long handle;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ls", &handle, &ip_address, &ip_len) == FAILURE) {
		return;
	}
	ip2location_ptr = (IP2Location*)zend_fetch_resource(NULL, handle, PHP_IP2LOCATION_PTR_RES_NAME,NULL,1, le_ip2location_ptr);
	PHP_IP2LOCATION_INVALID_HANDLE_CHECK

	record = IP2Location_get_netspeed(ip2location_ptr, ip_address);
	RETVAL_STRING(record->netspeed, 1);
	IP2Location_free_record(record);
}
/* }}} */

/* {{{ ip2location_get_iddcode(handle, "ip_address") 
 * Returns ip address's iddcode information */
PHP_FUNCTION(ip2location_get_iddcode)
{
	char *ip_address;
	int ip_len;
	IP2LocationRecord *record = NULL;
	IP2Location *ip2location_ptr;
	long handle;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ls", &handle, &ip_address, &ip_len) == FAILURE) {
		return;
	}
	ip2location_ptr = (IP2Location*)zend_fetch_resource(NULL, handle, PHP_IP2LOCATION_PTR_RES_NAME,NULL,1, le_ip2location_ptr);
	PHP_IP2LOCATION_INVALID_HANDLE_CHECK

	record = IP2Location_get_iddcode(ip2location_ptr, ip_address);
	RETVAL_STRING(record->iddcode, 1);
	IP2Location_free_record(record);
}
/* }}} */

/* {{{ ip2location_get_areacode(handle, "ip_address") 
 * Returns ip address's areacode  information */
PHP_FUNCTION(ip2location_get_areacode)
{
	char *ip_address;
	int ip_len;
	IP2LocationRecord *record = NULL;
	IP2Location *ip2location_ptr;
	long handle;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ls", &handle, &ip_address, &ip_len) == FAILURE) {
		return;
	}
	ip2location_ptr = (IP2Location*)zend_fetch_resource(NULL, handle, PHP_IP2LOCATION_PTR_RES_NAME,NULL,1, le_ip2location_ptr);
	PHP_IP2LOCATION_INVALID_HANDLE_CHECK

	record = IP2Location_get_areacode(ip2location_ptr, ip_address);
	RETVAL_STRING(record->areacode, 1);
	IP2Location_free_record(record);
}
/* }}} */

/* {{{ ip2location_get_weatherstationcode(handle, "ip_address") 
 * Returns ip address's weatherstationcode information */
PHP_FUNCTION(ip2location_get_weatherstationcode)
{
	char *ip_address;
	int ip_len;
	IP2LocationRecord *record = NULL;
	IP2Location *ip2location_ptr;
	long handle;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ls", &handle, &ip_address, &ip_len) == FAILURE) {
		return;
	}
	ip2location_ptr = (IP2Location*)zend_fetch_resource(NULL, handle, PHP_IP2LOCATION_PTR_RES_NAME,NULL,1, le_ip2location_ptr);
	PHP_IP2LOCATION_INVALID_HANDLE_CHECK

	record = IP2Location_get_weatherstationcode(ip2location_ptr, ip_address);
	RETVAL_STRING(record->weatherstationcode, 1);
	IP2Location_free_record(record);
}
/* }}} */

/* {{{ ip2location_get_weatherstationname(handle, "ip_address") 
 * Returns ip address's weatherstationname information */
PHP_FUNCTION(ip2location_get_weatherstationname)
{
	char *ip_address;
	int ip_len;
	IP2LocationRecord *record = NULL;
	IP2Location *ip2location_ptr;
	long handle;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ls", &handle, &ip_address, &ip_len) == FAILURE) {
		return;
	}
	ip2location_ptr = (IP2Location*)zend_fetch_resource(NULL, handle, PHP_IP2LOCATION_PTR_RES_NAME,NULL,1, le_ip2location_ptr);
	PHP_IP2LOCATION_INVALID_HANDLE_CHECK

	record = IP2Location_get_weatherstationname(ip2location_ptr, ip_address);
	RETVAL_STRING(record->weatherstationname, 1);
	IP2Location_free_record(record);
}
/* }}} */

/* {{{ ip2location_get_mcc(handle, "ip_address")
 * Returns ip address's mcc information */
PHP_FUNCTION(ip2location_get_mcc)
{
	char *ip_address;
	int ip_len;
	IP2LocationRecord *record = NULL;
	IP2Location *ip2location_ptr;
	long handle;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ls", &handle, &ip_address, &ip_len) == FAILURE) {
		return;
	}
	ip2location_ptr = (IP2Location*)zend_fetch_resource(NULL, handle, PHP_IP2LOCATION_PTR_RES_NAME,NULL,1, le_ip2location_ptr);
	PHP_IP2LOCATION_INVALID_HANDLE_CHECK

	record = IP2Location_get_mcc(ip2location_ptr, ip_address);
	RETVAL_STRING(record->mcc, 1);
	IP2Location_free_record(record);
}
/* }}} */

/* {{{ ip2location_get_mnc(handle, "ip_address") 
 * Returns ip address's mnc information */
PHP_FUNCTION(ip2location_get_mnc)
{
	char *ip_address;
	int ip_len;
	IP2LocationRecord *record = NULL;
	IP2Location *ip2location_ptr;
	long handle;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ls", &handle, &ip_address, &ip_len) == FAILURE) {
		return;
	}
	ip2location_ptr = (IP2Location*)zend_fetch_resource(NULL, handle, PHP_IP2LOCATION_PTR_RES_NAME,NULL,1, le_ip2location_ptr);
	PHP_IP2LOCATION_INVALID_HANDLE_CHECK

	record = IP2Location_get_mnc(ip2location_ptr, ip_address);
	RETVAL_STRING(record->mnc, 1);
	IP2Location_free_record(record);
}
/* }}} */

/* {{{ ip2location_get_mobilebrand(handle, "ip_address") 
 * Returns ip address's mobilebrand information */
PHP_FUNCTION(ip2location_get_mobilebrand)
{
	char *ip_address;
	int ip_len;
	IP2LocationRecord *record = NULL;
	IP2Location *ip2location_ptr;
	long handle;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ls", &handle, &ip_address, &ip_len) == FAILURE) {
		return;
	}
	ip2location_ptr = (IP2Location*)zend_fetch_resource(NULL, handle, PHP_IP2LOCATION_PTR_RES_NAME,NULL,1, le_ip2location_ptr);
	PHP_IP2LOCATION_INVALID_HANDLE_CHECK

	record = IP2Location_get_mobilebrand(ip2location_ptr, ip_address);
	RETVAL_STRING(record->mobilebrand, 1);
	IP2Location_free_record(record);
}
/* }}} */

/* {{{ ip2location_get_elevation(handle, "ip_address") 
 * Returns ip address's elevation  information */
PHP_FUNCTION(ip2location_get_elevation)
{
	char *ip_address;
	int ip_len;
	IP2LocationRecord *record = NULL;
	IP2Location *ip2location_ptr;
	long handle;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ls", &handle, &ip_address, &ip_len) == FAILURE) {
		return;
	}
	ip2location_ptr = (IP2Location*)zend_fetch_resource(NULL, handle, PHP_IP2LOCATION_PTR_RES_NAME,NULL,1, le_ip2location_ptr);
	PHP_IP2LOCATION_INVALID_HANDLE_CHECK

	record = IP2Location_get_elevation(ip2location_ptr, ip_address);
	RETVAL_DOUBLE(record->elevation);
	IP2Location_free_record(record);
}
/* }}} */

/* {{{ ip2location_get_usagetype(handle, "ip_address")
 * Returns ip address's internet connection usagetype information */
PHP_FUNCTION(ip2location_get_usagetype)
{
	char *ip_address;
	int ip_len;
	IP2LocationRecord *record = NULL;
	IP2Location *ip2location_ptr;
	long handle;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ls", &handle, &ip_address, &ip_len) == FAILURE) {
		return;
	}
	ip2location_ptr = (IP2Location*)zend_fetch_resource(NULL, handle, PHP_IP2LOCATION_PTR_RES_NAME,NULL,1, le_ip2location_ptr);
	PHP_IP2LOCATION_INVALID_HANDLE_CHECK

	record = IP2Location_get_usagetype(ip2location_ptr, ip_address);
	RETVAL_STRING(record->usagetype, 1);
	IP2Location_free_record(record);
}
/* }}} */

/* {{{ ip2location_get_all(handle, "ip_address")
 *  * Returns the record information */
PHP_FUNCTION(ip2location_get_all)
{
	char *ip_address;
	int ip_len;
	IP2LocationRecord *record = NULL;
	IP2Location *ip2location_ptr;
	long handle;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ls", &handle, &ip_address, &ip_len) == FAILURE) {
		return;
	}
	ip2location_ptr = (IP2Location*)zend_fetch_resource(NULL, handle, PHP_IP2LOCATION_PTR_RES_NAME,NULL,1, le_ip2location_ptr);
	PHP_IP2LOCATION_INVALID_HANDLE_CHECK

	record = IP2Location_get_all(ip2location_ptr, ip_address);
	array_init(return_value);
	add_assoc_string(return_value, "country_short", record->country_short, 1 );
	add_assoc_string(return_value, "country_long", record->country_long, 1 );
	add_assoc_string(return_value, "region",record->region, 1 );
	add_assoc_string(return_value, "city",record->city, 1 );
	add_assoc_string(return_value, "isp",record->isp, 1 );
	add_assoc_double(return_value, "latitude",record->latitude );
	add_assoc_double(return_value, "longitude",record->longitude );
	add_assoc_string(return_value, "domain",record->domain, 1 );
	add_assoc_string(return_value, "zipcode",record->zipcode, 1 );
	add_assoc_string(return_value, "timezone",record->timezone, 1 );
	add_assoc_string(return_value, "netspeed",record->netspeed, 1 );
	add_assoc_string(return_value, "iddcode",record->iddcode, 1 );
	add_assoc_string(return_value, "areacode",record->areacode, 1 );
	add_assoc_string(return_value, "weatherstationcode",record->weatherstationcode, 1 );
	add_assoc_string(return_value, "weatherstationname",record->weatherstationname, 1 );
	add_assoc_string(return_value, "mcc",record->mcc, 1 );
	add_assoc_string(return_value, "mnc",record->mnc, 1 );
	add_assoc_string(return_value, "mobilebrand",record->mobilebrand, 1 );
	add_assoc_double(return_value, "elevation",record->elevation);
	add_assoc_string(return_value, "usagetype",record->usagetype, 1 );
	IP2Location_free_record(record);
}
/* }}} */


/* {{{ ip2location_close(handle)
 * Returns the record information */
PHP_FUNCTION(ip2location_close)
{
	IP2Location *ip2location_ptr;
	long handle;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &handle) == FAILURE) {
		return;
	}
	ip2location_ptr = (IP2Location*)zend_fetch_resource(NULL, handle, PHP_IP2LOCATION_PTR_RES_NAME,NULL,1, le_ip2location_ptr);
	PHP_IP2LOCATION_INVALID_HANDLE_CHECK

	IP2Location_close(ip2location_ptr);
	zend_list_delete(handle);
}
/* }}} */

/* {{{ ip2location_delete_shm()
 * Returns nothing */
PHP_FUNCTION(ip2location_delete_shm)
{
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "") != SUCCESS) {
		return;
	}
	IP2Location_delete_shm();
}
/* }}} */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
