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

#if !defined(__SAM_XMS_PHP_H__)
#define __SAM_XMS_PHP_H__

/***************************************************************************/
/*                                                                         */
/* Import / export definitions                                             */
/*                                                                         */
/***************************************************************************/

#ifdef ZTS
#   include <TSRM.h>
#endif

#ifdef PHP_WIN32
#   define PHP_XMS_API __declspec(dllexport)
#else
#   define PHP_XMS_API
#endif

#include <xms.h>

/***************************************************************************/
/*                                                                         */
/* Global definitions used elsewhere. As these are global, they may be     */
/* referenced by other header files locally.                               */
/*                                                                         */
/***************************************************************************/

/*
 * This is the name by which the PHP module is known. The user tests for the
 * module being loaded with the code: if ( extension_loaded('sam_xms') ) { ... }
 */

#define SAM_XMS__MODULE_NAME            "sam_xms"
#define SAM_XMS__MODULE_VERSION         "1.1"

#define PHP_SAM_XMS_CONNECTION_RES_NAME "__SAM_XMS_Connection_pointer"

/*
 * Force the module entry point to be "extern"
 */
extern zend_module_entry sam_xms_module_entry;
#define phpext_sam_xms_ptr &sam_xms_module_entry

/***************************************************************************/
/*                                                                         */
/* Function prototypes for the module                                      */
/*                                                                         */
/***************************************************************************/

PHP_MINIT_FUNCTION(sam_xms);

PHP_MINFO_FUNCTION(sam_xms);

/***************************************************************************/
/*                                                                         */
/* Useful macros                                                           */
/*                                                                         */
/***************************************************************************/

/*
ZEND_BEGIN_MODULE_GLOBALS(sam_xms)
        long  global_value;
        char *global_string;
ZEND_END_MODULE_GLOBALS(sam_xms)
*/


#endif
