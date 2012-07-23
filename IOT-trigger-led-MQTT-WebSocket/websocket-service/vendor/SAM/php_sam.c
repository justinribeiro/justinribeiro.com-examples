/*
+----------------------------------------------------------------------+
| Copyright IBM Corporation 2006.                                      |
| All Rights Reserved.                                                 |
+----------------------------------------------------------------------+
|                                                                      |
| Licensed under the Apache License, Version 2.0 (the "License"); you  |
| may not use this file except in compliance with the License. You may |
| obtain a copy of the License at                                      |
| http://www.apache.org/licenses/LICENSE-2.0                           |
|                                                                      |
| Unless required by applicable law or agreed to in writing, software  |
| distributed under the License is distributed on an "AS IS" BASIS,    |
| WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or      |
| implied. See the License for the specific language governing         |
| permissions and limitations under the License.                       |
+----------------------------------------------------------------------+
| Authors: Dave Renshaw, Jerry Stevens                                 |
+----------------------------------------------------------------------+

$Id: php_sam.c,v 1.5 2007/02/02 15:38:53 dsr Exp $

*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

/* The Zend API includes */
#include <php.h>
#include <php_ini.h>
#include <ext/standard/info.h>

/* Our local implementation requirements */
#include "php_sam.h"


/* ZEND_DECLARE_MODULE_GLOBALS(sam)  */

/*
*
* Every user visible function must have an entry in sam_functions[].
*/
static function_entry sam_functions[] = {
    {NULL, NULL, NULL}  /* Must be the last line in sam_functions[] */
};

/* {{{ SAM module entry
*/
zend_module_entry sam_module_entry = {
    STANDARD_MODULE_HEADER,
        SAM__MODULE_NAME,          /* Name by which this module is known in PHP */
        sam_functions,             /* Global variable containing module entry points */
        PHP_MINIT(sam),            /* Function to execute at module initialise */
        NULL,                      /* Function to execute at module shutdown */
        PHP_RINIT(sam),            /* Replace with NULL if there's nothing to do at request start */
        NULL,                      /* Replace with NULL if there's nothing to do at request end */
        PHP_MINFO(sam),            /* Used to dump out module information */
        SAM__MODULE_VERSION,       /* Version number of this extension */
        STANDARD_MODULE_PROPERTIES
};
/* }}} */

#ifdef COMPILE_DL_SAM
ZEND_GET_MODULE(sam)
#endif

/***************************************************************************/
/*                                                                         */
/* Module initialisation routine                                           */
/*                                                                         */
/***************************************************************************/
/* {{{ PHP_MINIT_FUNCTION
*/
PHP_MINIT_FUNCTION(sam)
{
    /* define some constants...                       */

    /* property types                                                         */
    REGISTER_STRING_CONSTANT("SAM_BOOL", SAM_CONSTANT_BOOLEAN, CONST_CS | CONST_PERSISTENT);
    REGISTER_STRING_CONSTANT("SAM_BYTE", SAM_CONSTANT_BYTE, CONST_CS | CONST_PERSISTENT);
    REGISTER_STRING_CONSTANT("SAM_DOUBLE", SAM_CONSTANT_DOUBLE, CONST_CS | CONST_PERSISTENT);
    REGISTER_STRING_CONSTANT("SAM_FLOAT", SAM_CONSTANT_FLOAT, CONST_CS | CONST_PERSISTENT);
    REGISTER_STRING_CONSTANT("SAM_INT", SAM_CONSTANT_INTEGER, CONST_CS | CONST_PERSISTENT);
    REGISTER_STRING_CONSTANT("SAM_LONG", SAM_CONSTANT_LONG, CONST_CS | CONST_PERSISTENT);
    REGISTER_STRING_CONSTANT("SAM_STRING", SAM_CONSTANT_STRING, CONST_CS | CONST_PERSISTENT);

    /* property names                                                         */
    /*  These constants are defeined with the value the same as the name to   */
    /*  get round some unexpected behaviour in the PHP parsing of certain     */
    /*  statements. In particular  $msg->header->SAM_XXX will always pass in  */
    /* a property name of SAM_XXX rather than the value of the constant named */
    /* SAM_XXX!                                                               */
    REGISTER_STRING_CONSTANT(SAM_CONSTANT_BROKER, SAM_CONSTANT_BROKER, CONST_CS | CONST_PERSISTENT);
    REGISTER_STRING_CONSTANT(SAM_CONSTANT_BUS, SAM_CONSTANT_BUS, CONST_CS | CONST_PERSISTENT);
    REGISTER_STRING_CONSTANT(SAM_CONSTANT_CORRELID, SAM_CONSTANT_CORRELID, CONST_CS | CONST_PERSISTENT);
    REGISTER_STRING_CONSTANT(SAM_CONSTANT_DELIVERYMODE, SAM_CONSTANT_DELIVERYMODE, CONST_CS | CONST_PERSISTENT);
    REGISTER_STRING_CONSTANT(SAM_CONSTANT_ENDPOINTS, SAM_CONSTANT_ENDPOINTS, CONST_CS | CONST_PERSISTENT);
    REGISTER_STRING_CONSTANT(SAM_CONSTANT_HOST, SAM_CONSTANT_HOST, CONST_CS | CONST_PERSISTENT);
    REGISTER_STRING_CONSTANT(SAM_CONSTANT_MESSAGEID, SAM_CONSTANT_MESSAGEID, CONST_CS | CONST_PERSISTENT);
    REGISTER_STRING_CONSTANT(SAM_CONSTANT_MSGPRIORITY, SAM_CONSTANT_MSGPRIORITY, CONST_CS | CONST_PERSISTENT);
    REGISTER_STRING_CONSTANT(SAM_CONSTANT_PASSWORD, SAM_CONSTANT_PASSWORD, CONST_CS | CONST_PERSISTENT);
    REGISTER_STRING_CONSTANT(SAM_CONSTANT_PORT, SAM_CONSTANT_PORT, CONST_CS | CONST_PERSISTENT);
    REGISTER_STRING_CONSTANT(SAM_CONSTANT_REPLY_TO, SAM_CONSTANT_REPLY_TO, CONST_CS | CONST_PERSISTENT);
    REGISTER_STRING_CONSTANT(SAM_CONSTANT_TARGETCHAIN, SAM_CONSTANT_TARGETCHAIN, CONST_CS | CONST_PERSISTENT);
    REGISTER_STRING_CONSTANT(SAM_CONSTANT_TRANSACTIONS, SAM_CONSTANT_TRANSACTIONS, CONST_CS | CONST_PERSISTENT);
    REGISTER_STRING_CONSTANT(SAM_CONSTANT_TYPE, SAM_CONSTANT_TYPE, CONST_CS | CONST_PERSISTENT);
    REGISTER_STRING_CONSTANT(SAM_CONSTANT_USERID, SAM_CONSTANT_USERID, CONST_CS | CONST_PERSISTENT);
    REGISTER_STRING_CONSTANT(SAM_CONSTANT_WAIT, SAM_CONSTANT_WAIT, CONST_CS | CONST_PERSISTENT);

    /* property values                                                        */
    REGISTER_STRING_CONSTANT("SAM_AUTO", SAM_CONSTANT_AUTO, CONST_CS | CONST_PERSISTENT);
    REGISTER_STRING_CONSTANT(SAM_CONSTANT_BYTES, SAM_CONSTANT_BYTES, CONST_CS | CONST_PERSISTENT);
    REGISTER_STRING_CONSTANT("SAM_MANUAL", SAM_CONSTANT_MANUAL, CONST_CS | CONST_PERSISTENT);
    REGISTER_STRING_CONSTANT("SAM_NON_PERSISTENT", SAM_CONSTANT_NONPERSISTENT, CONST_CS | CONST_PERSISTENT);
    REGISTER_STRING_CONSTANT("SAM_PERSISTENT", SAM_CONSTANT_PERSISTENT, CONST_CS | CONST_PERSISTENT);
    REGISTER_STRING_CONSTANT(SAM_CONSTANT_TEXT, SAM_CONSTANT_TEXT, CONST_CS | CONST_PERSISTENT);
    REGISTER_STRING_CONSTANT(SAM_CONSTANT_UNKNOWN, SAM_CONSTANT_UNKNOWN, CONST_CS | CONST_PERSISTENT);

    /* protocols                                                              */
    REGISTER_STRING_CONSTANT("SAM_RTT", SAM_CONSTANT_RTT, CONST_CS | CONST_PERSISTENT);
    REGISTER_STRING_CONSTANT("SAM_WMQ", SAM_CONSTANT_WMQ, CONST_CS | CONST_PERSISTENT);
    REGISTER_STRING_CONSTANT("SAM_WMQ_CLIENT", SAM_CONSTANT_WMQ_CLIENT, CONST_CS | CONST_PERSISTENT);
    REGISTER_STRING_CONSTANT("SAM_WMQ_BINDINGS", SAM_CONSTANT_WMQ_BINDINGS, CONST_CS | CONST_PERSISTENT);
    REGISTER_STRING_CONSTANT("SAM_WPM", SAM_CONSTANT_WPM, CONST_CS | CONST_PERSISTENT);

    /* Error constants */
    REGISTER_LONG_CONSTANT(SAM_CONSTANT_RECEIVE_TIMEDOUT, 500, CONST_CS | CONST_PERSISTENT);

    return SUCCESS;
}
/* }}} */



/* {{{ PHP_MINFO_FUNCTION
*/
PHP_MINFO_FUNCTION(sam)
{
    php_info_print_table_start();
    php_info_print_table_header(2, "SAM - Simple Asynchronous Messaging support", "enabled");
    php_info_print_table_row(2, "SAM extension version", SAM__MODULE_VERSION);
    php_info_print_table_end();
}
/* }}} */


/* {{{ PHP_RINIT_FUNCTION
*/
PHP_RINIT_FUNCTION(sam)
{
    if (zend_eval_string("require_once(\"SAM/php_sam.php\");", NULL, "sam_factory" TSRMLS_CC) != SUCCESS) {
        /* set the standard error no for unsupported protocol   */
        return FAILURE;
    }
    return SUCCESS;
}
/* }}} */

