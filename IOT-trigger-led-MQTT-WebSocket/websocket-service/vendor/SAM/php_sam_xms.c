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

$Id:

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
#include "php_sam_xms.h"
#include "SAM_XMS_Connection.h"

/* Global package debug control flag...    */
int SAM_DEBUG   = 0;

zend_class_entry *php_sam_SAM_XMS_Connection_entry;

/* Define the XMS connection resource destructor...    */
static void php_sam_xms_connection_dtor(zend_rsrc_list_entry *rsrc TSRMLS_DC);

/* Resource list entry        */
int le_sam_xms_connection;

/* ZEND_DECLARE_MODULE_GLOBALS(sam_xms)  */

/*
*
* Every user visible function must have an entry in xms_functions[].
*/
static function_entry sam_xms_functions[] = {
    {NULL, NULL, NULL}  /* Must be the last line in xms_functions[] */
};

/* {{{ SAM module entry
*/
zend_module_entry sam_xms_module_entry = {
    STANDARD_MODULE_HEADER,
        SAM_XMS__MODULE_NAME,    /* Name by which this module is known in PHP */
        sam_xms_functions,       /* Global variable containing module entry points */
        PHP_MINIT(sam_xms),      /* Function to execute at module initialise */
        NULL,                    /* Function to execute at module shutdown */
        NULL,                    /* Replace with NULL if there's nothing to do at request start */
        NULL,                    /* Replace with NULL if there's nothing to do at request end */
        PHP_MINFO(sam_xms),      /* Used to dump out module information */
        SAM_XMS__MODULE_VERSION, /* Version number of this extension */
        STANDARD_MODULE_PROPERTIES
};
/* }}} */

#ifdef COMPILE_DL_SAM_XMS
ZEND_GET_MODULE(sam_xms)
#endif

/***************************************************************************/
/*                                                                         */
/* Module initialisation routine                                           */
/*                                                                         */
/***************************************************************************/
/* {{{ PHP_MINIT_FUNCTION
*/
PHP_MINIT_FUNCTION(sam_xms)
{
    zend_class_entry ce;

    if (SAM_DEBUG) zend_printf("<br/><b>sam_xms PHP_MINIT_FUNCTION</b> enter<br/>");

    /* Register the XMS connection resource   */
    le_sam_xms_connection = zend_register_list_destructors_ex(php_sam_xms_connection_dtor, NULL,
        PHP_SAM_XMS_CONNECTION_RES_NAME, module_number);


    /* Register the connection class */
    INIT_CLASS_ENTRY(ce, "SAMXMSConnection", sam_xms_conn_functions);
    php_sam_SAM_XMS_Connection_entry = zend_register_internal_class(&ce TSRMLS_CC);

    if (SAM_DEBUG) zend_printf("<b>sam_xms PHP_MINIT_FUNCTION</b> exit<br/>");

    return SUCCESS;
}
/* }}} */



/* {{{ PHP_MINFO_FUNCTION
*/
PHP_MINFO_FUNCTION(sam_xms)
{
    php_info_print_table_start();
    php_info_print_table_header(2, "SAM XMS - Simple Asynchronous Messaging support", "enabled");
    php_info_print_table_row(2, "SAM XMS extension version", SAM__MODULE_VERSION);
    php_info_print_table_end();
}
/* }}} */

/* {{{ proto void php_sam_connection_dtor(zend_rsrc_list_entry)
* routine to close off any open XMS connections at module exit
*/
static void php_sam_xms_connection_dtor(zend_rsrc_list_entry *rsrc TSRMLS_DC) {

    php_sam_xms_connection *connResource = (php_sam_xms_connection*)rsrc->ptr;

    xmsHErrorBlock          errorBlock;

    if (SAM_DEBUG) zend_printf("<br/><b>php_sam_xms_connection_dtor</b> enter<br/>");

    /* Close the session and connection and free the resource memory  */
    if (connResource) {

        /* Close the session */
        if (XMS_NULL_HANDLE != connResource->session) {

            /* Create an error block for xmsSessClose */
            if (XMS_OK != xmsErrorCreate(&errorBlock)) {
                if (SAM_DEBUG) zend_printf("<br/><b>php_sam_xms_dtor()</b> Error creating error block\n");
            } else {
                /* Issue an explicit rollback before closing the session if the session was manually transacted */
                if (connResource->transacted) {
                    if (SAM_DEBUG) zend_printf("<b>php_sam_xms_connection_dtor</b> Issuing rollback<br/>");
                    if (XMS_OK != xmsSessRollback(connResource->session, errorBlock)) {
                        if (SAM_DEBUG) zend_printf("<br/><b>SAMXMSConnection->php_sam_xms_connection_dtor</b> Session rollback failed!\n");
                    }
                }

                if (SAM_DEBUG) zend_printf("<b>php_sam_xms_connection_dtor</b> Closing session<br/>");
                if (XMS_OK != xmsSessClose(&(connResource->session), errorBlock)) {
                    if (SAM_DEBUG) zend_printf("<br/><b>SAMXMSConnection->php_sam_xms_connection_dtor</b> Session close failed!\n");
                }
                /* Finished with the error block now */
                xmsErrorDispose(&errorBlock);
            }
        }

        if (connResource->conn) {
            if (SAM_DEBUG) zend_printf("<b>php_sam_xms_connection_dtor</b> closing connection %d<br/>", connResource->conn);
            if (XMS_OK != xmsConnClose(&connResource->conn, NULL)) {
                if (SAM_DEBUG) zend_printf("<br/><b>php_sam_xms_connection_dtor</b> close failed!<br/>");
            }

            /* Set the connection to NULL to eliminate the possibility of attempting to disconnect again */
            connResource->conn=NULL;
        }


        /* Free the resource */
        efree(connResource);
    }
    if (SAM_DEBUG) zend_printf("<b>php_sam_xms_connection_dtor</b> exit<br/>");
}
/* }}} */
