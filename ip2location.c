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

ZEND_DECLARE_MODULE_GLOBALS(ip2location)

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

/* {{{ php_ip2location_init_globals
 *  */ 
static void php_ip2location_init_globals(zend_ip2location_globals *ip2location_globals)
{
        ip2location_globals->ip2location_ptr = NULL;
}
/* }}} */


/* {{{ PHP_MINIT_FUNCTION
 *  */
PHP_MINIT_FUNCTION(ip2location)
{
        ZEND_INIT_MODULE_GLOBALS(ip2location, php_ip2location_init_globals, NULL);

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
         
        if (ZEND_NUM_ARGS()!=1 || zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &file_path, &path_len) == FAILURE) { 
                 WRONG_PARAM_COUNT;
        }
        IP2LOCATION_G(ip2location_ptr) = IP2Location_open(file_path);
        if (IP2LOCATION_G(ip2location_ptr)) {
                RETURN_TRUE; 
        }
        else {
                RETURN_FALSE;
        }
}
/* }}} */

/* {{{ ip2location_open_mem(method)
 * Returns the success or failure */
PHP_FUNCTION(ip2location_open_mem)
{
        int method;
        if (ZEND_NUM_ARGS()!=1 || zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &method) == FAILURE) { 
                 WRONG_PARAM_COUNT;
        }
        /*Shared memory method is not supported*/
        if( method == IP2LOCATION_FILE_IO || method == IP2LOCATION_CACHE_MEMORY || method == IP2LOCATION_SHARED_MEMORY ){
                if(IP2Location_open_mem(IP2LOCATION_G(ip2location_ptr), method) == -1){
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
/* {{{ ip2location_get_country_short("ip_address")
 * Returns ip address's country in short */
PHP_FUNCTION(ip2location_get_country_short)
{
        char *ip_address;
        int ip_len;
        IP2LocationRecord *record = NULL;
        
        if( IP2LOCATION_G(ip2location_ptr) == NULL ) {
                RETURN_STRING("First Call ip2location_open with ip address db file path.\n", 1);
        }
       
        if (ZEND_NUM_ARGS()!=1 || zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &ip_address, &ip_len) == FAILURE) { 
                 WRONG_PARAM_COUNT;
        }
        record = IP2Location_get_country_short(IP2LOCATION_G(ip2location_ptr), ip_address);
        RETURN_STRING(record->country_short, 1);
        IP2Location_free_record(record);
}
/* }}} */

/* {{{ ip2location_get_country_long("ip_address") 
 * Returns ip address's country in long */
PHP_FUNCTION(ip2location_get_country_long)
{
        char *ip_address;
        int ip_len;
        IP2LocationRecord *record = NULL;

        if( IP2LOCATION_G(ip2location_ptr) == NULL ) {
                RETURN_STRING("First Call ip2location_open with ip address db file path.\n", 1);
        }

        if (ZEND_NUM_ARGS()!=1 || zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &ip_address, &ip_len) == FAILURE) { 
                 WRONG_PARAM_COUNT;
        }
        record = IP2Location_get_country_long(IP2LOCATION_G(ip2location_ptr), ip_address);
        RETURN_STRING(record->country_long, 1);
        IP2Location_free_record(record);
}
/* }}} */

/* {{{ ip2location_get_region("ip_address") 
 * Returns ip address's region*/
PHP_FUNCTION(ip2location_get_region)
{
        char *ip_address;
        int ip_len;
        IP2LocationRecord *record = NULL;

        if( IP2LOCATION_G(ip2location_ptr) == NULL ) {
                RETURN_STRING("First Call ip2location_open with ip address db file path.\n", 1);
        }

        if (ZEND_NUM_ARGS()!=1 || zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &ip_address, &ip_len) == FAILURE) { 
                 WRONG_PARAM_COUNT;
        }
        record = IP2Location_get_region(IP2LOCATION_G(ip2location_ptr), ip_address);
        RETURN_STRING(record->region, 1);
        IP2Location_free_record(record);
}
/* }}} */

/* {{{ ip2location_get_city("ip_address") 
 * Returns ip address's city*/
PHP_FUNCTION(ip2location_get_city)
{
        char *ip_address;
        int ip_len;
        IP2LocationRecord *record = NULL;

        if( IP2LOCATION_G(ip2location_ptr) == NULL ) {
                RETURN_STRING("First Call ip2location_open with ip address db file path.\n", 1);
        }

        if (ZEND_NUM_ARGS()!=1 || zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &ip_address, &ip_len) == FAILURE) {
                 WRONG_PARAM_COUNT;
        }
        record = IP2Location_get_city(IP2LOCATION_G(ip2location_ptr), ip_address);
        RETURN_STRING(record->city, 1);
        IP2Location_free_record(record);
}
/* }}} */

/* {{{ ip2location_get_("ip_address") 
 * Returns ip address's isp information */
PHP_FUNCTION(ip2location_get_isp)
{
        char *ip_address;
        int ip_len;
        IP2LocationRecord *record = NULL;

        if( IP2LOCATION_G(ip2location_ptr) == NULL ) {
                RETURN_STRING("First Call ip2location_open with ip address db file path.\n", 1);
        }

        if (ZEND_NUM_ARGS()!=1 || zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &ip_address, &ip_len) == FAILURE) {
                 WRONG_PARAM_COUNT;
        }
        record = IP2Location_get_isp(IP2LOCATION_G(ip2location_ptr), ip_address);
        RETURN_STRING(record->isp, 1);
        IP2Location_free_record(record);
}
/* }}} */

/* {{{ ip2location_get_latitude("ip_address") 
 * Returns ip address's latitude */
PHP_FUNCTION(ip2location_get_latitude)
{
        char *ip_address;
        int ip_len;
        IP2LocationRecord *record = NULL;

        if( IP2LOCATION_G(ip2location_ptr) == NULL ) {
                RETURN_DOUBLE(-1.0);
        }

        if (ZEND_NUM_ARGS()!=1 || zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &ip_address, &ip_len) == FAILURE) {
                 WRONG_PARAM_COUNT;
        }
        record = IP2Location_get_latitude(IP2LOCATION_G(ip2location_ptr), ip_address);
        RETURN_DOUBLE(record->latitude);
        IP2Location_free_record(record);
}
/* }}} */

/* {{{ ip2location_get_longitude("ip_address") 
 * Returns ip address's longitude  information */
PHP_FUNCTION(ip2location_get_longitude)
{
        char *ip_address;
        int ip_len;
        IP2LocationRecord *record = NULL;

        if( IP2LOCATION_G(ip2location_ptr) == NULL ) {
                RETURN_DOUBLE(-1.0);
        }

        if (ZEND_NUM_ARGS()!=1 || zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &ip_address, &ip_len) == FAILURE) {
                 WRONG_PARAM_COUNT;
        }
        record = IP2Location_get_longitude(IP2LOCATION_G(ip2location_ptr), ip_address);
        RETURN_DOUBLE(record->longitude);
        IP2Location_free_record(record);
}
/* }}} */

/* {{{ ip2location_get_domain("ip_address") 
 * Returns ip address's domain information */
PHP_FUNCTION(ip2location_get_domain)
{
        char *ip_address;
        int ip_len;
        IP2LocationRecord *record = NULL;

        if( IP2LOCATION_G(ip2location_ptr) == NULL ) {
                RETURN_STRING("First Call ip2location_open with ip address db file path.\n", 1);
        }

        if (ZEND_NUM_ARGS()!=1 || zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &ip_address, &ip_len) == FAILURE) {
                 WRONG_PARAM_COUNT;
        }
        record = IP2Location_get_domain(IP2LOCATION_G(ip2location_ptr), ip_address);
        RETURN_STRING(record->domain, 1);
        IP2Location_free_record(record);
}
/* }}} */

/* {{{ ip2location_get_zipcode("ip_address") 
 * Returns ip address's zipcode information */
PHP_FUNCTION(ip2location_get_zipcode)
{
        char *ip_address;
        int ip_len;
        IP2LocationRecord *record = NULL;

        if( IP2LOCATION_G(ip2location_ptr) == NULL ) {
                RETURN_STRING("First Call ip2location_open with ip address db file path.\n", 1);
        }

        if (ZEND_NUM_ARGS()!=1 || zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &ip_address, &ip_len) == FAILURE) {
                 WRONG_PARAM_COUNT;
        }
        record = IP2Location_get_zipcode(IP2LOCATION_G(ip2location_ptr), ip_address);
        RETURN_STRING(record->zipcode, 1);
        IP2Location_free_record(record);
}
/* }}} */

/* {{{ ip2location_get_timezone("ip_address") 
 * Returns ip address's timezone information */
PHP_FUNCTION(ip2location_get_timezone)
{
        char *ip_address;
        int ip_len;
        IP2LocationRecord *record = NULL;

        if( IP2LOCATION_G(ip2location_ptr) == NULL ) {
                RETURN_STRING("First Call ip2location_open with ip address db file path.\n", 1);
        }

        if (ZEND_NUM_ARGS()!=1 || zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &ip_address, &ip_len) == FAILURE) {
                 WRONG_PARAM_COUNT;
        }
        record = IP2Location_get_timezone(IP2LOCATION_G(ip2location_ptr), ip_address);
        RETURN_STRING(record->timezone, 1);
        IP2Location_free_record(record);
}
/* }}} */

/* {{{ ip2location_get_netspeed("ip_address") 
 * Returns ip address's netspeed information */
PHP_FUNCTION(ip2location_get_netspeed)
{
        char *ip_address;
        int ip_len;
        IP2LocationRecord *record = NULL;

        if( IP2LOCATION_G(ip2location_ptr) == NULL ) {
                RETURN_STRING("First Call ip2location_open with ip address db file path.\n", 1);
        }

        if (ZEND_NUM_ARGS()!=1 || zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &ip_address, &ip_len) == FAILURE) {
                 WRONG_PARAM_COUNT;
        }
        record = IP2Location_get_netspeed(IP2LOCATION_G(ip2location_ptr), ip_address);
        RETURN_STRING(record->netspeed, 1);
        IP2Location_free_record(record);
}
/* }}} */

/* {{{ ip2location_get_iddcode("ip_address") 
 * Returns ip address's iddcode information */
PHP_FUNCTION(ip2location_get_iddcode)
{
        char *ip_address;
        int ip_len;
        IP2LocationRecord *record = NULL;

        if( IP2LOCATION_G(ip2location_ptr) == NULL ) {
                RETURN_STRING("First Call ip2location_open with ip address db file path.\n", 1);
        }

        if (ZEND_NUM_ARGS()!=1 || zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &ip_address, &ip_len) == FAILURE) {
                 WRONG_PARAM_COUNT;
        }
        record = IP2Location_get_iddcode(IP2LOCATION_G(ip2location_ptr), ip_address);
        RETURN_STRING(record->iddcode, 1);
        IP2Location_free_record(record);
}
/* }}} */

/* {{{ ip2location_get_areacode("ip_address") 
 * Returns ip address's areacode  information */
PHP_FUNCTION(ip2location_get_areacode)
{
        char *ip_address;
        int ip_len;
        IP2LocationRecord *record = NULL;

        if( IP2LOCATION_G(ip2location_ptr) == NULL ) {
                RETURN_STRING("First Call ip2location_open with ip address db file path.\n", 1);
        }

        if (ZEND_NUM_ARGS()!=1 || zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &ip_address, &ip_len) == FAILURE) {
                 WRONG_PARAM_COUNT;
        }
        record = IP2Location_get_areacode(IP2LOCATION_G(ip2location_ptr), ip_address);
        RETURN_STRING(record->areacode, 1);
        IP2Location_free_record(record);
}
/* }}} */

/* {{{ ip2location_get_weatherstationcode("ip_address") 
 * Returns ip address's weatherstationcode information */
PHP_FUNCTION(ip2location_get_weatherstationcode)
{
        char *ip_address;
        int ip_len;
        IP2LocationRecord *record = NULL;

        if( IP2LOCATION_G(ip2location_ptr) == NULL ) {
                RETURN_STRING("First Call ip2location_open with ip address db file path.\n", 1);
        }

        if (ZEND_NUM_ARGS()!=1 || zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &ip_address, &ip_len) == FAILURE) {
                 WRONG_PARAM_COUNT;
        }
        record = IP2Location_get_weatherstationcode(IP2LOCATION_G(ip2location_ptr), ip_address);
        RETURN_STRING(record->weatherstationcode, 1);
        IP2Location_free_record(record);
}
/* }}} */

/* {{{ ip2location_get_weatherstationname("ip_address") 
 * Returns ip address's weatherstationname information */
PHP_FUNCTION(ip2location_get_weatherstationname)
{
        char *ip_address;
        int ip_len;
        IP2LocationRecord *record = NULL;

        if( IP2LOCATION_G(ip2location_ptr) == NULL ) {
                RETURN_STRING("First Call ip2location_open with ip address db file path.\n", 1);
        }

        if (ZEND_NUM_ARGS()!=1 || zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &ip_address, &ip_len) == FAILURE) {
                 WRONG_PARAM_COUNT;
        }
        record = IP2Location_get_weatherstationname(IP2LOCATION_G(ip2location_ptr), ip_address);
        RETURN_STRING(record->weatherstationname, 1);
        IP2Location_free_record(record);
}
/* }}} */

/* {{{ ip2location_get_mcc("ip_address") 
 * Returns ip address's mcc information */
PHP_FUNCTION(ip2location_get_mcc)
{
        char *ip_address;
        int ip_len;
        IP2LocationRecord *record = NULL;

        if( IP2LOCATION_G(ip2location_ptr) == NULL ) {
                RETURN_STRING("First Call ip2location_open with ip address db file path.\n", 1);
        }

        if (ZEND_NUM_ARGS()!=1 || zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &ip_address, &ip_len) == FAILURE) {
                 WRONG_PARAM_COUNT;
        }
        record = IP2Location_get_mcc(IP2LOCATION_G(ip2location_ptr), ip_address);
        RETURN_STRING(record->mcc, 1);
        IP2Location_free_record(record);
}
/* }}} */

/* {{{ ip2location_get_mnc("ip_address") 
 * Returns ip address's mnc information */
PHP_FUNCTION(ip2location_get_mnc)
{
        char *ip_address;
        int ip_len;
        IP2LocationRecord *record = NULL;

        if( IP2LOCATION_G(ip2location_ptr) == NULL ) {
                RETURN_STRING("First Call ip2location_open with ip address db file path.\n", 1);
        }

        if (ZEND_NUM_ARGS()!=1 || zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &ip_address, &ip_len) == FAILURE) {
                 WRONG_PARAM_COUNT;
        }
        record = IP2Location_get_mnc(IP2LOCATION_G(ip2location_ptr), ip_address);
        RETURN_STRING(record->mnc, 1);
        IP2Location_free_record(record);
}
/* }}} */

/* {{{ ip2location_get_mobilebrand("ip_address") 
 * Returns ip address's mobilebrand information */
PHP_FUNCTION(ip2location_get_mobilebrand)
{
        char *ip_address;
        int ip_len;
        IP2LocationRecord *record = NULL;

        if( IP2LOCATION_G(ip2location_ptr) == NULL ) {
                RETURN_STRING("First Call ip2location_open with ip address db file path.\n", 1);
        }

        if (ZEND_NUM_ARGS()!=1 || zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &ip_address, &ip_len) == FAILURE) {
                 WRONG_PARAM_COUNT;
        }
        record = IP2Location_get_mobilebrand(IP2LOCATION_G(ip2location_ptr), ip_address);
        RETURN_STRING(record->mobilebrand, 1);
        IP2Location_free_record(record);
}
/* }}} */

/* {{{ ip2location_get_elevation("ip_address") 
 * Returns ip address's elevation  information */
PHP_FUNCTION(ip2location_get_elevation)
{
        char *ip_address;
        int ip_len;
        IP2LocationRecord *record = NULL;

        if( IP2LOCATION_G(ip2location_ptr) == NULL ) {
                RETURN_DOUBLE(-1.0);
        }

        if (ZEND_NUM_ARGS()!=1 || zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &ip_address, &ip_len) == FAILURE) {
                 WRONG_PARAM_COUNT;
        }
        record = IP2Location_get_elevation(IP2LOCATION_G(ip2location_ptr), ip_address);
        RETURN_DOUBLE(record->elevation);
        IP2Location_free_record(record);
}
/* }}} */

/* {{{ ip2location_get_usagetype("ip_address")
 * Returns ip address's internet connection usagetype information */
PHP_FUNCTION(ip2location_get_usagetype)
{
        char *ip_address;
        int ip_len;
        IP2LocationRecord *record = NULL;

        if( IP2LOCATION_G(ip2location_ptr) == NULL ) {
                RETURN_STRING("First Call ip2location_open with ip address db file path.\n", 1);
        }

        if (ZEND_NUM_ARGS()!=1 || zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &ip_address, &ip_len) == FAILURE) {
                 WRONG_PARAM_COUNT;
        }
        record = IP2Location_get_usagetype(IP2LOCATION_G(ip2location_ptr), ip_address);
        RETURN_STRING(record->usagetype, 1);
        IP2Location_free_record(record);
}
/* }}} */

/* {{{ ip2location_get_all("ip_address")
 *  * Returns the record information */
PHP_FUNCTION(ip2location_get_all)
{
        char *ip_address;
        int ip_len;
        IP2LocationRecord *record = NULL;
        zval *row;

        if (ZEND_NUM_ARGS()!=1 || zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &ip_address, &ip_len) == FAILURE) {
                 WRONG_PARAM_COUNT;
        }

        record = IP2Location_get_all(IP2LOCATION_G(ip2location_ptr), ip_address);
        array_init(return_value);
        ALLOC_INIT_ZVAL(row);
        array_init(row);
        add_assoc_string(row, "country_short", record->country_short, 1 );
        add_assoc_string(row, "country_long", record->country_long, 1 );
        add_assoc_string(row, "region",record->region, 1 );
        add_assoc_string(row, "city",record->city, 1 );
        add_assoc_string(row, "isp",record->isp, 1 );
        add_assoc_double(row, "latitude",record->latitude );
        add_assoc_double(row, "longitude",record->longitude );
        add_assoc_string(row, "domain",record->domain, 1 );
        add_assoc_string(row, "zipcode",record->zipcode, 1 );
        add_assoc_string(row, "timezone",record->timezone, 1 );
        add_assoc_string(row, "netspeed",record->netspeed, 1 );
        add_assoc_string(row, "iddcode",record->iddcode, 1 );
        add_assoc_string(row, "areacode",record->areacode, 1 );
        add_assoc_string(row, "weatherstationcode",record->weatherstationcode, 1 );
        add_assoc_string(row, "weatherstationname",record->weatherstationname, 1 );
        add_assoc_string(row, "mcc",record->mcc, 1 );
        add_assoc_string(row, "mnc",record->mnc, 1 );
        add_assoc_string(row, "mobilebrand",record->mobilebrand, 1 );
        add_assoc_double(row, "elevation",record->elevation);
        add_assoc_string(row, "usagetype",record->usagetype, 1 );

        add_index_zval(return_value, 0, row);
        printf("Test IP Address %s. We got %s \n",ip_address,record->country_short);
}
/* }}} */


/* {{{ ip2location_close()
 * Returns the record information */
PHP_FUNCTION(ip2location_close)
{
        if (ZEND_NUM_ARGS() !=0 ) { 
                 WRONG_PARAM_COUNT;
        }
        IP2Location_close(IP2LOCATION_G(ip2location_ptr));
        IP2LOCATION_G(ip2location_ptr) = NULL;
}
/* }}} */

/* {{{ ip2location_delete_shm()
 * Returns nothing */
PHP_FUNCTION(ip2location_delete_shm)
{
        if (ZEND_NUM_ARGS() !=0 ) { 
                 WRONG_PARAM_COUNT;
        }
        IP2Location_delete_shm();
}
/* }}} */