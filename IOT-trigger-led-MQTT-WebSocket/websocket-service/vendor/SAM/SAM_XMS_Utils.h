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

$Id: SAM_XMS_Utils.h,v 1.1 2007/02/02 15:40:00 dsr Exp $

*/

#if !defined(__SAM_XMS_PHP_UTILS_H__)
#define __SAM_XMS_PHP_UTILS_H__

int SAMXMSGetSignature(char **s, char *aDestination);
xmsCHAR *SAMXMSBuildSelectorString(char *correlId, char *messageId);
void setError(zval *objvar TSRMLS_DC, int errorCode, char *errorString);
void resetError(zval *objvar TSRMLS_DC);

#endif /*__SAM_XMS_PHP_UTILS_H__) */

