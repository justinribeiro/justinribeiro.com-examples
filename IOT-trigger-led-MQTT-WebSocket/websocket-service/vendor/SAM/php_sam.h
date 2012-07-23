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

   $Id: php_sam.h,v 1.5 2007/02/02 15:38:53 dsr Exp $

*/

#if !defined(__SAM_PHP_H__)
#define __SAM_PHP_H__

/***************************************************************************/
/*                                                                         */
/* Import / export definitions                                             */
/*                                                                         */
/***************************************************************************/

#ifdef ZTS
#   include <TSRM.h>
#endif

/***************************************************************************/
/*                                                                         */
/* Global definitions used elsewhere. As these are global, they may be     */
/* referenced by other header files locally.                               */
/*                                                                         */
/***************************************************************************/

/*
 * This is the name by which the PHP module is known. Theuser tests for the
 * module being loaded with the code: if ( extension_loaded('sam') ) { ... }
 */

#define SAM__MODULE_NAME            "sam"
#define SAM__MODULE_VERSION         "1.1"

/* SAM constants                                                          */
/* property types                                                         */
#define SAM_CONSTANT_BOOLEAN        "bool"
#define SAM_CONSTANT_BYTE           "byte"
#define SAM_CONSTANT_DOUBLE         "double"
#define SAM_CONSTANT_FLOAT          "float"
#define SAM_CONSTANT_INTEGER        "int"
#define SAM_CONSTANT_LONG           "long"
#define SAM_CONSTANT_STRING         "string"

/* property names                                                         */
#define SAM_CONSTANT_BROKER            "SAM_BROKER"
#define SAM_CONSTANT_BUS               "SAM_BUS"
#define SAM_CONSTANT_CORRELID          "SAM_CORRELID"
#define SAM_CONSTANT_DELIVERYMODE      "SAM_DELIVERYMODE"
#define SAM_CONSTANT_ENDPOINTS         "SAM_ENDPOINTS"
#define SAM_CONSTANT_WMQ_TARGET_CLIENT "SAM_WMQ_TARGET_CLIENT"
#define SAM_CONSTANT_HOST              "SAM_HOST"
#define SAM_CONSTANT_MESSAGEID         "SAM_MESSAGEID"
#define SAM_CONSTANT_MSGPRIORITY       "SAM_MSGPRIORITY"
#define SAM_CONSTANT_PASSWORD          "SAM_PASSWORD"
#define SAM_CONSTANT_PORT              "SAM_PORT"
#define SAM_CONSTANT_PRIORITY          "SAM_PRIORITY"
#define SAM_CONSTANT_REPLY_TO          "SAM_REPLY_TO"
#define SAM_CONSTANT_TARGETCHAIN       "SAM_TARGETCHAIN"
#define SAM_CONSTANT_TIMETOLIVE        "SAM_TIMETOLIVE"
#define SAM_CONSTANT_TRANSACTIONS      "SAM_TRANSACTIONS"
#define SAM_CONSTANT_TYPE              "SAM_TYPE"
#define SAM_CONSTANT_USERID            "SAM_USERID"
#define SAM_CONSTANT_WAIT              "SAM_WAIT"
#define SAM_CONSTANT_WPM_DUR_SUB_HOME  "SAM_WPM_DUR_SUB_HOME"

/* property values                                                         */
#define SAM_CONSTANT_AUTO           "SAM_AUTO"
#define SAM_CONSTANT_BYTES          "SAM_BYTES"
#define SAM_CONSTANT_WMQTARGETCLIENT_JMS  "jms"
#define SAM_CONSTANT_WMQTARGETCLIENT_MQ   "mq"
#define SAM_CONSTANT_MANUAL         "SAM_MANUAL"
#define SAM_CONSTANT_NONPERSISTENT  "SAM_NON_PERSISTENT"
#define SAM_CONSTANT_PERSISTENT     "SAM_PERSISTENT"
#define SAM_CONSTANT_TEXT           "SAM_TEXT"
#define SAM_CONSTANT_UNKNOWN        "SAM_UNKNOWN"

/* protocols                                                               */
#define SAM_CONSTANT_RTT            "rtt"
#define SAM_CONSTANT_WMQ            "wmq"
#define SAM_CONSTANT_WMQ_CLIENT     "wmq:client"
#define SAM_CONSTANT_WMQ_BINDINGS   "wmq:bindings"
#define SAM_CONSTANT_WPM            "wpm"
#define SAM_CONSTANT_SUB            "sub"
#define SAM_CONSTANT_TOPIC          "topic"
#define SAM_CONSTANT_QUEUE          "queue"
#define SAM_CONSTANT_SUBPREFIX      "sub://"
#define SAM_CONSTANT_TOPICPREFIX    "topic://"
#define SAM_CONSTANT_QUEUEPREFIX    "queue://"

#define SAM_CONSTANT_OPTION_CLIENT   "client"
#define SAM_CONSTANT_OPTION_BINDINGS "bindings"

/* Error constants */
#define SAM_CONSTANT_RECEIVE_TIMEDOUT       "SAM_RECEIVE_TIMEDOUT"

/*
 * Force the module entry point to be "extern"
 */
extern zend_module_entry sam_module_entry;
#define phpext_sam_ptr &sam_module_entry

/***************************************************************************/
/*                                                                         */
/* Function prototypes for the module                                      */
/*                                                                         */
/***************************************************************************/

PHP_MINIT_FUNCTION(sam);

PHP_MINFO_FUNCTION(sam);

PHP_RINIT_FUNCTION(sam);


/***************************************************************************/
/*                                                                         */
/* Useful macros                                                           */
/*                                                                         */
/***************************************************************************/

/*
ZEND_BEGIN_MODULE_GLOBALS(sam)
        long  global_value;
        char *global_string;
ZEND_END_MODULE_GLOBALS(sam)
*/


#endif
