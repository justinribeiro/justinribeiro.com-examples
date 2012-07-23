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

$Id: SAM_XMS_Connection.c,v 1.1 2007/02/02 15:40:00 dsr Exp $

*/

/* Includes for the Zend PHP API */
#include <php.h>
#include <php_ini.h>
#include <ext/standard/info.h>

/* Includes for our implementation */
#include "php_sam.h"
#include "php_sam_xms.h"

#include "SAM_XMS_Connection.h"
#include "SAM_XMS_Utils.h"

/* Debug output control flag  (in php_sam_xms.c) */
extern int SAM_DEBUG;

/* Resource list entry  (in php_sam_xms.c) */
extern int le_sam_xms_connection;

/* Class entry for the SAM_Message class (in sam.c) */
extern zend_class_entry *php_sam_SAM_Message_entry;

/**
* \brief List of methods available to the PHP connection object.
*        Mapping is the PHP name of the function to the C function
*        name. Use the the same PHP method name as the PHP class name
*        to provide a custom constructor. Terminate this list with
*        three NULLs.
*/
function_entry sam_xms_conn_functions[] = {

    PHP_NAMED_FE(SAMXMSConnection, SAM_XMS_Connection_ctor,    NULL)
        PHP_NAMED_FE(commit, PHP_FN(SAM_XMS_Connection_commit) ,     NULL)
        PHP_NAMED_FE(connect, PHP_FN(SAM_XMS_Connection_connect) ,     NULL)
        PHP_NAMED_FE(disconnect, PHP_FN(SAM_XMS_Connection_disconnect) ,     NULL)
        PHP_NAMED_FE(isconnected, PHP_FN(SAM_XMS_Connection_isConnected) ,     NULL)
        PHP_NAMED_FE(peek, PHP_FN(SAM_XMS_Connection_peek),          NULL)
        PHP_NAMED_FE(peekAll, PHP_FN(SAM_XMS_Connection_peekAll),          NULL)
        PHP_NAMED_FE(receive, PHP_FN(SAM_XMS_Connection_receive),     NULL)
        PHP_NAMED_FE(remove, PHP_FN(SAM_XMS_Connection_remove),      NULL)
        PHP_NAMED_FE(rollback, PHP_FN(SAM_XMS_Connection_rollback),    NULL)
        PHP_NAMED_FE(send, PHP_FN(SAM_XMS_Connection_send),        NULL)
        PHP_NAMED_FE(setdebug, PHP_FN(SAM_XMS_Connection_setDebug),        NULL)
        PHP_NAMED_FE(subscribe, PHP_FN(SAM_XMS_Connection_subscribe),   NULL)
        PHP_NAMED_FE(unsubscribe, PHP_FN(SAM_XMS_Connection_unsubscribe), NULL)

    {NULL, NULL, NULL}
};


/***************************************************************************/
/*                                                                         */
/* Function implementations                                                */
/*                                                                         */
/***************************************************************************/
/*
* SYNOPSIS:   Returns the character string value for specified JMS type exception.
* PARAMETERS: exceptionType - JMS exception type
* RETURNS:    xmsCHAR *
*/

xmsCHAR * getExceptionTypeString(xmsJMSEXP_TYPE exceptionType)
{
    xmsCHAR *  pszExceptionType = NULL;

    switch (exceptionType)
    {
    case XMS_X_NO_EXCEPTION:

        pszExceptionType = (xmsCHAR *) "XMS_JMSEXP_TYPE_NONE";
        break;

    case XMS_X_GENERAL_EXCEPTION:

        pszExceptionType = (xmsCHAR *) "XMS_JMSEXP_TYPE_GENERALEXCEPTION";
        break;

    case XMS_X_ILLEGAL_STATE_EXCEPTION:

        pszExceptionType = (xmsCHAR *) "XMS_JMSEXP_TYPE_ILLEGALSTATEEXCEPTION";
        break;

    case XMS_X_INVALID_CLIENTID_EXCEPTION:

        pszExceptionType = (xmsCHAR *) "XMS_JMSEXP_TYPE_INVALIDCLIENTIDEXCEPTION";
        break;

    case XMS_X_INVALID_DESTINATION_EXCEPTION:

        pszExceptionType = (xmsCHAR *) "XMS_JMSEXP_TYPE_INVALIDDESTINATIONEXCEPTION";
        break;

    case XMS_X_INVALID_SELECTOR_EXCEPTION:

        pszExceptionType = (xmsCHAR *) "XMS_JMSEXP_TYPE_INVALIDSELECTOREXCEPTION";
        break;

    case XMS_X_MESSAGE_EOF_EXCEPTION:

        pszExceptionType = (xmsCHAR *) "XMS_JMSEXP_TYPE_MESSAGEEOFEXCEPTION";
        break;

    case XMS_X_MESSAGE_FORMAT_EXCEPTION:

        pszExceptionType = (xmsCHAR *) "XMS_JMSEXP_TYPE_MESSAGEFORMATEXCEPTION";
        break;

    case XMS_X_MESSAGE_NOT_READABLE_EXCEPTION:

        pszExceptionType = (xmsCHAR *) "XMS_JMSEXP_TYPE_MESSAGENOTREADABLEEXCEPTION";
        break;

    case XMS_X_MESSAGE_NOT_WRITEABLE_EXCEPTION:

        pszExceptionType = (xmsCHAR *) "XMS_JMSEXP_TYPE_MESSAGENOTWRITEABLEEXCEPTION";
        break;

    case XMS_X_RESOURCE_ALLOCATION_EXCEPTION:

        pszExceptionType = (xmsCHAR *) "XMS_JMSEXP_TYPE_RESOURCEALLOCATIONEXCEPTION";
        break;

    default:

        pszExceptionType = (xmsCHAR *) "";
        break;
    }

    return(pszExceptionType);
}


int check(xmsHErrorBlock hError) {

    xmsCHAR        szText[100]  = { '\0'};
    xmsINT         cbTextSize   = sizeof(szText) / sizeof(xmsCHAR);
    xmsCHAR        szData[1000] = { '\0'};
    xmsINT         cbDataSize   = sizeof(szData) / sizeof(xmsCHAR);
    xmsINT         cbActualSize = 0;
    xmsINT         nReason      = XMS_E_NONE;
    xmsJMSEXP_TYPE jmsexception = XMS_X_NO_EXCEPTION;

    int            rc;

    if (hError != NULL) {
        xmsHErrorBlock xmsLinkedError = (xmsHErrorBlock) XMS_NULL_HANDLE;

        xmsErrorGetJMSException(hError, &jmsexception);
        xmsErrorGetErrorCode(hError, &nReason);
        rc = nReason;
        xmsErrorGetErrorString(hError, szText, cbTextSize, &cbActualSize);
        xmsErrorGetErrorData(hError, szData, cbDataSize, &cbActualSize);

        if (SAM_DEBUG) {
            zend_printf("<br/>Error Block:<br/>");
            zend_printf("  -> JMSException = %d (%s)<br/>", jmsexception, getExceptionTypeString(jmsexception));
            zend_printf("  -> Error Code   = %d (%s)<br/>", nReason, szText);
            zend_printf("  -> Error Data   = %s<br/>", szData);
        }

        /*
        * Get the next linked error, and act recursively
        */

        xmsErrorGetLinkedError(hError, &xmsLinkedError);

        if (XMS_NULL_HANDLE != xmsLinkedError) rc = check(xmsLinkedError);
    }


    return rc;
}

/* {{{ proto PHP_NAMED_FUNCTION(SAM_Connection_ctor)
      Main constructor function for a SAMXMSConnection */

PHP_NAMED_FUNCTION(SAM_XMS_Connection_ctor) {
    if (SAM_DEBUG) zend_printf("<br/><b>SAMXMSConnection->ctor()</b> enter<br/>");

    if (SAM_DEBUG) zend_printf("<b>SAMXMSConnection->ctor()</b> exit<br/>");
    return;
}
/* }}} */

/* {{{ proto PHP_NAMED_FUNCTION(SAM_Connection_commit)
      Commit any outstanding unit of work on the connection */
PHP_FUNCTION(SAM_XMS_Connection_commit) {

    php_sam_xms_connection      *connResource;
    zval                    **zConnection;
    xmsHErrorBlock          errorBlock;

    if (SAM_DEBUG) zend_printf("<br/><b>SAMXMSConnection->commit()</b> enter<br/>");

    /* Retrieve the connection resource from this object's hash table */
    if (zend_hash_find(Z_OBJPROP_P(this_ptr), SAM_CONNECTION_CONNECTION_INDEX, sizeof(SAM_CONNECTION_CONNECTION_INDEX), (void**)&zConnection) == FAILURE) {
        setError(this_ptr TSRMLS_CC, SAM_CONNECTION_NO_CONNECTION, SAM_CONNECTION_NO_CONNECTION_MSG);
        return;
    }
    ZEND_FETCH_RESOURCE(connResource, php_sam_xms_connection*, zConnection, -1, PHP_SAM_CONNECTION_RES_NAME, le_sam_xms_connection);
    if (SAM_DEBUG) zend_printf("<b>SAMXMSConnection->commit()</b> connection is %d<br/>", connResource->conn);

    /* Check we have a valid XMS session handle */
    if (XMS_NULL_HANDLE == connResource->session) {
        setError(this_ptr TSRMLS_CC, SAM_CONNECTION_NULL_SESSION, SAM_CONNECTION_NULL_SESSION_MSG);
        RETURN_FALSE;
    }

    /* Create an XMS error block that we will need to issue a commit */
    if (XMS_OK != xmsErrorCreate(&errorBlock)) {
       setError(this_ptr TSRMLS_CC, SAM_CONNECTION_NO_ERROR_BLOCK, SAM_CONNECTION_NO_ERROR_BLOCK_MSG);
       RETURN_FALSE;
    }

    /* Issue the XMS commit */
    if (XMS_OK != xmsSessCommit(connResource->session, errorBlock)) {
        setError(this_ptr TSRMLS_CC, SAM_CONNECTION_COMMIT_FAILED, SAM_CONNECTION_COMMIT_FAILED_MSG);
        if (SAM_DEBUG) check(errorBlock);
        RETURN_FALSE;
    }

    /* Finished with the error block */
    xmsErrorDispose(&errorBlock);

    if (SAM_DEBUG) zend_printf("<b>SAMXMSConnection->commit()</b> exit<br/>");
    RETURN_TRUE;
}
/* }}} */

/* {{{ proto PHP_NAMED_FUNCTION(SAM_Connection_connect)
     Connnect to a messaging server.  */
PHP_FUNCTION(SAM_XMS_Connection_connect) {

    int rc;

    char *protocolString = NULL;
    int protocolStringLength = 0;
    zval *zvOptions = NULL;

    xmsHErrorBlock errorBlock;
    xmsHConnFact connFactory;
    xmsHConn connection;

    HashTable *optionsHT;
    zval **zTemp;

    /* Used to parse the protocol string */
    int i, j;
    char szProtocol[128]         = { '\0' };
    char szProtocolOption[256]   = { '\0' };

    char szHostName[512]         = { '\0' };
    char szPort[128]             = { '\0' };
    char szBrokerName[512]       = { '\0' };
    char szEndpoints[512]        = { '\0' };
    char szTargetChain[512]      = { '\0' };
    char szBusName[128]          = { '\0' };

    char *szWpmDurSubHome = NULL;
    char *szTransactions = NULL;
    char *szUserId = NULL;
    char *szPassword =NULL;

    php_sam_xms_connection           *connResource;
    zval*                        zConnection;

    int                          transacted = xmsFALSE;

    if (SAM_DEBUG) zend_printf("<br/><b>SAMXMSConnection->connect()</b> enter<br/>");

    resetError(this_ptr TSRMLS_CC);

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC,
        "s|a", &protocolString, &protocolStringLength, &zvOptions) == FAILURE) {
            setError(this_ptr TSRMLS_CC, SAM_CONNECTION_WRONG_PARAMETERS_CONNECT, SAM_CONNECTION_WRONG_PARAMETERS_CONNECT_MSG);
            WRONG_PARAM_COUNT;
            RETURN_FALSE;
        }

        /* Get the protocol (found before the first ':') */
        for (i = 0, j = 0; j >= 0 && i < protocolStringLength && j < sizeof(szProtocol); i++, j++) {
            switch (protocolString[i]) {
        case '\0':
        case SAM_CONNECTION_PROTOCOL_SEPERATOR:
            szProtocol[j] = '\0';
            j = -2;
            break;

        default:
            szProtocol[j] = protocolString[i];
            break;
            }
        }

        /* Now find any protocol option */
        for (j = 0 ; j >= 0 && i < protocolStringLength && j < sizeof(szProtocolOption); i++, j++) {
            switch (protocolString[i]) {
        case '\0':
            szProtocolOption[j] = '\0';
            j = -2;
            break;

        default:
            szProtocolOption[j] = protocolString[i];
            break;
            }
        }

        /* are the protocol and any options valid?    */
        if (strcmp(szProtocol, SAM_CONSTANT_WMQ) == 0) {
            if (strlen(szProtocolOption) == 0) {
                /* Default the protocol option to client    */
                strcpy(szProtocolOption, SAM_CONSTANT_OPTION_CLIENT);
                if (SAM_DEBUG) zend_printf("<b>SAMXMSConnection->connect()</b> Set default protocol option (%s)<br/>", szProtocolOption);
            }
            if (strcmp(szProtocolOption, SAM_CONSTANT_OPTION_CLIENT) != 0
                && strcmp(szProtocolOption, SAM_CONSTANT_OPTION_BINDINGS) != 0) {
                    if (SAM_DEBUG) zend_printf("<br/><b>SAMXMSConnection->connect()</b> Unsupported protocol option (%s) requested!<br/>", szProtocolOption);
                    setError(this_ptr TSRMLS_CC, SAM_CONNECTION_UNSUPPORTED_PROTOCOL_OPTION, SAM_CONNECTION_UNSUPPORTED_PROTOCOL_OPTION_MSG);
                    RETURN_FALSE;
                }
        } else if (strcmp(szProtocol, SAM_CONSTANT_RTT) == 0) {
        } else if (strcmp(szProtocol, SAM_CONSTANT_WPM) == 0) {
        } else {
            if (SAM_DEBUG) zend_printf("<br/><b>SAMXMSConnection->connect()</b> Unsupported protocol (%s) requested!<br/>", szProtocol);
            setError(this_ptr TSRMLS_CC, SAM_CONNECTION_UNSUPPORTED_PROTOCOL, SAM_CONNECTION_UNSUPPORTED_PROTOCOL_MSG);
            RETURN_FALSE;
        }

        /* Take a look in the options array if we have one...    */
        if (zvOptions) {
            optionsHT = Z_ARRVAL_P(zvOptions);
            if (zend_hash_num_elements(optionsHT) > 0) {
                optionsHT = HASH_OF(zvOptions);

                /* Has a host name been specified?      */
                if (zend_hash_find(optionsHT, SAM_CONSTANT_HOST, sizeof(SAM_CONSTANT_HOST), (void**)&zTemp) == SUCCESS) {
                    convert_to_string(*zTemp);
                    strncpy(szHostName, Z_STRVAL_PP(zTemp), Z_STRLEN_PP(zTemp));
                    if (SAM_DEBUG) zend_printf("<b>SAMXMSConnection->connect()</b> Caller specified hostname of <b>%s</b><br/>", szHostName);
                } else {
                    /* Looks like no hostname specified so default to localhost...    */
                    strcpy(szHostName, "localhost");
                }

                /* Has a port been specified?      */
                if (zend_hash_find(optionsHT, SAM_CONSTANT_PORT, sizeof(SAM_CONSTANT_PORT), (void**)&zTemp) == SUCCESS) {
                    convert_to_string(*zTemp);
                    strncpy(szPort, Z_STRVAL_PP(zTemp), Z_STRLEN_PP(zTemp));
                    if (SAM_DEBUG) zend_printf("<b>SAMXMSConnection->connect()</b> Caller specified port of <b>%s</b><br/>", szPort);
                } else {
                    /* Looks like no port number specified so default dependant on protocol...    */
                    if (strcmp(szProtocol, SAM_CONSTANT_WMQ) == 0) {
                        strcpy(szHostName, "1414");
                    } else if (strcmp(szProtocol, SAM_CONSTANT_RTT) == 0) {
                        strcpy(szHostName, "1506");
                    }
                }

                /* Has a broker name been specified?      */
                if (zend_hash_find(optionsHT, SAM_CONSTANT_BROKER, sizeof(SAM_CONSTANT_BROKER), (void**)&zTemp) == SUCCESS) {
                    convert_to_string(*zTemp);
                    strncpy(szBrokerName, Z_STRVAL_PP(zTemp), Z_STRLEN_PP(zTemp));
                    if (SAM_DEBUG) zend_printf("<b>SAMXMSConnection->connect()</b> Caller specified broker name of <b>%s</b><br/>", szBrokerName);
                }

                /* Have one or more endpoints been specified?      */
                if (zend_hash_find(optionsHT, SAM_CONSTANT_ENDPOINTS, sizeof(SAM_CONSTANT_ENDPOINTS), (void**)&zTemp) == SUCCESS) {
                    convert_to_string(*zTemp);
                    strncpy(szEndpoints, Z_STRVAL_PP(zTemp), Z_STRLEN_PP(zTemp));
                    if (SAM_DEBUG) zend_printf("<b>SAMXMSConnection->connect()</b> Caller specified endpoints of <b>%s</b><br/>", szEndpoints);
                }

                /* Has a bus name been specified?      */
                if (zend_hash_find(optionsHT, SAM_CONSTANT_BUS, sizeof(SAM_CONSTANT_BUS), (void**)&zTemp) == SUCCESS) {
                    convert_to_string(*zTemp);
                    strncpy(szBusName, Z_STRVAL_PP(zTemp), Z_STRLEN_PP(zTemp));
                    if (SAM_DEBUG) zend_printf("<b>SAMXMSConnection->connect()</b> Caller specified bus name of <b>%s</b><br/>", szBusName);
                }

                /* Has a WPM durable subscription home been specified?      */
                if (zend_hash_find(optionsHT, SAM_CONSTANT_WPM_DUR_SUB_HOME, sizeof(SAM_CONSTANT_WPM_DUR_SUB_HOME), (void**)&zTemp) == SUCCESS) {
                    convert_to_string(*zTemp);

                    if (NULL == (szWpmDurSubHome = emalloc(Z_STRLEN_PP(zTemp)+1))) {
                        setError(this_ptr TSRMLS_CC, SAM_CONNECTION_WPMDURSUBHOME_ALLOC_FAILED, SAM_CONNECTION_WPMDURSUBHOME_ALLOC_FAILED_MSG);
                        RETURN_FALSE;
                    }

                    strncpy(szWpmDurSubHome, Z_STRVAL_PP(zTemp), Z_STRLEN_PP(zTemp));
                    szWpmDurSubHome[Z_STRLEN_PP(zTemp)]=0;
                    if (SAM_DEBUG) zend_printf("<b>SAMXMSConnection->connect()</b> Caller specified WPM durable subscription home of <b>%s</b><br/>", szWpmDurSubHome);
                }

                /* Has a target chain been specified? */
                if (zend_hash_find(optionsHT, SAM_CONSTANT_TARGETCHAIN, sizeof(SAM_CONSTANT_TARGETCHAIN), (void**)&zTemp) == SUCCESS) {
                    convert_to_string(*zTemp);
                    strncpy(szTargetChain, Z_STRVAL_PP(zTemp), Z_STRLEN_PP(zTemp));
                    if (SAM_DEBUG) zend_printf("<b>SAMXMSConnection->connect()</b> Caller specified target chain of <b>%s</b><br/>", szTargetChain);
                }

                /* Has a transaction option been specified? */
                if (zend_hash_find(optionsHT, SAM_CONSTANT_TRANSACTIONS, sizeof(SAM_CONSTANT_TRANSACTIONS), (void**)&zTemp) == SUCCESS) {
                    convert_to_string(*zTemp);

                   if (NULL == (szTransactions = emalloc(Z_STRLEN_PP(zTemp)+1))) {
                        setError(this_ptr TSRMLS_CC, SAM_CONNECTION_TRANSACTIONS_ALLOC_FAILED, SAM_CONNECTION_TRANSACTIONS_ALLOC_FAILED_MSG);
                        RETURN_FALSE;
                    }

                    strncpy(szTransactions, Z_STRVAL_PP(zTemp), Z_STRLEN_PP(zTemp));
                    szTransactions[Z_STRLEN_PP(zTemp)]=0;
                    if (SAM_DEBUG) zend_printf("<b>SAMXMSConnection->connect()</b> Caller specified transactions options <b>%s</b><br/>", szTransactions);

                    i = strcmp(SAM_CONSTANT_MANUAL, szTransactions);
                    efree(szTransactions);
                    if (0 == i) {
                        if (SAM_DEBUG) zend_printf("<b>SAMXMSConnection->connect()</b> Caller specified manual transaction control<br/>");
                        transacted = xmsTRUE;
                    } else if (0 != strcmp(SAM_CONSTANT_AUTO, szTransactions)) {
                        setError(this_ptr TSRMLS_CC, SAM_CONNECTION_ILLEGAL_TRANSACTION_MODE, SAM_CONNECTION_ILLEGAL_TRANSACTION_MODE_MSG);
                        RETURN_FALSE;
                        }

                }

                /* Has a user identifier been specified? */
                if (zend_hash_find(optionsHT, SAM_CONSTANT_USERID, sizeof(SAM_CONSTANT_USERID), (void**)&zTemp) == SUCCESS) {
                    convert_to_string(*zTemp);

                    if (NULL == (szUserId = emalloc(Z_STRLEN_PP(zTemp)+1))) {
                        setError(this_ptr TSRMLS_CC, SAM_CONNECTION_USERID_ALLOC_FAILED, SAM_CONNECTION_USERID_ALLOC_FAILED_MSG);
                        RETURN_FALSE;
                    }

                    strncpy(szUserId, Z_STRVAL_PP(zTemp), Z_STRLEN_PP(zTemp));
                    szUserId[Z_STRLEN_PP(zTemp)]=0;
                    if (SAM_DEBUG) zend_printf("<b>SAMXMSConnection->connect()</b> Caller specified UserId of <b>%s</b><br/>", szUserId);
                }

                /* Has a password been specified? */
                if (zend_hash_find(optionsHT, SAM_CONSTANT_PASSWORD, sizeof(SAM_CONSTANT_PASSWORD), (void**)&zTemp) == SUCCESS) {
                    convert_to_string(*zTemp);

                    if (NULL == (szPassword = emalloc(Z_STRLEN_PP(zTemp)+1))) {
                        setError(this_ptr TSRMLS_CC, SAM_CONNECTION_PASSWORD_ALLOC_FAILED, SAM_CONNECTION_PASSWORD_ALLOC_FAILED_MSG);
                        RETURN_FALSE;
                    }

                    strncpy(szPassword, Z_STRVAL_PP(zTemp), Z_STRLEN_PP(zTemp));
                    szPassword[Z_STRLEN_PP(zTemp)]=0;
                    if (SAM_DEBUG) zend_printf("<b>SAMXMSConnection->connect()</b> Caller specified password of <b>%s</b><br/>", szPassword);
                }

            } /* zend_hash_num_elements(optionsHT) > 0 */
        } else {
            if (SAM_DEBUG) zend_printf("<b>SAMXMSConnection->connect()</b> No options array specified.<br/>");
        }  /* zvOptions */

        /*  Create an error block for capturing any error information from XMS...      */
        if (XMS_OK != xmsErrorCreate(&errorBlock)) {
            if (SAM_DEBUG) zend_printf("<br/><b>SAMXMSConnection->connect()</b> xmsErrorCreate failed!<br/>");
            setError(this_ptr TSRMLS_CC, SAM_CONNECTION_NO_ERROR_BLOCK, SAM_CONNECTION_NO_ERROR_BLOCK_MSG);
            RETURN_FALSE;
        }

        /* Create a connection factory from which we can conjure up a connection to the messaging server...    */
        if (XMS_OK != xmsConnFactCreate(&connFactory, errorBlock)) {
            if (SAM_DEBUG) zend_printf("<br/><b>SAMXMSConnection->connect()</b> xmsConnFactCreate failed!<br/>");
            setError(this_ptr TSRMLS_CC, SAM_CONNECTION_NO_CONNECTION_FACTORY, SAM_CONNECTION_NO_CONNECTION_FACTORY_MSG);
            RETURN_FALSE;
        }

        /* Set up the correct protocol property on the connection factory...     */
        if (strcmp(szProtocol, SAM_CONSTANT_WMQ) == 0) {
            if (SAM_DEBUG) zend_printf("<b>SAMXMSConnection->connect()</b> protocol WMQ<br/>");
            rc = xmsSetIntProperty( (xmsHObj) connFactory, XMSC_CONNECTION_TYPE, XMSC_CT_WMQ, errorBlock);
        } else if (strcmp(szProtocol, SAM_CONSTANT_RTT) == 0) {
            if (SAM_DEBUG) zend_printf("<b>SAMXMSConnection->connect()</b> protocol RTT<br/>");
            rc = xmsSetIntProperty( (xmsHObj) connFactory, XMSC_CONNECTION_TYPE, XMSC_CT_RTT, errorBlock);
        } else if (strcmp(szProtocol, SAM_CONSTANT_WPM) == 0) {
            if (SAM_DEBUG) zend_printf("<b>SAMXMSConnection->connect()</b> protocol WPM<br/>");
            rc = xmsSetIntProperty( (xmsHObj) connFactory, XMSC_CONNECTION_TYPE, XMSC_CT_WPM, errorBlock);
        }
        if (XMS_OK != rc) {
            if (SAM_DEBUG) zend_printf("<br/><b>SAMXMSConnection->connect()</b> xmsSetIntProperty (XMSC_CONNECTION_TYPE) failed!<br/>");
            setError(this_ptr TSRMLS_CC, SAM_CONNECTION_SET_CONNECTION_TYPE, SAM_CONNECTION_SET_CONNECTION_TYPE_MSG);
            RETURN_FALSE;
        }

        /* Set up any protocol independent properties */
        if (NULL != szUserId) {
            if (SAM_DEBUG) zend_printf("<b>SAMXMSConnection->connect()</b> userid %s<br/>", szUserId);

            /* Set the XMSC_USERID connection factory property */
            rc = xmsSetStringProperty( (xmsHObj) connFactory, XMSC_USERID, szUserId, strlen(szUserId), errorBlock);

            /* We can now free szUserId */
            efree(szUserId);

            if (XMS_OK != rc) {
               if (SAM_DEBUG) zend_printf("<br/><b>SAMXMSConnection->connect()</b> xmsSetStringProperty (XMSC_USERID) failed!<br/>");
               setError(this_ptr TSRMLS_CC, SAM_CONNECTION_SET_USERID, SAM_CONNECTION_SET_USERID_MSG);
               RETURN_FALSE;
            }

        }

        if (NULL != szPassword) {
            if (SAM_DEBUG) zend_printf("<b>SAMXMSConnection->connect()</b> password %s<br/>", szPassword);

            /* Set the XMSC_PASSWORD connection factory property */
            rc = xmsSetStringProperty( (xmsHObj) connFactory, XMSC_PASSWORD, szPassword, strlen(szPassword), errorBlock);

            /* We can now free szPassword */
            efree(szPassword);

            if (XMS_OK != rc) {
                if (SAM_DEBUG) zend_printf("<br/><b>SAMXMSConnection->connect()</b> xmsSetStringProperty (XMSC_PASSWORD) failed!<br/>");
                setError(this_ptr TSRMLS_CC, SAM_CONNECTION_SET_PASSWORD, SAM_CONNECTION_SET_PASSWORD_MSG);
                RETURN_FALSE;
            }
        }

        /* set up any other properties we need dependant on the protocol we are using...   */
        if (strcmp(szProtocol, SAM_CONSTANT_WMQ) == 0) {
            /*
            WMQ    WebSphere MQ
            ---
            */

            /* connection mode   */
            if (strcmp(szProtocolOption, SAM_CONSTANT_OPTION_BINDINGS) == 0) {
                if (SAM_DEBUG) zend_printf("<b>SAMXMSConnection->connect()</b> protocol option BINDINGS<br/>");
                rc = xmsSetIntProperty( (xmsHObj) connFactory, XMSC_WMQ_CONNECTION_MODE, XMSC_WMQ_CM_BINDINGS, errorBlock);
            } else {
                if (SAM_DEBUG) zend_printf("<b>SAMXMSConnection->connect()</b> protocol option CLIENT<br/>");
                rc = xmsSetIntProperty( (xmsHObj) connFactory, XMSC_WMQ_CONNECTION_MODE, XMSC_WMQ_CM_CLIENT, errorBlock);
            }
            if (XMS_OK != rc) {
                if (SAM_DEBUG) zend_printf("<br/><b>SAMXMSConnection->connect()</b> xmsSetIntProperty (XMSC_CONNECTION_MODE) failed for '%s'!<br/>", szProtocolOption);
                setError(this_ptr TSRMLS_CC, SAM_CONNECTION_SET_CONNECTION_MODE, SAM_CONNECTION_SET_CONNECTION_MODE_MSG);
                RETURN_FALSE;
            }

            /* hostname   */
            if (SAM_DEBUG) zend_printf("<b>SAMXMSConnection->connect()</b> hostname %s<br/>", szHostName);
            if (XMS_OK != xmsSetStringProperty((xmsHObj) connFactory, XMSC_WMQ_HOST_NAME, szHostName, -1, errorBlock)) {
                if (SAM_DEBUG) zend_printf("<br/><b>SAMXMSConnection->connect()</b> xmsSetStringProperty (XMSC_WMQ_HOST_NAME) failed!<br/>");
                setError(this_ptr TSRMLS_CC, SAM_CONNECTION_SET_HOST_NAME, SAM_CONNECTION_SET_HOST_NAME_MSG);
                RETURN_FALSE;
            }

            /* port   */
            if (SAM_DEBUG) zend_printf("<b>SAMXMSConnection->connect()</b> port %s<br/>", szPort);
            if (XMS_OK != xmsSetStringProperty((xmsHObj) connFactory, XMSC_WMQ_PORT, szPort, -1, errorBlock)) {
                if (SAM_DEBUG) zend_printf("<br/><b>SAMXMSConnection->connect()</b> xmsSetStringProperty (XMSC_WMQ_PORT) failed!<br/>");
                setError(this_ptr TSRMLS_CC, SAM_CONNECTION_SET_PORT, SAM_CONNECTION_SET_PORT_MSG);
                RETURN_FALSE;
            }

            /* broker name  */
            if (SAM_DEBUG) zend_printf("<b>SAMXMSConnection->connect()</b> queue manager %s<br/>", szBrokerName);
            if (XMS_OK !=  xmsSetStringProperty((xmsHObj) connFactory, XMSC_WMQ_QUEUE_MANAGER, szBrokerName, -1, errorBlock)) {
                if (SAM_DEBUG) zend_printf("<br/><b>SAMXMSConnection->connect()</b> xmsSetStringProperty (XMSC_WMQ_QUEUE_MANAGER) failed!<br/>");
                setError(this_ptr TSRMLS_CC, SAM_CONNECTION_SET_QUEUE_MANAGER_NAME, SAM_CONNECTION_SET_QUEUE_MANAGER_NAME_MSG);
                RETURN_FALSE;
            }

           /* Durable subscription home messaging engine is WPM only */
           if (NULL != szWpmDurSubHome) {
               /* We are not using WPM but the SAM_WPM_DUR_SUB_HOME option has been set, this is an error */
               setError(this_ptr TSRMLS_CC, SAM_CONNECTION_ILLEGAL_WPMDURSUBHOME_MODE, SAM_CONNECTION_ILLEGAL_WPMDURSUBHOME_MODE_MSG);
               efree(szWpmDurSubHome);
               RETURN_FALSE;
            }

        } else if (strcmp(szProtocol, SAM_CONSTANT_WPM) == 0) {
            /*
            WPM    WebSphere Platform Messaging
            ---
            */

            /* possible endpoints  */
            if (SAM_DEBUG) zend_printf("<b>SAMXMSConnection->connect()</b> endpoints %s<br/>", szEndpoints);
            if (XMS_OK != xmsSetStringProperty((xmsHObj) connFactory, XMSC_WPM_PROVIDER_ENDPOINTS, szEndpoints, -1, errorBlock)) {
                if (SAM_DEBUG) zend_printf("<br/><b>SAMXMSConnection->connect()</b> xmsSetStringProperty (XMSC_WPM_PROVIDER_ENDPOINTS) failed!<br/>");
                setError(this_ptr TSRMLS_CC, SAM_CONNECTION_SET_ENDPOINTS, SAM_CONNECTION_SET_ENDPOINTS_MSG);
                RETURN_FALSE;
            }

            /* target chain  */
            if (SAM_DEBUG) zend_printf("<b>SAMXMSConnection->connect()</b> target chain %s<br/>", szTargetChain);
            if (XMS_OK != xmsSetStringProperty((xmsHObj) connFactory, XMSC_WPM_TARGET_TRANSPORT_CHAIN, szTargetChain, -1, errorBlock)) {
                if (SAM_DEBUG) zend_printf("<br/><b>SAMXMSConnection->connect()</b> xmsSetStringProperty (XMSC_WPM_TARGET_TRANSPORT_CHAIN) failed!<br/>");
                setError(this_ptr TSRMLS_CC, SAM_CONNECTION_SET_ENDPOINTS, SAM_CONNECTION_SET_ENDPOINTS_MSG);
                RETURN_FALSE;
            }

            /* Bus name  */
            if (SAM_DEBUG) zend_printf("<b>SAMXMSConnection->connect()</b> bus %s<br/>", szBusName);
            if (XMS_OK != xmsSetStringProperty((xmsHObj) connFactory, XMSC_WPM_BUS_NAME, szBusName, -1, errorBlock)) {
                if (SAM_DEBUG) zend_printf("<br/><b>SAMXMSConnection->connect()</b> xmsSetStringProperty (XMSC_WPM_BUS_NAME) failed!<br/>");
                setError(this_ptr TSRMLS_CC, SAM_CONNECTION_SET_BUS, SAM_CONNECTION_SET_BUS_MSG);
                RETURN_FALSE;
            }

            /* WPM_DUR_SUB_HOME - if this option has been specified, set the connection factory property
               NB: This must be provided for a pub/sub type connection but is not required for point to point */
            if (NULL != szWpmDurSubHome) {

                /* Set the XMSC_WPM_DUR_SUB_HOME connection factory property */
               rc = xmsSetStringProperty( (xmsHObj) connFactory, XMSC_WPM_DUR_SUB_HOME, szWpmDurSubHome, strlen(szWpmDurSubHome), errorBlock);

               /* We can now free szWpmDurSubHome */
               efree(szWpmDurSubHome);

               if (XMS_OK != rc) {
                   if (SAM_DEBUG) zend_printf("<br/><b>SAMXMSConnection->connect()</b> xmsSetStringProperty (XMSC_WPM_DUR_SUB_HOME) failed!<br/>");
                   setError(this_ptr TSRMLS_CC, SAM_CONNECTION_SET_WPMDURSUBHOME, SAM_CONNECTION_SET_WPMDURSUBHOME_MSG);
                   RETURN_FALSE;
               }
            }

        } else if (strcmp(szProtocol, SAM_CONSTANT_RTT) == 0) {
            /*
            RTT
            ---
            */

            /* connection protocol   */
            if (XMS_OK != xmsSetIntProperty((xmsHObj) connFactory, XMSC_RTT_CONNECTION_PROTOCOL, XMSC_RTT_CP_TCP, errorBlock)) {
                if (SAM_DEBUG) zend_printf("<br/><b>SAMXMSConnection->connect()</b> xmsSetIntProperty (XMSC_RTT_CONNECTION_PROTOCOL) failed for '%s'!<br/>", szProtocolOption);
                setError(this_ptr TSRMLS_CC, SAM_CONNECTION_SET_CONNECTION_PROTOCOL, SAM_CONNECTION_SET_CONNECTION_PROTOCOL_MSG);
                return;
            }

            /* hostname    */
            if (SAM_DEBUG) zend_printf("<b>SAMXMSConnection->connect()</b> hostname %s<br/>", szHostName);
            if (XMS_OK != xmsSetStringProperty((xmsHObj) connFactory, XMSC_RTT_HOST_NAME, szHostName, -1, errorBlock)) {
                if (SAM_DEBUG) zend_printf("<br/><b>SAMXMSConnection->connect()</b> xmsSetStringProperty (XMSC_RTT_HOST_NAME) failed!<br/>");
                setError(this_ptr TSRMLS_CC, SAM_CONNECTION_SET_HOST_NAME, SAM_CONNECTION_SET_HOST_NAME_MSG);
                RETURN_FALSE;
            }

            /* port   */
            if (SAM_DEBUG) zend_printf("<b>SAMXMSConnection->connect()</b> port %s<br/>", szPort);
            if (XMS_OK != xmsSetStringProperty((xmsHObj) connFactory, XMSC_RTT_PORT, szPort, -1, errorBlock)) {
                if (SAM_DEBUG) zend_printf("<br/><b>SAMXMSConnection->connect()</b> xmsSetStringProperty (XMSC_RTT_PORT) failed!<br/>");
                setError(this_ptr TSRMLS_CC, SAM_CONNECTION_SET_PORT, SAM_CONNECTION_SET_PORT_MSG);
                RETURN_FALSE;
            }
 
            /* Durable subscription home messaging engine is WPM only */
            if (NULL != szWpmDurSubHome) {
               /* We are not using WPM but the SAM_WPM_DUR_SUB_HOME option has been set, this is an error */
               setError(this_ptr TSRMLS_CC, SAM_CONNECTION_ILLEGAL_WPMDURSUBHOME_MODE, SAM_CONNECTION_ILLEGAL_WPMDURSUBHOME_MODE_MSG);
               efree(szWpmDurSubHome);
               RETURN_FALSE;
            }
        }

        /* create the XMS connection */
        rc = xmsConnFactCreateConnection(connFactory, &connection, errorBlock);
        if (XMS_OK != rc) {
            if (SAM_DEBUG) zend_printf("<br/><b>SAMXMSConnection->connect()</b> Connection create failed!\n");
            setError(this_ptr TSRMLS_CC, SAM_CONNECTION_CONNECTION_CREATE, SAM_CONNECTION_CONNECTION_CREATE_MSG);
            if (SAM_DEBUG) check(errorBlock);
            RETURN_FALSE;
        }

        /* start the connection (we can't receive anything without starting it!)  */
        rc = xmsConnStart(connection, errorBlock);
        if (XMS_OK != rc) {
            if (SAM_DEBUG) zend_printf("<br/><b>SAMXMSConnection->connect()</b> Connection start failed!\n");
            setError(this_ptr TSRMLS_CC, SAM_CONNECTION_CONNECTION_START, SAM_CONNECTION_CONNECTION_START_MSG);
            if (SAM_DEBUG) check(errorBlock);
            RETURN_FALSE;
        }

        if (SAM_DEBUG) zend_printf("<b>SAMXMSConnection->connect()</b>  xmsConnFactCreateConnection rc=%d connection=%ld<br/>", rc, (long)connection);

        /* Create a resource to represent this XMS connection   */
        MAKE_STD_ZVAL(zConnection);
        connResource = emalloc(sizeof(php_sam_xms_connection));
        connResource->conn = connection;
        connResource->session = XMS_NULL_HANDLE;
        connResource->transacted = transacted;

        /* Create the XMS session. The session handle is stored in the connection resource so it can be used without
           creating and closing further sessions in each api call. */
        if (XMS_OK != xmsConnCreateSession(connResource->conn, transacted, XMSC_AUTO_ACKNOWLEDGE, &(connResource->session), errorBlock)) {
            setError(this_ptr TSRMLS_CC, SAM_CONNECTION_NO_SESSION, SAM_CONNECTION_NO_SESSION_MSG);
            if (SAM_DEBUG) check(errorBlock);
            RETURN_FALSE;
        }

        /* copy the protocol type into the connection resource (so we can validate the target client option in the send) */
        strcpy(connResource->protocol, szProtocol);

        ZEND_REGISTER_RESOURCE(zConnection, connResource, le_sam_xms_connection);

        /* Store the connection property in this object's hash table */
        add_property_zval(this_ptr, SAM_CONNECTION_CONNECTION_INDEX, zConnection);

        if (SAM_DEBUG) zend_printf("<b>SAMXMSConnection->connect()</b> new connection %d<br/>", connResource->conn);

        /* clean up any XMS resources we no longer need...   */
        /* We have finished with the connection factory */
        if (SAM_DEBUG) zend_printf("<b>SAMXMSConnection->connect()</b> Deleting connection factory<br/>");
        rc = xmsConnFactDispose(&connFactory, errorBlock);
        if (XMS_OK != rc) {
            if (SAM_DEBUG) zend_printf("<br/><b>SAMXMSConnection->connect()</b> Delete of connection factory failed!\n");
            setError(this_ptr TSRMLS_CC, SAM_CONNECTION_CONNFACT_DELETE, SAM_CONNECTION_CONNFACT_DELETE_MSG);
            if (SAM_DEBUG) check(errorBlock);
            RETURN_FALSE;
        }

        /* And we have finished with the error block */
        xmsErrorDispose(&errorBlock);

        if (SAM_DEBUG) zend_printf("<b>SAMXMSConnection->connect()</b> exit<br/>");
        RETURN_TRUE;
}
/* }}} */


/* {{{ proto PHP_NAMED_FUNCTION(SAM_Connection_disconnect)
     disconnnect from a messaging server.  */
PHP_FUNCTION(SAM_XMS_Connection_disconnect) {

    php_sam_xms_connection  *connResource;
    zval                   **zConnection;
    xmsHErrorBlock          errorBlock;
    int                     rc;

    if (SAM_DEBUG) zend_printf("<br/><b>SAMXMSConnection->disconnect()</b> enter<br/>");

    /* Retrieve the connection resource from this object's hash table */
    if (zend_hash_find(Z_OBJPROP_P(this_ptr), SAM_CONNECTION_CONNECTION_INDEX, sizeof(SAM_CONNECTION_CONNECTION_INDEX), (void**)&zConnection) == FAILURE) {
        setError(this_ptr TSRMLS_CC, SAM_CONNECTION_NO_CONNECTION, SAM_CONNECTION_NO_CONNECTION_MSG);
        return;
    }
    ZEND_FETCH_RESOURCE(connResource, php_sam_xms_connection*, zConnection, -1, PHP_SAM_CONNECTION_RES_NAME, le_sam_xms_connection);

    if (connResource) {

        /* Create an error block */
        if (XMS_OK != xmsErrorCreate(&errorBlock)) {
            setError(this_ptr TSRMLS_CC, SAM_CONNECTION_NO_ERROR_BLOCK, SAM_CONNECTION_NO_ERROR_BLOCK_MSG);
            RETURN_FALSE;
        }

        /* Close the session */
        if (XMS_NULL_HANDLE != connResource->session) {

            /* Issue an explicit rollback before closing the session if the session was manually transacted */
            if (connResource->transacted) {
                if (SAM_DEBUG) zend_printf("<b>SAMXMSConnection->disconnect()</b> Issuing rollback<br/>");
                if (XMS_OK != xmsSessRollback(connResource->session, errorBlock)) {
                    if (SAM_DEBUG) zend_printf("<br/><b>SAMXMSConnection->disconnect()</b> Session rollback failed!\n");
                    setError(this_ptr TSRMLS_CC, SAM_CONNECTION_ROLLBACK_FAILED, SAM_CONNECTION_ROLLBACK_FAILED_MSG);
                    RETURN_FALSE;
                }
            }

            if (SAM_DEBUG) zend_printf("<br/><b>SAMXMSConnection->disconnect()</b> Closing session\n");
            if (XMS_OK != xmsSessClose(&connResource->session, errorBlock)) {
                if (SAM_DEBUG) zend_printf("<br/><b>SAMXMSConnection->disconnect()</b> Session close failed!\n");
                setError(this_ptr TSRMLS_CC, SAM_CONNECTION_SESSION_CLOSE, SAM_CONNECTION_SESSION_CLOSE_MSG);
                if (SAM_DEBUG) check(errorBlock);
                RETURN_FALSE;
            }
        }


        /* stop the connection */
        if (connResource->conn) {

            if (SAM_DEBUG) zend_printf("<br/><b>SAMXMSConnection->disconnect()</b> Stopping connection\n");
                rc = xmsConnStop(connResource->conn, errorBlock);
                if (XMS_OK != rc) {
                    if (SAM_DEBUG) zend_printf("<br/><b>SAMXMSConnection->disconnect()</b> Connection stop failed!\n");
                    setError(this_ptr TSRMLS_CC, SAM_CONNECTION_CONNECTION_STOP, SAM_CONNECTION_CONNECTION_STOP_MSG);
                    if (SAM_DEBUG) check(errorBlock);
                    RETURN_FALSE;
                }

            /* close the connection */
            if (SAM_DEBUG) zend_printf("<b>SAMXMSConnection->disconnect()</b> closing connection %d<br/>", connResource->conn);
            rc = xmsConnClose(&connResource->conn, errorBlock);
            if (XMS_OK != rc) {
                if (SAM_DEBUG) zend_printf("<br/><b>SAMXMSConnection->disconnect()</b> Connection close failed!\n");
                setError(this_ptr TSRMLS_CC, SAM_CONNECTION_CONNECTION_CLOSE, SAM_CONNECTION_CONNECTION_CLOSE_MSG);
                if (SAM_DEBUG) check(errorBlock);
                RETURN_FALSE;

            }

            /* Set the connection to NULL to eliminate the possibility of attempting to disconnect again */
            connResource->conn=NULL;
        }

        /* Remove the hashtable entry that points to the resource we have now finished with */
        zend_hash_del(Z_OBJPROP_P(this_ptr), SAM_CONNECTION_CONNECTION_INDEX, sizeof(SAM_CONNECTION_CONNECTION_INDEX));

        /* Dispose of the error block */
        xmsErrorDispose(&errorBlock);
    }
    if (SAM_DEBUG) zend_printf("<b>SAMXMSConnection->disconnect()</b> exit<br/>");
    RETURN_TRUE;
}
/* }}} */


/* {{{ proto PHP_NAMED_FUNCTION(SAM_Connection_isConnected)
     check if a connection is connected to a messaging server.  */
PHP_FUNCTION(SAM_XMS_Connection_isConnected) {

    php_sam_xms_connection   *connResource;
    zval                    **zConnection;

    if (SAM_DEBUG) zend_printf("<br/><b>SAMXMSConnection->isConnected()</b> enter<br/>");

    /* Retrieve the connection resource from this object's hash table */
    if (zend_hash_find(Z_OBJPROP_P(this_ptr), SAM_CONNECTION_CONNECTION_INDEX, sizeof(SAM_CONNECTION_CONNECTION_INDEX), (void**)&zConnection) == FAILURE) {
        if (SAM_DEBUG) zend_printf("<b>SAMXMSConnection->isConnected()</b> exit (no hashtable entry)<br/>");
        RETURN_FALSE;
    }
    ZEND_FETCH_RESOURCE(connResource, php_sam_xms_connection*, zConnection, -1, PHP_SAM_CONNECTION_RES_NAME, le_sam_xms_connection);

    if (connResource) {
        if (SAM_DEBUG) zend_printf("<b>SAMXMSConnection->isConnected()</b> connection is %d<br/>", connResource->conn);

        if (XMS_NULL_HANDLE == connResource->session) {
           if (SAM_DEBUG) zend_printf("<b>SAMXMSConnection->isConnected()</b> exit (no session)<br/>");
           RETURN_FALSE;
        }

        if (SAM_DEBUG) zend_printf("<b>SAMXMSConnection->isConnected()</b> exit<br/>");
        RETURN_TRUE;
    }

    if (SAM_DEBUG) zend_printf("<b>SAMXMSConnection->isConnected()</b> exit (no resource)<br/>");
    RETURN_FALSE;
}
/* }}} */


/* {{{ proto PHP_NAMED_FUNCTION(SAM_Connection_peek)
     peek at a queue for the next available or a particular message.  */

PHP_FUNCTION(SAM_XMS_Connection_peek)
{
    int rc;

    char *destString = NULL;            /* Destination string parsed in via the argument list */
    int  destLength=0;                   /* Length of the destination given in the arg list */

    zval *zvOptions = NULL;
    HashTable *optionsHT;
    zval **zTemp;

    char                    *szCorrelID = NULL;
    char                    *szMessageID = NULL;
    xmsCHAR                 *szSelector = NULL;

    xmsHErrorBlock          errorBlock;
    xmsHDest                dest = XMS_NULL_HANDLE;
    xmsHQueueBrowser        qbrowser = XMS_NULL_HANDLE;
    xmsHIterator            browserIterator = XMS_NULL_HANDLE;
    xmsHMsg                 message;

    xmsMESSAGE_TYPE         messageType;

    int                     transacted = xmsFALSE;
    xmsCHAR                 *text = NULL;
    xmsINT                  receivedLength;

    xmsHIterator            props;
    xmsHProperty            prop;
    xmsBOOL                 messagesReturned;

    php_sam_xms_connection  *connResource;
    zval                    **zConnection;

    zval*                   msg;
    zval*                   func;
    zval*                   zBody;
    zval*                   parms[1];
    zval*                   zVal;
    zval*                   zMsgHdr;

    xmsCHAR                 *msgID= NULL;
    xmsINT                  length;

    xmsCHAR *propName=NULL, *propValue = NULL;
    xmsINT  pnLength, pvLength;


    if (SAM_DEBUG) zend_printf("<br/><b>SAMXMSConnection->peek()</b> enter<br/>");

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC,
        "s|a", &destString, &destLength, &zvOptions) == FAILURE) {
            setError(this_ptr TSRMLS_CC, SAM_CONNECTION_WRONG_PARAMETERS_PEEK, SAM_CONNECTION_WRONG_PARAMETERS_PEEK_MSG);
            WRONG_PARAM_COUNT;
            RETURN_FALSE;
        }

        if (SAM_DEBUG) zend_printf("<b>SAMXMSConnection->peek()</b> Target destination is: %s.<br/>", destString);


        /* Retrieve the connection resource from this object's hash table */
        if (zend_hash_find(Z_OBJPROP_P(this_ptr), SAM_CONNECTION_CONNECTION_INDEX, sizeof(SAM_CONNECTION_CONNECTION_INDEX), (void**)&zConnection) == FAILURE) {
            setError(this_ptr TSRMLS_CC, SAM_CONNECTION_NO_CONNECTION, SAM_CONNECTION_NO_CONNECTION_MSG);
            return;
        }
        ZEND_FETCH_RESOURCE(connResource, php_sam_xms_connection*, zConnection, -1, PHP_SAM_CONNECTION_RES_NAME, le_sam_xms_connection);
        if (SAM_DEBUG) zend_printf("<b>SAMXMSConnection->peek()</b> connection is %d<br/>", connResource->conn);

       /* Check we have a valid XMS connection */
        if (XMS_NULL_HANDLE == connResource->session) {
            setError(this_ptr TSRMLS_CC, SAM_CONNECTION_NULL_SESSION, SAM_CONNECTION_NULL_SESSION_MSG);
            RETURN_FALSE;
        }

        /* Take a look in the options array if we have one...    */
        if (zvOptions) {
            optionsHT = Z_ARRVAL_P(zvOptions);
            if (zend_hash_num_elements(optionsHT) > 0) {
                optionsHT = HASH_OF(zvOptions);

                /* Has a correlID been specified?      */
                if (zend_hash_find(optionsHT, SAM_CONSTANT_CORRELID, sizeof(SAM_CONSTANT_CORRELID), (void**)&zTemp) == SUCCESS) {
                    convert_to_string(*zTemp);

                    if (NULL == (szCorrelID = emalloc(Z_STRLEN_PP(zTemp)+1))) {
                        setError(this_ptr TSRMLS_CC, SAM_CONNECTION_CORRELID_ALLOC_FAILED, SAM_CONNECTION_CORRELID_ALLOC_FAILED_MSG);
                        RETURN_FALSE;
                    }

                    strncpy(szCorrelID, Z_STRVAL_PP(zTemp), Z_STRLEN_PP(zTemp));
                    szCorrelID[Z_STRLEN_PP(zTemp)]=0;
                    if (SAM_DEBUG) zend_printf("<b>SAMXMSConnection->peek()</b> Caller specified correlID of <b>%s</b><br/>", szCorrelID);
                }

                /* Has a messageID been specified?      */
                if (zend_hash_find(optionsHT, SAM_CONSTANT_MESSAGEID, sizeof(SAM_CONSTANT_MESSAGEID), (void**)&zTemp) == SUCCESS) {
                    convert_to_string(*zTemp);

                    if (NULL == (szMessageID = emalloc(Z_STRLEN_PP(zTemp)+1))) {
                        setError(this_ptr TSRMLS_CC, SAM_CONNECTION_MESSAGEID_ALLOC_FAILED, SAM_CONNECTION_MESSAGEID_ALLOC_FAILED_MSG);
                        RETURN_FALSE;
                    }

                    strncpy(szMessageID, Z_STRVAL_PP(zTemp), Z_STRLEN_PP(zTemp));
                    szMessageID[Z_STRLEN_PP(zTemp)]=0;
                    if (SAM_DEBUG) zend_printf("<b>SAMXMSConnection->peek()</b> Caller specified messageID of <b>%s</b><br/>", szMessageID);
                }
            } /* zend_hash_num_elements(optionsHT) > 0   */
        } else {
            if (SAM_DEBUG) zend_printf("<b>SAMXMSConnection->peek()</b> No options array specified.<br/>");
        } /* zvOptions  */

        if (XMS_OK != xmsErrorCreate(&errorBlock)) {
            setError(this_ptr TSRMLS_CC, SAM_CONNECTION_NO_ERROR_BLOCK, SAM_CONNECTION_NO_ERROR_BLOCK_MSG);
            RETURN_FALSE;
        }

        if (XMS_OK != xmsDestCreate(destString, &dest, errorBlock)) {
            setError(this_ptr TSRMLS_CC, SAM_CONNECTION_NO_DESTINATION, SAM_CONNECTION_NO_DESTINATION_MSG);
            if (SAM_DEBUG) check(errorBlock);
            SAM_CLEANUP_PEEK();
            RETURN_FALSE;
        }

         /* Do we have any selection criteria? (I.e. a correlation id or message id string)    */
        if ( (NULL == szCorrelID) && (NULL == szMessageID) ) {
            /* No, so we can use a queue browser without a select */
            if (SAM_DEBUG) zend_printf("<b>SAMXMSConnection->peek()</b> Creating queue browser<br/>");
            rc = xmsSessCreateBrowser(connResource->session, dest, &qbrowser, errorBlock);
        } else {
            /* Yes, so build a selector string accordingly */
            szSelector = SAMXMSBuildSelectorString(szCorrelID, szMessageID);
            if (SAM_DEBUG) zend_printf("<b>SAMXMSConnection->peek()</b> Creating queue browser with selector - %s<br/>", szSelector);
            rc = xmsSessCreateBrowserSelector(connResource->session, dest, szSelector, XMSC_CALCULATE_STRING_SIZE, &qbrowser, errorBlock);

            /* Free any correlId & message id buffers */
            if (NULL != szCorrelID) efree(szCorrelID);
            if (NULL != szMessageID) efree(szMessageID);

            /* Free the selector string */
            if (NULL != szSelector) efree(szSelector);
        }

        if (XMS_OK != rc) {
            setError(this_ptr TSRMLS_CC, SAM_CONNECTION_NO_QBROWSER, SAM_CONNECTION_NO_QBROWSER_MSG);
            if (SAM_DEBUG) check(errorBlock);
            SAM_CLEANUP_PEEK();
            RETURN_FALSE;
        }

        if (SAM_DEBUG) zend_printf("<b>SAMXMSConnection->peek()</b> About to browse messages<br/>");
        if (XMS_OK != xmsQueueBrowserGetEnumeration(qbrowser, &browserIterator, errorBlock)) {
            setError(this_ptr TSRMLS_CC, SAM_CONNECTION_QBROWSE_FAILED, SAM_CONNECTION_QBROWSE_FAILED_MSG);
            if (SAM_DEBUG) check(errorBlock);
            SAM_CLEANUP_PEEK();
            RETURN_FALSE;
        }

        if (XMS_OK != xmsIteratorHasNext(browserIterator, &messagesReturned, errorBlock)) {
            setError(this_ptr TSRMLS_CC, SAM_CONNECTION_HASNEXT_FAILED, SAM_CONNECTION_HASNEXT_FAILED_MSG);
            if (SAM_DEBUG) check(errorBlock);
            SAM_CLEANUP_PEEK();
            RETURN_FALSE;
        }

        if (!messagesReturned)
        {
            if (SAM_DEBUG) zend_printf("<b>SAMXMSConnection->peek()</b> No matching messages<br/>");
            SAM_CLEANUP_PEEK();
            RETURN_FALSE;
        }

        if (XMS_OK != xmsIteratorGetNext(browserIterator, (xmsHObj*)&message, errorBlock)) {
            setError(this_ptr TSRMLS_CC, SAM_CONNECTION_GETNEXT_FAILED, SAM_CONNECTION_GETNEXT_FAILED_MSG);
            if (SAM_DEBUG) check(errorBlock);
            SAM_CLEANUP_PEEK();
            RETURN_FALSE;
        }

        /* Finished with the iterator so throw it away */
        xmsIteratorDispose(&browserIterator, errorBlock);

        /* get the message type...       */
        if (XMS_OK != xmsMsgGetTypeId(message, &messageType, errorBlock)) {
            setError(this_ptr TSRMLS_CC, SAM_CONNECTION_TYPE_GET_FAILED, SAM_CONNECTION_TYPE_GET_FAILED_MSG);
            if (SAM_DEBUG) check(errorBlock);
            SAM_CLEANUP_PEEK();
            RETURN_FALSE;
        }

        if (messageType == XMS_MESSAGE_TYPE_BYTES) {

            /* Allocate a buffer for the message */
            /* -- First determine the length of the message */
            if (XMS_OK != xmsBytesMsgReadBytes(message, (xmsSBYTE *) text, XMSC_QUERY_SIZE, &receivedLength, errorBlock)) {
                setError(this_ptr TSRMLS_CC, SAM_CONNECTION_READBYTES_FAILED, SAM_CONNECTION_READBYTES_FAILED_MSG);
                if (SAM_DEBUG) check(errorBlock);
                SAM_CLEANUP_PEEK();
                RETURN_FALSE;
            }

            /* -- Now attempt to allocate the buffer */

            if (SAM_DEBUG) zend_printf("<b>SAMXMSConnection->peek()</b> allocating %d bytes for rcvd bytes msg<br/>", receivedLength);
            if (NULL == (text = emalloc(receivedLength))){
                setError(this_ptr TSRMLS_CC, SAM_CONNECTION_MSG_BUFFER_ALLOC_FAILED, SAM_CONNECTION_MSG_BUFFER_ALLOC_FAILED_MSG);
                if (SAM_DEBUG) check(errorBlock);
                SAM_CLEANUP_PEEK();
                RETURN_FALSE;
            }


            if (XMS_OK != xmsBytesMsgReadBytes(message, (xmsSBYTE *) text, receivedLength, &receivedLength, errorBlock)) {
                setError(this_ptr TSRMLS_CC, SAM_CONNECTION_READBYTES_FAILED, SAM_CONNECTION_READBYTES_FAILED_MSG);
                if (SAM_DEBUG) check(errorBlock);
                SAM_CLEANUP_PEEK();
                RETURN_FALSE;
            }
        } else if (messageType == XMS_MESSAGE_TYPE_TEXT) {

            /* Allocate a buffer for the message */
            /* -- First determine the length of the message */
            if (XMS_OK != xmsTextMsgGetText(message, text, XMSC_QUERY_SIZE, &receivedLength, errorBlock)) {
                setError(this_ptr TSRMLS_CC, SAM_CONNECTION_READBYTES_FAILED, SAM_CONNECTION_READBYTES_FAILED_MSG);
                if (SAM_DEBUG) check(errorBlock);
                SAM_CLEANUP_PEEK();
                RETURN_FALSE;
            }

            /* -- Now attempt to allocate the buffer */
            if (SAM_DEBUG) zend_printf("<b>SAMXMSConnection->peek()</b> allocating %d bytes for rcvd text msg<br/>", receivedLength);
            if (NULL == (text = emalloc(receivedLength))){
                setError(this_ptr TSRMLS_CC, SAM_CONNECTION_MSG_BUFFER_ALLOC_FAILED, SAM_CONNECTION_MSG_BUFFER_ALLOC_FAILED_MSG);
                if (SAM_DEBUG) check(errorBlock);
                SAM_CLEANUP_PEEK();
                RETURN_FALSE;

            }

            if (XMS_OK != xmsTextMsgGetText(message, text, receivedLength, &receivedLength, errorBlock)) {
                setError(this_ptr TSRMLS_CC, SAM_CONNECTION_GETTEXT_FAILED, SAM_CONNECTION_GETTEXT_FAILED_MSG);
                if (SAM_DEBUG) check(errorBlock);
                SAM_CLEANUP_PEEK();
                RETURN_FALSE;
            }
        }

        /* create a new SAM_Message object to hold the results...    */
        MAKE_STD_ZVAL(msg);
//        Z_TYPE_P(msg) = IS_OBJECT;
//        object_init_ex(msg, php_sam_SAM_Message_entry);
        zend_eval_string("new SAMMessage();", msg, "sam_xms_connection" TSRMLS_CC);

        /* call the constructor    */
//        MAKE_STD_ZVAL(func);
//        ZVAL_STRING(func, "sammessage", 1);

        MAKE_STD_ZVAL(zBody);
        ZVAL_STRINGL(zBody, text, receivedLength, 1);
        add_property_zval(msg, SAM_MESSAGE_BODY, zBody);
//        parms[0] = zBody;
//        call_user_function(&php_sam_SAM_Message_entry->function_table, &msg, func, return_value, 1, parms TSRMLS_CC);

        return_value->value.obj = msg->value.obj;
        return_value->type = msg->type;


        if (SAM_DEBUG) zend_printf("<b>SAMXMSConnection->peek()</b> Message of %ld bytes peeked<br/>", receivedLength);

        /* Create the SAM message header property...   */
        MAKE_STD_ZVAL(zMsgHdr);
        object_init(zMsgHdr);

        if (add_property_zval(msg, SAM_MESSAGE_HEADER, zMsgHdr) == FAILURE) {
            if (SAM_DEBUG) zend_printf("<b>SAMXMSConnection->peek()</b> Unable to create header property in message!<br/>", receivedLength);
            setError(this_ptr TSRMLS_CC, SAM_CONNECTION_HEADER_CREATE_FAILED, SAM_CONNECTION_HEADER_CREATE_FAILED_MSG);
            SAM_CLEANUP_PEEK();
            RETURN_FALSE;
        }

        /* find the message id...                                            */
        if (XMS_OK == xmsMsgGetJMSMessageID(message, msgID, XMSC_QUERY_SIZE, &length, errorBlock)) {
            msgID = emalloc(length);
            if (XMS_OK == xmsMsgGetJMSMessageID(message, msgID, length, NULL, errorBlock)) {
                MAKE_STD_ZVAL(zVal);
                ZVAL_STRINGL(zVal, msgID, length, 1);
                if (add_property_zval(zMsgHdr, SAM_CONSTANT_MESSAGEID, zVal) == FAILURE) {
                    setError(this_ptr TSRMLS_CC, SAM_CONNECTION_MESSAGE_PROPERTY_SET_FAILED, SAM_CONNECTION_MESSAGE_PROPERTY_SET_FAILED_MSG);
                    SAM_CLEANUP_PEEK();
                    RETURN_FALSE;
                }
            }
            if (msgID != NULL) efree(msgID);
        }

        /* see what properties we have in the XMS message that we can reflect in the SAM message...    */
        if (XMS_OK != xmsMsgGetProperties(message, &props, errorBlock)) {
            if (SAM_DEBUG) zend_printf("<b>SAMXMSConnection->peek()</b> Unable to retrieve message properties!<br/>", receivedLength);
        } else {
            /* Walk through the properties...   */
            while(XMS_OK == xmsIteratorGetNext(props, (xmsHObj*)&prop, errorBlock)) {
                if (XMS_OK != xmsPropertyGetName((xmsHProperty) prop, propName, XMSC_QUERY_SIZE, &pnLength, errorBlock)) {
                    setError(this_ptr TSRMLS_CC, SAM_CONNECTION_PROPERTY_GET_NAME_FAILED, SAM_CONNECTION_PROPERTY_GET_NAME_FAILED_MSG);
                    if (SAM_DEBUG) check(errorBlock);
                    SAM_CLEANUP_PEEK();
                    RETURN_FALSE;
                }

                if (NULL == (propName = emalloc(pnLength))) {
                    setError(this_ptr TSRMLS_CC, SAM_CONNECTION_PROPNAME_ALLOC_FAILED, SAM_CONNECTION_PROPNAME_ALLOC_FAILED_MSG);
                    if (SAM_DEBUG) check(errorBlock);
                    SAM_CLEANUP_PEEK();
                    RETURN_FALSE;
                }

                if (XMS_OK != xmsPropertyGetName((xmsHProperty) prop, propName, pnLength, &pnLength, errorBlock)) {
                    setError(this_ptr TSRMLS_CC, SAM_CONNECTION_PROPERTY_GET_NAME_FAILED, SAM_CONNECTION_PROPERTY_GET_NAME_FAILED_MSG);
                    if (SAM_DEBUG) check(errorBlock);
                    SAM_CLEANUP_PEEK();
                    RETURN_FALSE;
                }

                if (XMS_OK != xmsPropertyGetString((xmsHProperty) prop, propValue, XMSC_QUERY_SIZE, &pvLength, errorBlock)) {
                    setError(this_ptr TSRMLS_CC, SAM_CONNECTION_PROPERTY_GET_STRING_FAILED, SAM_CONNECTION_PROPERTY_GET_STRING_FAILED_MSG);
                    if (SAM_DEBUG) check(errorBlock);
                    SAM_CLEANUP_PEEK();
                    RETURN_FALSE;
                }

                if (NULL == (propValue = emalloc(pvLength))) {
                    setError(this_ptr TSRMLS_CC, SAM_CONNECTION_PROPVAL_ALLOC_FAILED, SAM_CONNECTION_PROPVAL_ALLOC_FAILED_MSG);
                    if (SAM_DEBUG) check(errorBlock);
                    SAM_CLEANUP_PEEK();
                    RETURN_FALSE;
                }

                if (XMS_OK != xmsPropertyGetString((xmsHProperty) prop, propValue, pvLength, &pvLength, errorBlock)) {
                    setError(this_ptr TSRMLS_CC, SAM_CONNECTION_PROPERTY_GET_STRING_FAILED, SAM_CONNECTION_PROPERTY_GET_STRING_FAILED_MSG);
                    if (SAM_DEBUG) check(errorBlock);
                    SAM_CLEANUP_PEEK();
                    RETURN_FALSE;
                }

                if (SAM_DEBUG) zend_printf("<b>SAMXMSConnection->peek()</b> Retrieved message property '%s' value=%s<br/>", propName, propValue);

                MAKE_STD_ZVAL(zVal);
                ZVAL_STRINGL(zVal, propValue, pvLength, 1);
                if (add_property_zval(zMsgHdr, propName, zVal) == FAILURE) {
                    setError(this_ptr TSRMLS_CC, SAM_CONNECTION_MESSAGE_PROPERTY_SET_FAILED, SAM_CONNECTION_MESSAGE_PROPERTY_SET_FAILED_MSG);
                    SAM_CLEANUP_PEEK();
                    RETURN_FALSE;
                }

                if (NULL != propName) efree(propName);
                if (NULL != propValue) efree(propValue);
            } /*while*/

            /* Finished with the iterator so throw it away...   */
            xmsIteratorDispose(&props, errorBlock);
        }

        /* if we understood then set the message type as a header property...   */
        MAKE_STD_ZVAL(zVal);
        if (messageType == XMS_MESSAGE_TYPE_BYTES) {
            ZVAL_STRING(zVal, SAM_CONSTANT_BYTES, 1);
        } else if (messageType == XMS_MESSAGE_TYPE_TEXT) {
            ZVAL_STRING(zVal, SAM_CONSTANT_TEXT, 1);
        } else {
            ZVAL_STRING(zVal, SAM_CONSTANT_UNKNOWN, 1);
        }
        if (add_property_zval(zMsgHdr, SAM_CONSTANT_TYPE, zVal) == FAILURE) {
            setError(this_ptr TSRMLS_CC, SAM_CONNECTION_MESSAGE_PROPERTY_SET_FAILED, SAM_CONNECTION_MESSAGE_PROPERTY_SET_FAILED_MSG);
            SAM_CLEANUP_PEEK();
            RETURN_FALSE;
        }


        SAM_CLEANUP_PEEK();

        if (SAM_DEBUG) zend_printf("<b>SAMXMSConnection->peek()</b> exit<br/>");
        return;
}
/* }}} */


/* {{{ proto PHP_NAMED_FUNCTION(SAM_Connection_peekAll)
     peek at a queue for all available or a particular set of messages.  */
PHP_FUNCTION(SAM_XMS_Connection_peekAll)
{
    int rc;

    char *destString = NULL;            /* Destination string parsed in via the argument list */
    int  destLength=0;                  /* Length of the destination given in the arg list */

    zval *zvOptions = NULL;
    HashTable *optionsHT;
    zval **zTemp;

    char                    *szCorrelID = NULL;
    char                    *szMessageID = NULL;
    xmsCHAR                 *szSelector = NULL;

    xmsHErrorBlock          errorBlock;
    xmsHDest                dest = XMS_NULL_HANDLE;
    xmsHQueueBrowser        qbrowser = XMS_NULL_HANDLE;
    xmsHIterator            browserIterator = XMS_NULL_HANDLE;
    xmsHMsg                 message;

    xmsMESSAGE_TYPE         messageType;

    int                     transacted = xmsFALSE;
    xmsCHAR                 *text = NULL;
    xmsINT                  receivedLength;


    xmsHIterator            props;
    xmsHProperty            prop;
    xmsBOOL                 messagesReturned;

    php_sam_xms_connection  *connResource;
    zval                    **zConnection;

    zval*                   func;
    zval*                   zBody;
    zval*                   parms[1];
    zval*                   zVal;
    zval*                   zMsgHdr;

    xmsCHAR                 *msgID = NULL;
    xmsINT                  length;

    xmsCHAR                 *propName = NULL;
    xmsCHAR                 *propValue = NULL;
    xmsINT                  pnLength, pvLength;

    if (SAM_DEBUG) zend_printf("<br/><b>SAMXMSConnection->peekAll()</b> enter<br/>");

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC,
        "s|a", &destString, &destLength, &zvOptions) == FAILURE) {
            setError(this_ptr TSRMLS_CC, SAM_CONNECTION_WRONG_PARAMETERS_PEEKALL, SAM_CONNECTION_WRONG_PARAMETERS_PEEKALL_MSG);
            WRONG_PARAM_COUNT;
            RETURN_FALSE;
        }

        if (SAM_DEBUG) zend_printf("<b>SAMXMSConnection->peekAll()</b> Target destination is: %s.<br/>", destString);


        /* Retrieve the connection resource from this object's hash table */
        if (zend_hash_find(Z_OBJPROP_P(this_ptr), SAM_CONNECTION_CONNECTION_INDEX, sizeof(SAM_CONNECTION_CONNECTION_INDEX), (void**)&zConnection) == FAILURE) {
            setError(this_ptr TSRMLS_CC, SAM_CONNECTION_NO_CONNECTION, SAM_CONNECTION_NO_CONNECTION_MSG);
            return;
        }
        ZEND_FETCH_RESOURCE(connResource, php_sam_xms_connection*, zConnection, -1, PHP_SAM_CONNECTION_RES_NAME, le_sam_xms_connection);
        if (SAM_DEBUG) zend_printf("<b>SAMXMSConnection->peekAll()</b> connection is %d<br/>", connResource->conn);

        /* Check we have a valid XMS session handle */
        if (XMS_NULL_HANDLE == connResource->session) {
            setError(this_ptr TSRMLS_CC, SAM_CONNECTION_NULL_SESSION, SAM_CONNECTION_NULL_SESSION_MSG);
            RETURN_FALSE;
        }

        /* Take a look in the options array if we have one...    */
        if (zvOptions) {
            optionsHT = Z_ARRVAL_P(zvOptions);
            if (zend_hash_num_elements(optionsHT) > 0) {
                optionsHT = HASH_OF(zvOptions);

                /* Has a correlID been specified?      */
                if (zend_hash_find(optionsHT, SAM_CONSTANT_CORRELID, sizeof(SAM_CONSTANT_CORRELID), (void**)&zTemp) == SUCCESS) {
                    convert_to_string(*zTemp);

                    if (NULL == (szCorrelID = emalloc(Z_STRLEN_PP(zTemp)+1))) {
                        setError(this_ptr TSRMLS_CC, SAM_CONNECTION_CORRELID_ALLOC_FAILED, SAM_CONNECTION_CORRELID_ALLOC_FAILED_MSG);
                        RETURN_FALSE;
                    }

                    strncpy(szCorrelID, Z_STRVAL_PP(zTemp), Z_STRLEN_PP(zTemp));
                    szCorrelID[Z_STRLEN_PP(zTemp)]=0;
                    if (SAM_DEBUG) zend_printf("<b>SAMXMSConnection->peekAll()</b> Caller specified correlID of <b>%s</b><br/>", szCorrelID);
                }

                /* Has a messageID been specified?      */
                if (zend_hash_find(optionsHT, SAM_CONSTANT_MESSAGEID, sizeof(SAM_CONSTANT_MESSAGEID), (void**)&zTemp) == SUCCESS) {
                    convert_to_string(*zTemp);

                    if (NULL == (szMessageID = emalloc(Z_STRLEN_PP(zTemp)+1))) {
                        setError(this_ptr TSRMLS_CC, SAM_CONNECTION_MESSAGEID_ALLOC_FAILED, SAM_CONNECTION_MESSAGEID_ALLOC_FAILED_MSG);
                        RETURN_FALSE;
                    }

                    strncpy(szMessageID, Z_STRVAL_PP(zTemp), Z_STRLEN_PP(zTemp));
                    szMessageID[Z_STRLEN_PP(zTemp)]=0;
                    if (SAM_DEBUG) zend_printf("<b>SAMXMSConnection->peekAll()</b> Caller specified messageID of <b>%s</b><br/>", szMessageID);
                }

            } /* zend_hash_num_elements(optionsHT) > 0   */
        } else {
            if (SAM_DEBUG) zend_printf("<b>SAMXMSConnection->peekAll()</b> No options array specified.<br/>");
        } /* zvOptions  */

        if (XMS_OK != xmsErrorCreate(&errorBlock)) {
            setError(this_ptr TSRMLS_CC, SAM_CONNECTION_NO_ERROR_BLOCK, SAM_CONNECTION_NO_ERROR_BLOCK_MSG);
            RETURN_FALSE;
        }

        if (XMS_OK != xmsDestCreate(destString, &dest, errorBlock)) {
            setError(this_ptr TSRMLS_CC, SAM_CONNECTION_NO_DESTINATION, SAM_CONNECTION_NO_DESTINATION_MSG);
            if (SAM_DEBUG) check(errorBlock);
            SAM_CLEANUP_PEEKALL();
            RETURN_FALSE;
        }

        /* Do we have any selection criteria? (I.e. a correlation id or message id string)    */
        if ( (NULL == szCorrelID) && (NULL == szMessageID) ) {
            /* No, so we can use a queue browser without a select */
            if (SAM_DEBUG) zend_printf("<b>SAMXMSConnection->peekAll()</b> Creating queue browser<br/>");
            rc = xmsSessCreateBrowser(connResource->session, dest, &qbrowser, errorBlock);
        } else {
            /* Yes, so build a selector string accordingly */
            szSelector = SAMXMSBuildSelectorString(szCorrelID, szMessageID);
            if (SAM_DEBUG) zend_printf("<b>SAMXMSConnection->peekAll()</b> Creating queue browser with selector - %s<br/>", szSelector);
            rc = xmsSessCreateBrowserSelector(connResource->session, dest, szSelector, XMSC_CALCULATE_STRING_SIZE, &qbrowser, errorBlock);

            /* Free any correlId & message id buffers */
            if (NULL != szCorrelID) efree(szCorrelID);
            if (NULL != szMessageID) efree(szMessageID);

            /* Free any selector string */
            if (NULL != szSelector) efree(szSelector);
        }


        if (XMS_OK != rc) {
            setError(this_ptr TSRMLS_CC, SAM_CONNECTION_NO_QBROWSER, SAM_CONNECTION_NO_QBROWSER_MSG);
            if (SAM_DEBUG) check(errorBlock);
            SAM_CLEANUP_PEEKALL();
            RETURN_FALSE;
        }

        if (SAM_DEBUG) zend_printf("<b>SAMXMSConnection->peekAll()</b> About to browse messages<br/>");
        if (XMS_OK != xmsQueueBrowserGetEnumeration(qbrowser, &browserIterator, errorBlock)) {
            setError(this_ptr TSRMLS_CC, SAM_CONNECTION_QBROWSE_FAILED, SAM_CONNECTION_QBROWSE_FAILED_MSG);
            if (SAM_DEBUG) check(errorBlock);
            SAM_CLEANUP_PEEKALL();
            RETURN_FALSE;
        }

        if (XMS_OK != xmsIteratorHasNext(browserIterator, &messagesReturned, errorBlock)) {
            setError(this_ptr TSRMLS_CC, SAM_CONNECTION_HASNEXT_FAILED, SAM_CONNECTION_HASNEXT_FAILED_MSG);
            if (SAM_DEBUG) check(errorBlock);
            SAM_CLEANUP_PEEKALL();
            RETURN_FALSE;
        }

        if (!messagesReturned)
        {
            if (SAM_DEBUG) zend_printf("<b>SAMXMSConnection->peekAll()</b> No matching messages<br/>");
            SAM_CLEANUP_PEEKALL();
            RETURN_FALSE;
        }

        /* Initialise the array of messages to be returned */
        array_init(return_value);

        while (XMS_OK == xmsIteratorGetNext(browserIterator, (xmsHObj*)&message, errorBlock))
        {
            /* get the message type...       */
            if (XMS_OK != xmsMsgGetTypeId(message, &messageType, errorBlock)) {
                setError(this_ptr TSRMLS_CC, SAM_CONNECTION_TYPE_GET_FAILED, SAM_CONNECTION_TYPE_GET_FAILED_MSG);
                if (SAM_DEBUG) check(errorBlock);
                SAM_CLEANUP_PEEKALL();
                RETURN_FALSE;
            }

            if (messageType == XMS_MESSAGE_TYPE_BYTES) {

                /* Allocate a buffer for the message */
                /* -- First determine the length of the message */
                if (XMS_OK != xmsBytesMsgReadBytes(message, (xmsSBYTE *) text, XMSC_QUERY_SIZE, &receivedLength, errorBlock)) {
                    setError(this_ptr TSRMLS_CC, SAM_CONNECTION_READBYTES_FAILED, SAM_CONNECTION_READBYTES_FAILED_MSG);
                    if (SAM_DEBUG) check(errorBlock);
                    SAM_CLEANUP_PEEKALL();
                    RETURN_FALSE;
                }

                /* -- Now attempt to allocate the buffer */
                if (SAM_DEBUG) zend_printf("<b>SAMXMSConnection->peek()</b> allocating %d bytes for rcvd bytes msg<br/>", receivedLength);
                if (NULL == (text = emalloc(receivedLength))){
                    setError(this_ptr TSRMLS_CC, SAM_CONNECTION_MSG_BUFFER_ALLOC_FAILED, SAM_CONNECTION_MSG_BUFFER_ALLOC_FAILED_MSG);
                    if (SAM_DEBUG) check(errorBlock);
                    SAM_CLEANUP_PEEKALL();
                    RETURN_FALSE;
                }

                if (XMS_OK != xmsBytesMsgReadBytes(message, (xmsSBYTE *) text, receivedLength, &receivedLength, errorBlock)) {
                    setError(this_ptr TSRMLS_CC, SAM_CONNECTION_READBYTES_FAILED, SAM_CONNECTION_READBYTES_FAILED_MSG);
                    if (SAM_DEBUG) check(errorBlock);
                    SAM_CLEANUP_PEEKALL();
                    RETURN_FALSE;
                }

            } else if (messageType == XMS_MESSAGE_TYPE_TEXT) {
                /* Allocate a buffer for the message */
                /* -- First determine the length of the message */
                if (XMS_OK != xmsTextMsgGetText(message, text, XMSC_QUERY_SIZE, &receivedLength, errorBlock)) {
                    setError(this_ptr TSRMLS_CC, SAM_CONNECTION_READBYTES_FAILED, SAM_CONNECTION_READBYTES_FAILED_MSG);
                    if (SAM_DEBUG) check(errorBlock);
                    SAM_CLEANUP_PEEKALL();
                    RETURN_FALSE;
                }

                /* -- Now attempt to allocate the buffer */
                if (SAM_DEBUG) zend_printf("<b>SAMXMSConnection->peek()</b> allocating %d bytes for rcvd bytes msg<br/>", receivedLength);
                if (NULL == (text = emalloc(receivedLength))){
                    setError(this_ptr TSRMLS_CC, SAM_CONNECTION_MSG_BUFFER_ALLOC_FAILED, SAM_CONNECTION_MSG_BUFFER_ALLOC_FAILED_MSG);
                    if (SAM_DEBUG) check(errorBlock);
                    SAM_CLEANUP_PEEKALL();
                    RETURN_FALSE;

                }

                if (XMS_OK != xmsTextMsgGetText(message, text, receivedLength, &receivedLength, errorBlock)) {
                    setError(this_ptr TSRMLS_CC, SAM_CONNECTION_GETTEXT_FAILED, SAM_CONNECTION_GETTEXT_FAILED_MSG);
                    if (SAM_DEBUG) check(errorBlock);
                    SAM_CLEANUP_PEEKALL();
                    RETURN_FALSE;
                }
            }

            /* create a new SAM_Message object to hold the results...    */
            {
                zval* msg;
                zval* cnstrRetVal;
                MAKE_STD_ZVAL(msg);

                zend_eval_string("new SAMMessage();", msg, "sam_xms_connection" TSRMLS_CC);

                MAKE_STD_ZVAL(zBody);
                ZVAL_STRINGL(zBody, text, receivedLength, 1);
                add_property_zval(msg, SAM_MESSAGE_BODY, zBody);

                /* Add this message to the array of messages to be returned */
                add_next_index_zval(return_value, msg);

                if (SAM_DEBUG) zend_printf("<b>SAMXMSConnection->peekAll()</b> Message of %ld bytes peeked<br/>", receivedLength);

                /* Create the SAM message header property...   */
                MAKE_STD_ZVAL(zMsgHdr);
                object_init(zMsgHdr);

                if (add_property_zval(msg, SAM_MESSAGE_HEADER, zMsgHdr) == FAILURE) {
                    if (SAM_DEBUG) zend_printf("<b>SAMXMSConnection->peekAll()</b> Unable to create header property in message!<br/>", receivedLength);
                    setError(this_ptr TSRMLS_CC, SAM_CONNECTION_HEADER_CREATE_FAILED, SAM_CONNECTION_HEADER_CREATE_FAILED_MSG);
                    SAM_CLEANUP_PEEKALL();
                    RETURN_FALSE;
                }


                /* find the message id...                                            */
                if (XMS_OK == xmsMsgGetJMSMessageID(message, msgID, XMSC_QUERY_SIZE, &length, errorBlock)) {
                    msgID = emalloc(length);
                    if (XMS_OK == xmsMsgGetJMSMessageID(message, msgID, length, NULL, errorBlock)) {
                        MAKE_STD_ZVAL(zVal);
                        ZVAL_STRINGL(zVal, msgID, length, 1);
                        if (add_property_zval(zMsgHdr, SAM_CONSTANT_MESSAGEID, zVal) == FAILURE) {
                            setError(this_ptr TSRMLS_CC, SAM_CONNECTION_MESSAGE_PROPERTY_SET_FAILED, SAM_CONNECTION_MESSAGE_PROPERTY_SET_FAILED_MSG);
                            SAM_CLEANUP_PEEKALL();
                            RETURN_FALSE;
                        }
                    }
                    if (msgID != NULL)
                    {
                        efree(msgID);
                        msgID = NULL;
                    }
                }
                /* see what properties we have in the XMS message that we can reflect in the SAM message...    */
                if (XMS_OK != xmsMsgGetProperties(message, &props, errorBlock)) {
                    if (SAM_DEBUG) zend_printf("<b>SAMXMSConnection->peekAll()</b> Unable to retrieve message properties!<br/>", receivedLength);
                } else {
                    /* Walk through the properties...   */
                    while(XMS_OK == xmsIteratorGetNext(props, (xmsHObj*)&prop, errorBlock)) {

                        if (XMS_OK != xmsPropertyGetName((xmsHProperty) prop, propName, XMSC_QUERY_SIZE, &pnLength, errorBlock)) {
                            setError(this_ptr TSRMLS_CC, SAM_CONNECTION_PROPERTY_GET_NAME_FAILED, SAM_CONNECTION_PROPERTY_GET_NAME_FAILED_MSG);
                            if (SAM_DEBUG) check(errorBlock);
                            SAM_CLEANUP_PEEKALL();
                            RETURN_FALSE;
                        }

                        if (NULL == (propName = emalloc(pnLength))) {
                            setError(this_ptr TSRMLS_CC, SAM_CONNECTION_PROPNAME_ALLOC_FAILED, SAM_CONNECTION_PROPNAME_ALLOC_FAILED_MSG);
                            if (SAM_DEBUG) check(errorBlock);
                            SAM_CLEANUP_PEEKALL();
                            RETURN_FALSE;
                        }

                        if (XMS_OK != xmsPropertyGetName((xmsHProperty) prop, propName, pnLength, &pnLength, errorBlock)) {
                            setError(this_ptr TSRMLS_CC, SAM_CONNECTION_PROPERTY_GET_NAME_FAILED, SAM_CONNECTION_PROPERTY_GET_NAME_FAILED_MSG);
                            if (SAM_DEBUG) check(errorBlock);
                            SAM_CLEANUP_PEEKALL();
                            RETURN_FALSE;
                        }

                        if (XMS_OK != xmsPropertyGetString((xmsHProperty) prop, propValue, XMSC_QUERY_SIZE, &pvLength, errorBlock)) {
                            setError(this_ptr TSRMLS_CC, SAM_CONNECTION_PROPERTY_GET_STRING_FAILED, SAM_CONNECTION_PROPERTY_GET_STRING_FAILED_MSG);
                            if (SAM_DEBUG) check(errorBlock);
                            SAM_CLEANUP_PEEKALL();
                            RETURN_FALSE;
                        }

                        if (NULL == (propValue = emalloc(pvLength))) {
                            setError(this_ptr TSRMLS_CC, SAM_CONNECTION_PROPVAL_ALLOC_FAILED, SAM_CONNECTION_PROPVAL_ALLOC_FAILED_MSG);
                            if (SAM_DEBUG) check(errorBlock);
                            SAM_CLEANUP_PEEKALL();
                            RETURN_FALSE;
                        }

                        if (XMS_OK != xmsPropertyGetString((xmsHProperty) prop, propValue, pvLength, &pvLength, errorBlock)) {
                            setError(this_ptr TSRMLS_CC, SAM_CONNECTION_PROPERTY_GET_STRING_FAILED, SAM_CONNECTION_PROPERTY_GET_STRING_FAILED_MSG);
                            if (SAM_DEBUG) check(errorBlock);
                            SAM_CLEANUP_PEEKALL();
                            RETURN_FALSE;
                        }
                        if (SAM_DEBUG) zend_printf("<b>SAMXMSConnection->peekAll()</b> Retrieved message property '%s' value=%s<br/>", propName, propValue);

                        MAKE_STD_ZVAL(zVal);
                        ZVAL_STRINGL(zVal, propValue, pvLength, 1);
                        if (add_property_zval(zMsgHdr, propName, zVal) == FAILURE) {
                            setError(this_ptr TSRMLS_CC, SAM_CONNECTION_MESSAGE_PROPERTY_SET_FAILED, SAM_CONNECTION_MESSAGE_PROPERTY_SET_FAILED_MSG);
                            SAM_CLEANUP_PEEKALL();
                            RETURN_FALSE;
                        }

                        if (NULL != propName) efree(propName);
                        if (NULL != propValue) efree(propValue);
                    } /*while*/

                    /* Finished with the iterator so throw it away...   */
                    xmsIteratorDispose(&props, errorBlock);
                }

                /* if we understood then set the message type as a header property...   */
                MAKE_STD_ZVAL(zVal);
                if (messageType == XMS_MESSAGE_TYPE_BYTES) {
                    ZVAL_STRING(zVal, SAM_CONSTANT_BYTES, 1);
                } else if (messageType == XMS_MESSAGE_TYPE_TEXT) {
                    ZVAL_STRING(zVal, SAM_CONSTANT_TEXT, 1);
                } else {
                    ZVAL_STRING(zVal, SAM_CONSTANT_UNKNOWN, 1);
                }
                if (add_property_zval(zMsgHdr, SAM_CONSTANT_TYPE, zVal) == FAILURE) {
                    setError(this_ptr TSRMLS_CC, SAM_CONNECTION_MESSAGE_PROPERTY_SET_FAILED, SAM_CONNECTION_MESSAGE_PROPERTY_SET_FAILED_MSG);
                    SAM_CLEANUP_PEEKALL();
                    RETURN_FALSE;
                }
            }

            /* Free the buffer we allocated for the data retrieved from the message */
            if (NULL != text) efree(text);
            text = NULL;
        }

        /* Finished with the iterator so throw it away */
        xmsIteratorDispose(&browserIterator, errorBlock);

        SAM_CLEANUP_PEEKALL();

        if (SAM_DEBUG) zend_printf("<b>SAMXMSConnection->peekAll()</b> exit<br/>");
        return;
}
/* }}} */


/* {{{ proto PHP_NAMED_FUNCTION(SAM_Connection_receive)
     remove the next available or a particular message from a queue.  */

PHP_FUNCTION(SAM_XMS_Connection_receive) {

    int rc;

    char *destString = NULL;            /* Destination string parsed in via the argument list */
    int  destLength=0;                   /* Length of the destination given in the arg list */

    char destPrefix[128] = { '\0' };    /* The destination prefix - topic, queue or sub */
    char aDestination[128] = { '\0' };  /* The destination parsed out of the destination given in the argument list */
    char *tempString;

    char aSubscriptionName[128] ={ '\0' };
    int subNameLength = 0;

    xmsBOOL noLocal = xmsFALSE;

    zval *zvOptions = NULL;

    HashTable *optionsHT;
    zval **zTemp;

    char                    *szCorrelID = NULL;
    char                    *szMessageID = NULL;
    xmsCHAR                 *szSelector = NULL;

    xmsINT                  waitTimeout = 0;

    xmsHErrorBlock          errorBlock;
    xmsHDest                dest = XMS_NULL_HANDLE;
    xmsHMsgConsumer         consumer = XMS_NULL_HANDLE;
    xmsHMsg                 message;
    int                     transacted = xmsFALSE;
    xmsCHAR                 *text = NULL;
    xmsINT                  receivedLength;


    xmsHIterator            props;
    xmsHProperty            prop;
    xmsMESSAGE_TYPE         messageType;

    php_sam_xms_connection  *connResource;
    zval                    **zConnection;

    zval*                   msg;
    zval*                   func;
    zval*                   zBody;
    zval*                   parms[1];
    zval*                   zVal;
    zval*                   zMsgHdr;
    int i,j;

    xmsCHAR                 *msgID = NULL;
    xmsINT                  length;

    xmsCHAR                 *propName = NULL;
    xmsCHAR                 *propValue = NULL;
    xmsINT                  pnLength, pvLength;

    if (SAM_DEBUG) zend_printf("<br/><b>SAMXMSConnection->receive()</b> enter<br/>");

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC,
        "s|a", &destString, &destLength, &zvOptions) == FAILURE) {
            setError(this_ptr TSRMLS_CC, SAM_CONNECTION_WRONG_PARAMETERS_RECEIVE, SAM_CONNECTION_WRONG_PARAMETERS_RECEIVE_MSG);
            WRONG_PARAM_COUNT;
            RETURN_FALSE;
        }

        if (SAM_DEBUG) zend_printf("<b>SAMXMSConnection->receive()</b> Target destination is: %s.<br/>", destString);


        /* Extract the prefix from the destination string, this should be "queue", "topic" or "sub" */
        for (i = 0, j = 0; j >= 0 && i < destLength && j < sizeof(destPrefix); i++, j++) {
            switch (destString[i]) {
        case '\0':
        case SAM_CONNECTION_PROTOCOL_SEPERATOR:
            destPrefix[j] = '\0';
            j = -2;
            break;

        default:
            destPrefix[j] = destString[i];
            break;
            }
        }

        if (SAM_DEBUG) zend_printf("<b>SAMXMSConnection->receive()</b> Destination prefix: %s.<br/>", destPrefix);

        /* Parse the destination in the argument list 
        For destinations not prefixed with "sub:" we just use the whole argument as the destination
        Otherwise the destination string is expected to be in the form:
        sub://....subscription name...._topic://.......
        so we then parse out the destination from the end of the string */
        if  ( (0 == strcmp(destPrefix, SAM_CONSTANT_TOPIC)) || (0 == strcmp(destPrefix, SAM_CONSTANT_QUEUE)) ) {
            /* We do not have a destination with a subscription name so just use the input parameter
            as the destination string */
            if (SAM_DEBUG) zend_printf("<b>SAMXMSConnection->receive()</b> No subscription name provided.<br/>");

            strncpy(aDestination, destString, destLength);
        } else if (0 == strcmp(destPrefix, SAM_CONSTANT_SUB)) {
            /* We have a destination with a subscription name. The topic should be at the end. Parse out
            the topic and subscription name */
            if ((destString == strstr(destString, SAM_CONSTANT_SUBPREFIX)) &&
                (NULL != (tempString = strstr(destString, SAM_CONSTANT_TOPICPREFIX)))) {
                    if (SAM_DEBUG) zend_printf("<b>SAMXMSConnection->receive()</b> Subscription name type destination provided.<br/>");

                    /* Extract the trailing destination from the supplied subscription name URL */
                    strcpy(aDestination, tempString);

                    /* The whole of our sub:// destination string is the subscription name */
                    strncpy(aSubscriptionName, destString, destLength);
                    subNameLength = strlen(aSubscriptionName);

                    if (SAM_DEBUG) zend_printf("<b>SAMXMSConnection->receive()</b> Parsed subscription name: %s<br/>", aSubscriptionName);
                }
            else {
                /* We have a subscription type destination with no topic, this is invalid */
                setError(this_ptr TSRMLS_CC, SAM_CONNECTION_NO_DESTINATION, SAM_CONNECTION_NO_DESTINATION_MSG);
                RETURN_FALSE;
            }
        } else {
            /* We have a destination with an illegal prefix */
            setError(this_ptr TSRMLS_CC, SAM_CONNECTION_ILLEGAL_DEST_PREFIX, SAM_CONNECTION_ILLEGAL_DEST_PREFIX_MSG);
            RETURN_FALSE;
        }

        /* Retrieve the connection resource from this object's hash table */
        if (zend_hash_find(Z_OBJPROP_P(this_ptr), SAM_CONNECTION_CONNECTION_INDEX, sizeof(SAM_CONNECTION_CONNECTION_INDEX), (void**)&zConnection) == FAILURE) {
            setError(this_ptr TSRMLS_CC, SAM_CONNECTION_NO_CONNECTION, SAM_CONNECTION_NO_CONNECTION_MSG);
            return;
        }
        ZEND_FETCH_RESOURCE(connResource, php_sam_xms_connection*, zConnection, -1, PHP_SAM_CONNECTION_RES_NAME, le_sam_xms_connection);
        if (SAM_DEBUG) zend_printf("<b>SAMXMSConnection->receive()</b> connection is %d<br/>", connResource->conn);

       /* Check we have a valid XMS session handle */
        if (XMS_NULL_HANDLE == connResource->session) {
            setError(this_ptr TSRMLS_CC, SAM_CONNECTION_NULL_SESSION, SAM_CONNECTION_NULL_SESSION_MSG);
            RETURN_FALSE;
        }

        /* Take a look in the options array if we have one...    */
        if (zvOptions) {
            optionsHT = Z_ARRVAL_P(zvOptions);
            if (zend_hash_num_elements(optionsHT) > 0) {
                optionsHT = HASH_OF(zvOptions);

                /* Has a correlID been specified?      */
                if (zend_hash_find(optionsHT, SAM_CONSTANT_CORRELID, sizeof(SAM_CONSTANT_CORRELID), (void**)&zTemp) == SUCCESS) {
                    convert_to_string(*zTemp);

                    if (NULL == (szCorrelID = emalloc(Z_STRLEN_PP(zTemp)+1))) {
                        setError(this_ptr TSRMLS_CC, SAM_CONNECTION_CORRELID_ALLOC_FAILED, SAM_CONNECTION_CORRELID_ALLOC_FAILED_MSG);
                        RETURN_FALSE;
                    }

                    strncpy(szCorrelID, Z_STRVAL_PP(zTemp), Z_STRLEN_PP(zTemp));
                    szCorrelID[Z_STRLEN_PP(zTemp)]=0;
                    if (SAM_DEBUG) zend_printf("<b>SAMXMSConnection->receive()</b> Caller specified correlID of <b>%s</b><br/>", szCorrelID);
                }

                /* Has a messageID been specified?      */
                if (zend_hash_find(optionsHT, SAM_CONSTANT_MESSAGEID, sizeof(SAM_CONSTANT_MESSAGEID), (void**)&zTemp) == SUCCESS) {
                    convert_to_string(*zTemp);

                    if (NULL == (szMessageID = emalloc(Z_STRLEN_PP(zTemp)+1))) {
                        setError(this_ptr TSRMLS_CC, SAM_CONNECTION_MESSAGEID_ALLOC_FAILED, SAM_CONNECTION_MESSAGEID_ALLOC_FAILED_MSG);
                        RETURN_FALSE;
                    }

                    strncpy(szMessageID, Z_STRVAL_PP(zTemp), Z_STRLEN_PP(zTemp));
                    szMessageID[Z_STRLEN_PP(zTemp)]=0;
                    if (SAM_DEBUG) zend_printf("<b>SAMXMSConnection->receive()</b> Caller specified messageID of <b>%s</b><br/>", szMessageID);
                }

                /* Has a timeout value been specified?      */
                if (zend_hash_find(optionsHT, SAM_CONSTANT_WAIT, sizeof(SAM_CONSTANT_WAIT), (void**)&zTemp) == SUCCESS) {
                    convert_to_long(*zTemp);
                    waitTimeout = Z_LVAL_PP(zTemp);
                    if (SAM_DEBUG) zend_printf("<b>SAMXMSConnection->receive()</b> Caller specified wait timeout of <b>%ld</b><br/>", waitTimeout);
                }
            } /* zend_hash_num_elements(optionsHT) > 0   */
        } else {
            if (SAM_DEBUG) zend_printf("<b>SAMXMSConnection->receive()</b> No options array specified.<br/>");
        } /* zvOptions  */

        if (XMS_OK != xmsErrorCreate(&errorBlock)) {
            setError(this_ptr TSRMLS_CC, SAM_CONNECTION_NO_ERROR_BLOCK, SAM_CONNECTION_NO_ERROR_BLOCK_MSG);
            RETURN_FALSE;
        }

        if (XMS_OK != xmsDestCreate(aDestination, &dest, errorBlock)) {
            setError(this_ptr TSRMLS_CC, SAM_CONNECTION_NO_DESTINATION, SAM_CONNECTION_NO_DESTINATION_MSG);
            if (SAM_DEBUG) check(errorBlock);
            SAM_CLEANUP_RECEIVE();
            RETURN_FALSE;
        }

        /* Do we have any selection criteria? (I.e. a correlation id or message id string) */
        if ( (NULL == szCorrelID) && (NULL == szMessageID) ) {
            /* No, so create a consumer without specifying a selector    */
            if (subNameLength == 0)
            {
                if (SAM_DEBUG) zend_printf("<b>SAMXMSConnection->receive()</b> Creating non-durable consumer<br/>");
                rc = xmsSessCreateConsumer(connResource->session, dest, &consumer, errorBlock);
            }
            else
            {
                if (SAM_DEBUG) zend_printf("<b>SAMXMSConnection->receive()</b> Creating durable subscriber<br/>");
                rc = xmsSessCreateDurableSubscriber(connResource->session, dest, aSubscriptionName, &consumer, errorBlock);
            }
        } else {
            /* Yes, so build a selector string accordingly */
            szSelector = SAMXMSBuildSelectorString(szCorrelID, szMessageID);

            if (SAM_DEBUG) zend_printf("<b>SAMXMSConnection->receive()</b> Using selector string - %s<br/>", szSelector);
            if (subNameLength == 0)
            {
                if (SAM_DEBUG) zend_printf("<b>SAMXMSConnection->receive()</b> Creating non-durable consumer with selector<br/>");
                rc = xmsSessCreateConsumerSelector(connResource->session, dest, szSelector, XMSC_CALCULATE_STRING_SIZE, &consumer, errorBlock);
            }
            else
            {
                if (SAM_DEBUG) zend_printf("<b>SAMXMSConnection->receive()</b> Creating durable subscriber with selector<br/>");
                rc = xmsSessCreateDurableSubscriberSelector(connResource->session, dest, aSubscriptionName, szSelector, XMSC_CALCULATE_STRING_SIZE, noLocal, &consumer, errorBlock);
            }

            /* Free any correlId or message id buffers */
            if (NULL != szCorrelID) efree(szCorrelID);
            if (NULL != szMessageID) efree(szMessageID);

            /* Free the selector string */
            if (NULL != szSelector) efree(szSelector);
        }

        if (XMS_OK != rc) {
            setError(this_ptr TSRMLS_CC, SAM_CONNECTION_NO_CONSUMER, SAM_CONNECTION_NO_CONSUMER_MSG);
            if (SAM_DEBUG) check(errorBlock);
            SAM_CLEANUP_RECEIVE();
            RETURN_FALSE;
        }

        if (XMS_OK != xmsMsgConsumerReceiveWithWait(consumer, waitTimeout, &message, errorBlock)) {
            setError(this_ptr TSRMLS_CC, SAM_CONNECTION_RECEIVE_FAILED, SAM_CONNECTION_RECEIVE_FAILED_MSG);
            if (SAM_DEBUG) check(errorBlock);
            SAM_CLEANUP_RECEIVE();
            RETURN_FALSE;
        }

        /* If there was no error but a null message handle was returned then no message was received 
           during the wait interval */
        if (message == XMS_NULL_HANDLE) {
            setError(this_ptr TSRMLS_CC, SAM_CONNECTION_RECEIVE_TIMEDOUT, SAM_CONNECTION_RECEIVE_TIMEDOUT_MSG);
            SAM_CLEANUP_RECEIVE();
            RETURN_FALSE;
        }

            /* get the message type...       */
            if (XMS_OK != xmsMsgGetTypeId(message, &messageType, errorBlock)) {
                setError(this_ptr TSRMLS_CC, SAM_CONNECTION_TYPE_GET_FAILED, SAM_CONNECTION_TYPE_GET_FAILED_MSG);
                if (SAM_DEBUG) check(errorBlock);
                SAM_CLEANUP_RECEIVE();
                RETURN_FALSE;
            }

            if (messageType == XMS_MESSAGE_TYPE_BYTES) {

                /* Allocate a buffer for the message */
                /* -- First determine the length of the message */
                if (XMS_OK != xmsBytesMsgReadBytes(message, (xmsSBYTE *) text, XMSC_QUERY_SIZE, &receivedLength, errorBlock)) {
                    setError(this_ptr TSRMLS_CC, SAM_CONNECTION_READBYTES_FAILED, SAM_CONNECTION_READBYTES_FAILED_MSG);
                    if (SAM_DEBUG) check(errorBlock);
                    SAM_CLEANUP_RECEIVE();
                    RETURN_FALSE;
                }

                /* -- Now allocate the buffer */
                if (NULL == (text = emalloc(receivedLength))){
                    setError(this_ptr TSRMLS_CC, SAM_CONNECTION_MSG_BUFFER_ALLOC_FAILED, SAM_CONNECTION_MSG_BUFFER_ALLOC_FAILED_MSG);
                    if (SAM_DEBUG) check(errorBlock);
                    SAM_CLEANUP_RECEIVE();
                    RETURN_FALSE;
                }

                if (XMS_OK != xmsBytesMsgReadBytes(message, (xmsSBYTE *) text, receivedLength, &receivedLength, errorBlock)) {
                    setError(this_ptr TSRMLS_CC, SAM_CONNECTION_READBYTES_FAILED, SAM_CONNECTION_READBYTES_FAILED_MSG);
                    if (SAM_DEBUG) check(errorBlock);
                    SAM_CLEANUP_RECEIVE();
                    RETURN_FALSE;
                }
            } else if (messageType == XMS_MESSAGE_TYPE_TEXT) {

                /* Allocate a buffer for the message */
                /* -- First determine the length of the message */
                if (XMS_OK != xmsTextMsgGetText(message, text, XMSC_QUERY_SIZE, &receivedLength, errorBlock)) {
                    setError(this_ptr TSRMLS_CC, SAM_CONNECTION_READBYTES_FAILED, SAM_CONNECTION_READBYTES_FAILED_MSG);
                    if (SAM_DEBUG) check(errorBlock);
                    SAM_CLEANUP_RECEIVE();
                    RETURN_FALSE;
                }

                /* -- Now attempt to allocate the buffer */
                if (NULL == (text = emalloc(receivedLength))){
                    setError(this_ptr TSRMLS_CC, SAM_CONNECTION_MSG_BUFFER_ALLOC_FAILED, SAM_CONNECTION_MSG_BUFFER_ALLOC_FAILED_MSG);
                    if (SAM_DEBUG) check(errorBlock);
                    SAM_CLEANUP_RECEIVE();
                    RETURN_FALSE;

                }

                if (XMS_OK != xmsTextMsgGetText(message, text, receivedLength, &receivedLength, errorBlock)) {
                    setError(this_ptr TSRMLS_CC, SAM_CONNECTION_GETTEXT_FAILED, SAM_CONNECTION_GETTEXT_FAILED_MSG);
                    if (SAM_DEBUG) check(errorBlock);
                    SAM_CLEANUP_RECEIVE();
                    RETURN_FALSE;
                }
            }

            if (SAM_DEBUG) zend_printf("<b>SAMXMSConnection->receive()</b> Message of %ld bytes received<br/>", receivedLength);

            /* create a new SAM_Message object to hold the results...    */
            MAKE_STD_ZVAL(msg);
            zend_eval_string("new SAMMessage();", msg, "sam_xms_connection" TSRMLS_CC);

            MAKE_STD_ZVAL(zBody);
            ZVAL_STRINGL(zBody, text, receivedLength, 1);
            add_property_zval(msg, SAM_MESSAGE_BODY, zBody);

            return_value->value.obj = msg->value.obj;
            return_value->type = msg->type;

            if (SAM_DEBUG) zend_printf("<b>SAMXMSConnection->receive()</b> Message received OK. '%.*s'<br/>", receivedLength, text);

            /* Create the SAM message header property...   */
            MAKE_STD_ZVAL(zMsgHdr);
            object_init(zMsgHdr);

            if (add_property_zval(msg, SAM_MESSAGE_HEADER, zMsgHdr) == FAILURE) {
                if (SAM_DEBUG) zend_printf("<b>SAMXMSConnection->receive()</b> Unable to create header property in message!<br/>", receivedLength);
                setError(this_ptr TSRMLS_CC, SAM_CONNECTION_HEADER_CREATE_FAILED, SAM_CONNECTION_HEADER_CREATE_FAILED_MSG);
                SAM_CLEANUP_RECEIVE();
                RETURN_FALSE;
            }

            /* find the message id...                                            */
            if (XMS_OK == xmsMsgGetJMSMessageID(message, msgID, XMSC_QUERY_SIZE, &length, errorBlock)) {
                msgID = emalloc(length);
                if (XMS_OK == xmsMsgGetJMSMessageID(message, msgID, length, NULL, errorBlock)) {
                    MAKE_STD_ZVAL(zVal);
                    ZVAL_STRINGL(zVal, msgID, length, 1);
                    if (add_property_zval(zMsgHdr, SAM_CONSTANT_MESSAGEID, zVal) == FAILURE) {
                        setError(this_ptr TSRMLS_CC, SAM_CONNECTION_MESSAGE_PROPERTY_SET_FAILED, SAM_CONNECTION_MESSAGE_PROPERTY_SET_FAILED_MSG);
                        SAM_CLEANUP_RECEIVE();
                        RETURN_FALSE;
                    }
                }
                if (msgID != NULL)
                {
                    efree(msgID);
                    msgID = NULL;
                }
            }

            /* see what properties we have in the XMS message that we can reflect in the SAM message...    */
            if (XMS_OK != xmsMsgGetProperties(message, &props, errorBlock)) {
                if (SAM_DEBUG) zend_printf("<b>SAMXMSConnection->receive()</b> Unable to retrieve message properties!<br/>", receivedLength);
            } else {
                /* Walk through the properties...   */
                while(XMS_OK == xmsIteratorGetNext(props, (xmsHObj*)&prop, errorBlock)) {

                    if (XMS_OK != xmsPropertyGetName((xmsHProperty) prop, propName, XMSC_QUERY_SIZE, &pnLength, errorBlock)) {
                        setError(this_ptr TSRMLS_CC, SAM_CONNECTION_PROPERTY_GET_NAME_FAILED, SAM_CONNECTION_PROPERTY_GET_NAME_FAILED_MSG);
                        if (SAM_DEBUG) check(errorBlock);
                        SAM_CLEANUP_RECEIVE();
                        RETURN_FALSE;
                    }

                    if (NULL == (propName = emalloc(pnLength))) {
                        setError(this_ptr TSRMLS_CC, SAM_CONNECTION_PROPNAME_ALLOC_FAILED, SAM_CONNECTION_PROPNAME_ALLOC_FAILED_MSG);
                        if (SAM_DEBUG) check(errorBlock);
                        SAM_CLEANUP_RECEIVE();
                        RETURN_FALSE;
                    }

                    if (XMS_OK != xmsPropertyGetName((xmsHProperty) prop, propName, pnLength, &pnLength, errorBlock)) {
                        setError(this_ptr TSRMLS_CC, SAM_CONNECTION_PROPERTY_GET_NAME_FAILED, SAM_CONNECTION_PROPERTY_GET_NAME_FAILED_MSG);
                        if (SAM_DEBUG) check(errorBlock);
                        SAM_CLEANUP_RECEIVE();
                        RETURN_FALSE;
                    }

                    if (XMS_OK != xmsPropertyGetString((xmsHProperty) prop, propValue, XMSC_QUERY_SIZE, &pvLength, errorBlock)) {
                        setError(this_ptr TSRMLS_CC, SAM_CONNECTION_PROPERTY_GET_STRING_FAILED, SAM_CONNECTION_PROPERTY_GET_STRING_FAILED_MSG);
                        if (SAM_DEBUG) check(errorBlock);
                        SAM_CLEANUP_RECEIVE();
                        RETURN_FALSE;
                    }

                    if (NULL == (propValue = emalloc(pvLength))) {
                        setError(this_ptr TSRMLS_CC, SAM_CONNECTION_PROPVAL_ALLOC_FAILED, SAM_CONNECTION_PROPVAL_ALLOC_FAILED_MSG);
                        if (SAM_DEBUG) check(errorBlock);
                        SAM_CLEANUP_RECEIVE();
                        RETURN_FALSE;
                    }

                    if (XMS_OK != xmsPropertyGetString((xmsHProperty) prop, propValue, pvLength, &pvLength, errorBlock)) {
                        setError(this_ptr TSRMLS_CC, SAM_CONNECTION_PROPERTY_GET_STRING_FAILED, SAM_CONNECTION_PROPERTY_GET_STRING_FAILED_MSG);
                        if (SAM_DEBUG) check(errorBlock);
                        SAM_CLEANUP_RECEIVE();
                        RETURN_FALSE;
                    }
                    if (SAM_DEBUG) zend_printf("<b>SAMXMSConnection->receive()</b> Retrieved message property '%s' value=%s<br/>", propName, propValue);

                    MAKE_STD_ZVAL(zVal);
                    ZVAL_STRINGL(zVal, propValue, pvLength, 1);
                    if (add_property_zval(zMsgHdr, propName, zVal) == FAILURE) {
                        setError(this_ptr TSRMLS_CC, SAM_CONNECTION_MESSAGE_PROPERTY_SET_FAILED, SAM_CONNECTION_MESSAGE_PROPERTY_SET_FAILED_MSG);
                        SAM_CLEANUP_RECEIVE();
                        RETURN_FALSE;
                    }

                    if (NULL != propName) efree(propName);
                    if (NULL != propValue) efree(propValue);
                } /*while*/

                /* Finished with the iterator so throw it away...   */
                xmsIteratorDispose(&props, errorBlock);
            }

            /* if we understood then set the message type as a header property...   */
            MAKE_STD_ZVAL(zVal);
            if (messageType == XMS_MESSAGE_TYPE_BYTES) {
                ZVAL_STRING(zVal, SAM_CONSTANT_BYTES, 1);
            } else if (messageType == XMS_MESSAGE_TYPE_TEXT) {
                ZVAL_STRING(zVal, SAM_CONSTANT_TEXT, 1);
            } else {
                ZVAL_STRING(zVal, SAM_CONSTANT_UNKNOWN, 1);
            }
            if (add_property_zval(zMsgHdr, SAM_CONSTANT_TYPE, zVal) == FAILURE) {
                setError(this_ptr TSRMLS_CC, SAM_CONNECTION_MESSAGE_PROPERTY_SET_FAILED, SAM_CONNECTION_MESSAGE_PROPERTY_SET_FAILED_MSG);
                SAM_CLEANUP_RECEIVE();
                RETURN_FALSE;
            }

            /* Free the buffer we allocated for the data retrieved from the message */
            if (NULL != text) efree(text);

            SAM_CLEANUP_RECEIVE();

            if (SAM_DEBUG) zend_printf("<b>SAMXMSConnection->receive()</b> exit<br/>");
            return;
}
/* }}} */


/* {{{ proto PHP_NAMED_FUNCTION(SAM_Connection_remove)
     remove a particular message from a queue.  */

PHP_FUNCTION(SAM_XMS_Connection_remove)
{
    int rc;

    char *destString = NULL;            /* Destination string parsed in via the argument list */
    int  destLength=0;                   /* Length of the destination given in the arg list */

    zval *zvOptions = NULL;

    HashTable *optionsHT;
    zval **zTemp;

    char                    *szCorrelID = NULL;
    char                    *szMessageID = NULL;
    xmsCHAR                 *szSelector = NULL;

    xmsINT                  waitTimeout = 0;

    xmsHErrorBlock          errorBlock;
    xmsHDest                dest = XMS_NULL_HANDLE;
    xmsHMsgConsumer         consumer = XMS_NULL_HANDLE;
    xmsHMsg                 message;
    int                     transacted = xmsFALSE;


    php_sam_xms_connection  *connResource;
    zval                    **zConnection;

    if (SAM_DEBUG) zend_printf("<br/><b>SAMXMSConnection->remove()</b> enter<br/>");

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC,
        "sa", &destString, &destLength, &zvOptions) == FAILURE) {
            setError(this_ptr TSRMLS_CC, SAM_CONNECTION_WRONG_PARAMETERS_REMOVE, SAM_CONNECTION_WRONG_PARAMETERS_REMOVE_MSG);
            WRONG_PARAM_COUNT;
            RETURN_FALSE;
        }

        if (SAM_DEBUG) zend_printf("<b>SAMXMSConnection->remove()</b> Target destination is: %s.<br/>", destString);

        /* Retrieve the connection resource from this object's hash table */
        if (zend_hash_find(Z_OBJPROP_P(this_ptr), SAM_CONNECTION_CONNECTION_INDEX, sizeof(SAM_CONNECTION_CONNECTION_INDEX), (void**)&zConnection) == FAILURE) {
            setError(this_ptr TSRMLS_CC, SAM_CONNECTION_NO_CONNECTION, SAM_CONNECTION_NO_CONNECTION_MSG);
            return;
        }
        ZEND_FETCH_RESOURCE(connResource, php_sam_xms_connection*, zConnection, -1, PHP_SAM_CONNECTION_RES_NAME, le_sam_xms_connection);
        if (SAM_DEBUG) zend_printf("<b>SAMXMSConnection->remove()</b> connection is %d<br/>", connResource->conn);

       /* Check we have a valid XMS session handle */
        if (XMS_NULL_HANDLE == connResource->session) {
            setError(this_ptr TSRMLS_CC, SAM_CONNECTION_NULL_SESSION, SAM_CONNECTION_NULL_SESSION_MSG);
            RETURN_FALSE;
        }

        /* Take a look in the options array if we have one...    */
        if (zvOptions) {
            optionsHT = Z_ARRVAL_P(zvOptions);
            if (zend_hash_num_elements(optionsHT) > 0) {
                optionsHT = HASH_OF(zvOptions);

                /* Has a correlID been specified?      */
                if (zend_hash_find(optionsHT, SAM_CONSTANT_CORRELID, sizeof(SAM_CONSTANT_CORRELID), (void**)&zTemp) == SUCCESS) {
                    convert_to_string(*zTemp);

                    if (NULL == (szCorrelID = emalloc(Z_STRLEN_PP(zTemp)+1))) {
                        setError(this_ptr TSRMLS_CC, SAM_CONNECTION_CORRELID_ALLOC_FAILED, SAM_CONNECTION_CORRELID_ALLOC_FAILED_MSG);
                        RETURN_FALSE;
                    }

                    strncpy(szCorrelID, Z_STRVAL_PP(zTemp), Z_STRLEN_PP(zTemp));
                    szCorrelID[Z_STRLEN_PP(zTemp)]=0;
                    if (SAM_DEBUG) zend_printf("<b>SAMXMSConnection->remove()</b> Caller specified correlID of <b>%s</b><br/>", szCorrelID);
                }

                /* Has a messageID been specified?      */
                if (zend_hash_find(optionsHT, SAM_CONSTANT_MESSAGEID, sizeof(SAM_CONSTANT_MESSAGEID), (void**)&zTemp) == SUCCESS) {
                    convert_to_string(*zTemp);

                    if (NULL == (szMessageID = emalloc(Z_STRLEN_PP(zTemp)+1))) {
                        setError(this_ptr TSRMLS_CC, SAM_CONNECTION_MESSAGEID_ALLOC_FAILED, SAM_CONNECTION_MESSAGEID_ALLOC_FAILED_MSG);
                        RETURN_FALSE;
                    }

                    strncpy(szMessageID, Z_STRVAL_PP(zTemp), Z_STRLEN_PP(zTemp));
                    szMessageID[Z_STRLEN_PP(zTemp)]=0;
                    if (SAM_DEBUG) zend_printf("<b>SAMXMSConnection->remove()</b> Caller specified messageID of <b>%s</b><br/>", szMessageID);
                }

            } /* zend_hash_num_elements(optionsHT) > 0   */
        } else {
            if (SAM_DEBUG) zend_printf("<b>SAMXMSConnection->remove()</b> No options array specified.<br/>");
            RETURN_FALSE;
        } /* zvOptions  */

        if  ( ( (NULL == szCorrelID)  || (0 == strlen(szCorrelID) ) ) &&
            ( (NULL == szMessageID) || (0 == strlen(szMessageID) ) ) ) {
                if (SAM_DEBUG) zend_printf("<b>SAMXMSConnection->remove()</b> No correlation id or message id specified.<br/>");
                RETURN_FALSE;
            }

            if (XMS_OK != xmsErrorCreate(&errorBlock)) {
                setError(this_ptr TSRMLS_CC, SAM_CONNECTION_NO_ERROR_BLOCK, SAM_CONNECTION_NO_ERROR_BLOCK_MSG);
                RETURN_FALSE;
            }

            if (XMS_OK != xmsDestCreate(destString, &dest, errorBlock)) {
                setError(this_ptr TSRMLS_CC, SAM_CONNECTION_NO_DESTINATION, SAM_CONNECTION_NO_DESTINATION_MSG);
                if (SAM_DEBUG) check(errorBlock);
                SAM_CLEANUP_REMOVE();
                RETURN_FALSE;
            }

            /* Build a selector string from the message id or correlid */
            szSelector = SAMXMSBuildSelectorString(szCorrelID, szMessageID);

            /* Free the correlId and messageid buffers */
            if (NULL != szCorrelID) efree(szCorrelID);
            if (NULL != szMessageID) efree(szMessageID);

            if (SAM_DEBUG) zend_printf("<b>SAMXMSConnection->remove()</b> Using selector string - %s<br/>", szSelector);

            if (SAM_DEBUG) zend_printf("<b>SAMXMSConnection->remove()</b> Creating non-durable consumer with selector<br/>");
            rc = xmsSessCreateConsumerSelector(connResource->session, dest, szSelector, XMSC_CALCULATE_STRING_SIZE, &consumer, errorBlock);

            /* Free the selector buffer */
            if (NULL != szSelector) efree(szSelector);

            if (XMS_OK != rc) {
                setError(this_ptr TSRMLS_CC, SAM_CONNECTION_NO_CONSUMER, SAM_CONNECTION_NO_CONSUMER_MSG);
                if (SAM_DEBUG) check(errorBlock);
                SAM_CLEANUP_REMOVE();
                RETURN_FALSE;
            }

            /* Here we just set the waitTimeout to a very low value */
            waitTimeout = 1;

            if (XMS_OK != xmsMsgConsumerReceiveWithWait(consumer, waitTimeout, &message, errorBlock)
                || (message == XMS_NULL_HANDLE)) {
                    setError(this_ptr TSRMLS_CC, SAM_CONNECTION_REMOVE_FAILED, SAM_CONNECTION_REMOVE_FAILED_MSG);
                    if (SAM_DEBUG) check(errorBlock);
                    SAM_CLEANUP_REMOVE();
                    RETURN_FALSE;
                }

                if (SAM_DEBUG) zend_printf("<b>SAMXMSConnection->remove()</b> Message removed<br/>");

                SAM_CLEANUP_REMOVE();

                if (SAM_DEBUG) zend_printf("<b>SAMXMSConnection->remove()</b> exit<br/>");
                RETURN_TRUE;
}
/* }}} */


/* {{{ proto PHP_NAMED_FUNCTION(SAM_Connection_rollback)
     cancel the affects of all operation on the currently opne unit of work.  */

PHP_FUNCTION(SAM_XMS_Connection_rollback)
{
    php_sam_xms_connection  *connResource;
    zval                    **zConnection;
    xmsHErrorBlock          errorBlock;

    if (SAM_DEBUG) zend_printf("<br/><b>SAMXMSConnection->rollback()</b> enter<br/>");

    /* Retrieve the connection resource from this object's hash table */
    if (zend_hash_find(Z_OBJPROP_P(this_ptr), SAM_CONNECTION_CONNECTION_INDEX, sizeof(SAM_CONNECTION_CONNECTION_INDEX), (void**)&zConnection) == FAILURE) {
        setError(this_ptr TSRMLS_CC, SAM_CONNECTION_NO_CONNECTION, SAM_CONNECTION_NO_CONNECTION_MSG);
        return;
    }
    ZEND_FETCH_RESOURCE(connResource, php_sam_xms_connection*, zConnection, -1, PHP_SAM_CONNECTION_RES_NAME, le_sam_xms_connection);
    if (SAM_DEBUG) zend_printf("<b>SAMXMSConnection->rollback()</b> connection is %d<br/>", connResource->conn);

    /* Check we have a valid XMS session handle */
    if (XMS_NULL_HANDLE == connResource->session) {
        setError(this_ptr TSRMLS_CC, SAM_CONNECTION_NULL_SESSION, SAM_CONNECTION_NULL_SESSION_MSG);
        RETURN_FALSE;
    } 

    /* Create an XMS error block that we will need to issue a rollback */
    if (XMS_OK != xmsErrorCreate(&errorBlock)) {
       setError(this_ptr TSRMLS_CC, SAM_CONNECTION_NO_ERROR_BLOCK, SAM_CONNECTION_NO_ERROR_BLOCK_MSG);
       RETURN_FALSE;
    }

    /* Issue the XMS rollback */
    if (XMS_OK != xmsSessRollback(connResource->session, errorBlock)) {
        setError(this_ptr TSRMLS_CC, SAM_CONNECTION_ROLLBACK_FAILED, SAM_CONNECTION_ROLLBACK_FAILED_MSG);
        RETURN_FALSE;
    }

    /* Finished with the error block */
    xmsErrorDispose(&errorBlock);

    if (SAM_DEBUG) zend_printf("<b>SAMXMSConnection->rollback()</b> exit<br/>");
    RETURN_TRUE;
}
/* }}} */


/* {{{ proto PHP_NAMED_FUNCTION(SAM_Connection_send)
     send a message to a queue.  */
PHP_FUNCTION(SAM_XMS_Connection_send)
{
    char *aDestination=NULL;
    int destLength=0;
    zval *msgParm;
    zval *zvOptions = NULL;
    HashTable *optionsHT;
    zval **zTemp;

    char *szCorrelID = NULL;
    char szDeliveryMode[512] = {'\0'};
    char *szHeaders = NULL;   /* Used to store any target client option (which must be "mq" or "jms") */
    xmsINT priority = -1;
    xmsLONG timeToLive = -1;

    zend_class_entry *objce;
    xmsHDest dest;
    xmsHDest replyDest;
    xmsHErrorBlock errorBlock;
    xmsHMsgProducer producer=NULL;
    xmsHMsg message;
    int transacted = xmsFALSE;
    php_sam_xms_connection *connResource;
    zval **zConnection;
    zval **zMsg;
    xmsINT length;
    xmsCHAR* msgID;
    zval *zCorrelID;
    zval **zHeader = NULL;
    zval **data;
    HashTable *header_hash;
    HashPosition pointer;
    HashPosition pointer2;
    zval **zVal;
    zval **zType;
    float fVal;

    char *key;
    int key_len;
    long index;

    int msgType = SAM_MSGTYPE_BYTES;

    if (SAM_DEBUG) zend_printf("<br/><b>SAMXMSConnection->send()</b> enter<br/>");

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC,
        "so|a", &aDestination, &destLength, &msgParm, &zvOptions) == FAILURE) {
            setError(this_ptr TSRMLS_CC, SAM_CONNECTION_WRONG_PARAMETERS_SEND, SAM_CONNECTION_WRONG_PARAMETERS_SEND_MSG);
            WRONG_PARAM_COUNT;
            RETURN_FALSE;
        }

        if (SAM_DEBUG) zend_printf("<b>SAMXMSConnection->send()</b> Target destination is: %s<br/>", aDestination);

        /* make sure the message parameter is actually a SAM_Message object...   */
        objce = Z_OBJCE_P(msgParm);
//        if (objce != php_sam_SAM_Message_entry) {
//            setError(this_ptr TSRMLS_CC, SAM_CONNECTION_INVALID_MESSAGE, SAM_CONNECTION_INVALID_MESSAGE_MSG);
//            RETURN_FALSE;
 //       }

        /* Retrieve the message body from the SAM_Message object...   */
        if (zend_hash_find(Z_OBJPROP_P(msgParm), SAM_MESSAGE_BODY, sizeof(SAM_MESSAGE_BODY), (void**)&zMsg) == FAILURE) {
            setError(this_ptr TSRMLS_CC, SAM_CONNECTION_NO_MSG_BODY, SAM_CONNECTION_NO_MSG_BODY_MSG);
            RETURN_FALSE;
        } else {
            switch (Z_TYPE_PP(zMsg)) {
            case IS_STRING:
                if (SAM_DEBUG) zend_printf("<b>SAMXMSConnection->send()</b> Message body is a string: %s<br/>", Z_STRVAL_PP(zMsg));
                break;
            default:
                if (SAM_DEBUG) zend_printf("<b>SAMXMSConnection->send()</b> Message body is of type %d<br/>", Z_TYPE_PP(zMsg));
                convert_to_string(*zMsg);
                break;
            }
        }

        /* Take a look in the options array if we have one...    */
        if (zvOptions) {
            optionsHT = Z_ARRVAL_P(zvOptions);
            if (zend_hash_num_elements(optionsHT) > 0) {
                optionsHT = HASH_OF(zvOptions);

                /* Has a correlID been specified?      */
                if (zend_hash_find(optionsHT, SAM_CONSTANT_CORRELID, sizeof(SAM_CONSTANT_CORRELID), (void**)&zTemp) == SUCCESS) {
                    convert_to_string(*zTemp);

                    if (NULL == (szCorrelID = emalloc(Z_STRLEN_PP(zTemp)+1))) {
                        setError(this_ptr TSRMLS_CC, SAM_CONNECTION_CORRELID_ALLOC_FAILED, SAM_CONNECTION_CORRELID_ALLOC_FAILED_MSG);
                        RETURN_FALSE;
                    }

                    strncpy(szCorrelID, Z_STRVAL_PP(zTemp), Z_STRLEN_PP(zTemp));
                    szCorrelID[Z_STRLEN_PP(zTemp)]=0;
                    if (SAM_DEBUG) zend_printf("<b>SAMXMSConnection->send()</b> Caller specified correlID of <b>%s</b><br/>", szCorrelID);
                }

                /* Has a deliveryMode been specified? */
                if (zend_hash_find(optionsHT, SAM_CONSTANT_DELIVERYMODE, sizeof(SAM_CONSTANT_DELIVERYMODE), (void**)&zTemp) == SUCCESS) {
                    convert_to_string(*zTemp);
                    strncpy(szDeliveryMode, Z_STRVAL_PP(zTemp), Z_STRLEN_PP(zTemp));
                    if (SAM_DEBUG) zend_printf("<b>SAMXMSConnection->send()</b> Caller specified deliveryMode of <b>%s</b><br/>", szDeliveryMode);
                    if ( (0 != strcmp(szDeliveryMode, SAM_CONSTANT_NONPERSISTENT)) &&
                        (0 != strcmp(szDeliveryMode, SAM_CONSTANT_PERSISTENT)) )
                    {
                        setError(this_ptr TSRMLS_CC, SAM_CONNECTION_ILLEGAL_DELIVERY_MODE, SAM_CONNECTION_ILLEGAL_DELIVERY_MODE_MSG);
                        RETURN_FALSE;
                    }
                }

                /* Has a priority been specified? */
                if (zend_hash_find(optionsHT, SAM_CONSTANT_PRIORITY, sizeof(SAM_CONSTANT_PRIORITY), (void**)&zTemp) == SUCCESS) {
                    convert_to_long(*zTemp);
                    priority = Z_LVAL_PP(zTemp);

                    if (SAM_DEBUG) zend_printf("<b>SAMXMSConnection->send()</b> Caller specified priority of <b>%d</b><br/>", priority);
                    if ((priority < 0 || priority > 9))
                    {
                        setError(this_ptr TSRMLS_CC, SAM_CONNECTION_ILLEGAL_PRIORITY, SAM_CONNECTION_ILLEGAL_PRIORITY_MSG);
                        RETURN_FALSE;
                    }
                }

                /* Has a time to live been specified? */
                if (zend_hash_find(optionsHT, SAM_CONSTANT_TIMETOLIVE, sizeof(SAM_CONSTANT_TIMETOLIVE), (void**)&zTemp) == SUCCESS) {
                    convert_to_long(*zTemp);
                    timeToLive = Z_LVAL_PP(zTemp);

                    if (SAM_DEBUG) zend_printf("<b>SAMXMSConnection->send()</b> Caller specified time to live of <b>%d</b><br/>", timeToLive);
                    if (timeToLive < 0)
                    {
                        setError(this_ptr TSRMLS_CC, SAM_CONNECTION_ILLEGAL_TIMETOLIVE, SAM_CONNECTION_ILLEGAL_TIMETOLIVE_MSG);
                        RETURN_FALSE;
                    }
                }

                /* Was the SAM_HEADERS option specified */
                if (zend_hash_find(optionsHT, SAM_CONSTANT_WMQ_TARGET_CLIENT, sizeof(SAM_CONSTANT_WMQ_TARGET_CLIENT), (void**)&zTemp) == SUCCESS) {

                    if (NULL == (szHeaders = emalloc(Z_STRLEN_PP(zTemp)+1))) {
                        setError(this_ptr TSRMLS_CC, SAM_CONNECTION_HEADERS_ALLOC_FAILED, SAM_CONNECTION_HEADERS_ALLOC_FAILED_MSG);
                        RETURN_FALSE;
                    }

                    strncpy(szHeaders, Z_STRVAL_PP(zTemp), Z_STRLEN_PP(zTemp));
                    szHeaders[Z_STRLEN_PP(zTemp)]=0;
                    if (SAM_DEBUG) zend_printf("<b>SAMXMSConnection->send()</b> Caller specified headers of <b>%s</b><br/>", szHeaders);

                    /* This option should be set to jms or none */
                    if ( (0 != strcmp(szHeaders, SAM_CONSTANT_WMQTARGETCLIENT_JMS)) &&
                        (0 != strcmp(szHeaders, SAM_CONSTANT_WMQTARGETCLIENT_MQ)) )
                    {
                        setError(this_ptr TSRMLS_CC, SAM_CONNECTION_ILLEGAL_TARGETCLIENT, SAM_CONNECTION_ILLEGAL_TARGETCLIENT_MSG);
                        RETURN_FALSE;
                    }
                }

            } /* zend_hash_num_elements(optionsHT) > 0 */
        } else {
            if (SAM_DEBUG) zend_printf("<b>SAMXMSConnection->send()</b> No options array specified.<br/>");
        } /* zvOptions */

        /* Retrieve the connection resource from this object's hash table */
        if (zend_hash_find(Z_OBJPROP_P(this_ptr), SAM_CONNECTION_CONNECTION_INDEX, sizeof(SAM_CONNECTION_CONNECTION_INDEX), (void**)&zConnection) == FAILURE) {
            setError(this_ptr TSRMLS_CC, SAM_CONNECTION_NO_CONNECTION, SAM_CONNECTION_NO_CONNECTION_MSG);
            RETURN_FALSE;
        }
        ZEND_FETCH_RESOURCE(connResource, php_sam_xms_connection*, zConnection, -1, PHP_SAM_CONNECTION_RES_NAME, le_sam_xms_connection);
        if (SAM_DEBUG) zend_printf("<b>SAMXMSConnection->send()</b> connection is %d<br/>", connResource->conn);

       /* Check we have a valid XMS session handle */
        if (XMS_NULL_HANDLE == connResource->session) {
            setError(this_ptr TSRMLS_CC, SAM_CONNECTION_NULL_SESSION, SAM_CONNECTION_NULL_SESSION_MSG);
            RETURN_FALSE;
        }

        /* The SAM_HEADERS option is only supported with MQ */
        if ((NULL != szHeaders) && (0 != strcmp(SAM_CONSTANT_WMQ, connResource->protocol))) {
            setError(this_ptr TSRMLS_CC, SAM_CONNECTION_ILLEGAL_TARGETCLIENT_MODE, SAM_CONNECTION_ILLEGAL_TARGETCLIENT_MODE_MSG);
            RETURN_FALSE;
        }

        /* create an XMS error block to capture error info...   */
        if (XMS_OK != xmsErrorCreate(&errorBlock)) {
            setError(this_ptr TSRMLS_CC, SAM_CONNECTION_NO_ERROR_BLOCK, SAM_CONNECTION_NO_ERROR_BLOCK_MSG);
            RETURN_FALSE;
        }

        /* Create a destination that represents where we are trying to send the message...   */
        if (XMS_OK != xmsDestCreate(aDestination, &dest, errorBlock)) {
            setError(this_ptr TSRMLS_CC, SAM_CONNECTION_NO_DESTINATION, SAM_CONNECTION_NO_DESTINATION_MSG);
            if (SAM_DEBUG) check(errorBlock);
            SAM_CLEANUP_SEND();
            RETURN_FALSE;
        }

        /* If we are not setting up the RFH2 for MQ then set the XMSC_WMQ_TARGET_CLIENT property to XMSC_WMQ_TARGET_DEST_MQ */
        if ( (NULL != szHeaders) && (0 == strcmp(szHeaders, SAM_CONSTANT_WMQTARGETCLIENT_MQ))) {
            if (XMS_OK != xmsSetIntProperty( (xmsHObj) dest, XMSC_WMQ_TARGET_CLIENT, XMSC_WMQ_TARGET_DEST_MQ, errorBlock)) {
                if (SAM_DEBUG) zend_printf("<br/><b>SAMXMSConnection->send()</b> xmsSetIntProperty (XMSC_WMQ_TARGET_CLIENT) failed!<br/>");
                setError(this_ptr TSRMLS_CC, SAM_CONNECTION_SET_TARGET_CLIENT, SAM_CONNECTION_SET_TARGET_CLIENT_MSG);
                SAM_CLEANUP_SEND();
                RETURN_FALSE;
            }
            efree(szHeaders);
        }

        /* Create an XMS producer with which to do our send...   */
        if (XMS_OK != xmsSessCreateProducer(connResource->session, dest, &producer, errorBlock)) {
            setError(this_ptr TSRMLS_CC, SAM_CONNECTION_NO_PRODUCER, SAM_CONNECTION_NO_PRODUCER_MSG);
            if (SAM_DEBUG) check(errorBlock);
            SAM_CLEANUP_SEND();
            RETURN_FALSE;
        }

        /* Set the deliveryMode if one was specified */
        if (NULL != szDeliveryMode && strlen(szDeliveryMode) > 0) {
            /* Non persistent delivery mode was requested */
            if (0 == strcmp(szDeliveryMode, SAM_CONSTANT_NONPERSISTENT)) {
                if (XMS_OK != xmsMsgProducerSetDeliveryMode(producer, XMSC_DELIVERY_NOT_PERSISTENT, errorBlock)) {
                    setError(this_ptr TSRMLS_CC, SAM_CONNECTION_SET_DELIVERY_MODE, SAM_CONNECTION_SET_DELIVERY_MODE_MSG);
                    if (SAM_DEBUG) check(errorBlock);
                    SAM_CLEANUP_SEND();
                    RETURN_FALSE;
                }
            }

            /* Persistent delivery mode was requested */
            if (0 == strcmp(szDeliveryMode, SAM_CONSTANT_PERSISTENT)) {
                if (XMS_OK != xmsMsgProducerSetDeliveryMode(producer, XMSC_DELIVERY_PERSISTENT, errorBlock)) {
                    setError(this_ptr TSRMLS_CC, SAM_CONNECTION_SET_DELIVERY_MODE, SAM_CONNECTION_SET_DELIVERY_MODE_MSG);
                    if (SAM_DEBUG) check(errorBlock);
                    SAM_CLEANUP_SEND();
                    RETURN_FALSE;
                }
            }
        }

        /* Set the priority on the message producer if a priority setting was specified */
        if (priority != -1) {
            if (XMS_OK != xmsMsgProducerSetPriority(producer, priority, errorBlock)) {
                setError(this_ptr TSRMLS_CC, SAM_CONNECTION_SET_PRIORITY, SAM_CONNECTION_SET_PRIORITY_MSG);
                if (SAM_DEBUG) check(errorBlock);
                SAM_CLEANUP_SEND();
                RETURN_FALSE;
            }
        }

        /* Set the time to live on the message producer if a time to live setting was specified */
        if (timeToLive != -1) {
            if (XMS_OK != xmsMsgProducerSetTimeToLive(producer, timeToLive, errorBlock)) {
                setError(this_ptr TSRMLS_CC, SAM_CONNECTION_SET_TIMETOLIVE, SAM_CONNECTION_SET_TIMETOLIVE_MSG);
                if (SAM_DEBUG) check(errorBlock);
                SAM_CLEANUP_SEND();
                RETURN_FALSE;
            }
        }


        /* See if we have any header properties...   */
        if (zend_hash_find(Z_OBJPROP_P(msgParm), SAM_MESSAGE_HEADER, sizeof(SAM_MESSAGE_HEADER), (void**)&zHeader) != FAILURE) {
            if (SAM_DEBUG) zend_printf("<b>SAMXMSConnection->send()</b> We have a header property<br/>");
        } else {
            if (SAM_DEBUG) zend_printf("<b>SAMXMSConnection->send()</b> No header properties<br/>");
        }

        /* Do we have any header properties to reflect into the message? Here we are looking for msgType only before
        we create the actual message
        */
        if (zHeader) {
            /* Do we have a message type specified?                  */
            if (zend_hash_find(Z_OBJPROP_PP(zHeader), SAM_CONSTANT_TYPE, sizeof(SAM_CONSTANT_TYPE), (void**)&data) != FAILURE) {
                /* Is the message type specified something we understand?     */
                if (SAM_DEBUG) zend_printf("<b>SAMXMSConnection->send()</b> TYPE =%s<br/>", Z_STRVAL_PP(data));
                if ( 0 == strncmp(Z_STRVAL_PP(data), SAM_CONSTANT_TEXT, Z_STRLEN_PP(data))) {
                    msgType = SAM_MSGTYPE_TEXT;
                    if (SAM_DEBUG) zend_printf("<b>SAMXMSConnection->send()</b> msgType=%s<br/>", SAM_CONSTANT_TEXT);
                } else if (0 == strncmp(Z_STRVAL_PP(data), SAM_CONSTANT_BYTES, Z_STRLEN_PP(data))) {
                    msgType = SAM_MSGTYPE_BYTES;
                    if (SAM_DEBUG) zend_printf("<b>SAMXMSConnection->send()</b> msgType=%s<br/>", SAM_CONSTANT_BYTES);
                } else {
                    setError(this_ptr TSRMLS_CC, SAM_CONNECTION_MSG_TYPE_INVALID, SAM_CONNECTION_MSG_TYPE_INVALID_MSG);
                    SAM_CLEANUP_SEND();
                    RETURN_FALSE;
                }
            } else {
                if (SAM_DEBUG) zend_printf("<b>SAMXMSConnection->send()</b> no TYPE specified<br/>");
            }
        }

        if (SAM_MSGTYPE_BYTES == msgType) {
            /* Bytes message format - create an XMS bytes message object that can take any arbitrary data */
            if (XMS_OK != xmsSessCreateBytesMessage(connResource->session, &message, errorBlock)) {
                setError(this_ptr TSRMLS_CC, SAM_CONNECTION_NO_BYTES_MESSAGE, SAM_CONNECTION_NO_BYTES_MESSAGE_MSG);
                if (SAM_DEBUG) check(errorBlock);
                SAM_CLEANUP_SEND();
                RETURN_FALSE;
            }
        } else {
            /* Text message format - create an XMS text message */
            if (XMS_OK != xmsSessCreateTextMessage(connResource->session, &message, errorBlock)) {
                setError(this_ptr TSRMLS_CC, SAM_CONNECTION_NO_TEXT_MESSAGE, SAM_CONNECTION_NO_TEXT_MESSAGE_MSG);
                if (SAM_DEBUG) check(errorBlock);
                SAM_CLEANUP_SEND();
                RETURN_FALSE;
            }

        }

        /* Scan the rest of the header properties to reflect into the message  */
        if (zHeader) {
            /* walk the header properties and do something with them...      */
            header_hash = Z_ARRVAL_PP(zHeader);
            for(zend_hash_internal_pointer_reset_ex(Z_OBJPROP_PP(zHeader), &pointer);
                zend_hash_get_current_data_ex(Z_OBJPROP_PP(zHeader), (void**) &data, &pointer) == SUCCESS;
                zend_hash_move_forward_ex(Z_OBJPROP_PP(zHeader), &pointer)) {
                    zend_hash_get_current_key_ex(Z_OBJPROP_PP(zHeader), &key, &key_len, &index, 0, &pointer);
                    /* compare the key to magic SAM constants for the things we understand...   */
                    if (strncmp(key, SAM_CONSTANT_REPLY_TO, key_len) == 0) {
                        /* identity of the queue on which any response should be put...    */
                        if (SAM_DEBUG) zend_printf("<b>SAMXMSConnection->send()</b> REPLY_TO =%s<br/>", Z_STRVAL_PP(data));
                        /* Create a destination that represents where we the responses will arrive...   */
                        if (XMS_OK != xmsDestCreate(Z_STRVAL_PP(data), &replyDest, errorBlock)) {
                            setError(this_ptr TSRMLS_CC, SAM_CONNECTION_NO_REPLY_DESTINATION, SAM_CONNECTION_NO_REPLY_DESTINATION_MSG);
                            if (SAM_DEBUG) check(errorBlock);
                            SAM_CLEANUP_SEND();
                            RETURN_FALSE;
                        }
                        if (XMS_OK != xmsMsgSetJMSReplyTo(message, replyDest, errorBlock)) {
                            setError(this_ptr TSRMLS_CC, SAM_CONNECTION_REPLY_TO_SET_FAILED, SAM_CONNECTION_REPLY_TO_SET_FAILED_MSG);
                            if (SAM_DEBUG) check(errorBlock);
                            SAM_CLEANUP_SEND();
                            RETURN_FALSE;
                        }
                    } else if (strncmp(key, SAM_CONSTANT_TYPE, key_len) == 0) {
                        /* Ignore type as we have already dealt with it...               */
                    } else {
                        if (SAM_DEBUG) zend_printf("<b>SAMXMSConnection->send()</b> property key =%s<br/>", key);
                        /* Doesn't look like anything we recognise so set it as a user property on the message...  */
                        /* Is the data an array of (value, type) ?   */
                        if (Z_TYPE_PP(data) == IS_ARRAY) {
                            /* we have an array, does it have 2 entries?    */
                            if (zend_hash_num_elements(Z_ARRVAL_PP(data)) == 2) {
                                /* we got 2 elements so assume the 1st is the value and the 2nd the type...  */
                                zend_hash_internal_pointer_reset_ex(Z_ARRVAL_PP(data), &pointer2);
                                zend_hash_get_current_data_ex(Z_ARRVAL_PP(data), (void**) &zVal, &pointer2);
                                zend_hash_move_forward_ex(Z_ARRVAL_PP(data), &pointer2);
                                zend_hash_get_current_data_ex(Z_ARRVAL_PP(data), (void**) &zType, &pointer2);
                                if (Z_TYPE_PP(zType) != IS_STRING) {
                                    if (SAM_DEBUG) zend_printf("<b>SAMXMSConnection->send()</b> property key =%s value specified via array but type is not a string!<br/>", key);
                                    /* if the type is not a string then we have an error!  */
                                    setError(this_ptr TSRMLS_CC, SAM_CONNECTION_PROPERTY_ARRAY_INVALID, SAM_CONNECTION_PROPERTY_ARRAY_INVALID_MSG);
                                    if (SAM_DEBUG) check(errorBlock);
                                    SAM_CLEANUP_SEND();
                                    RETURN_FALSE;
                                } else {
                                    if (strncmp(SAM_CONSTANT_STRING, Z_STRVAL_PP(zType), Z_STRLEN_PP(zType)) == 0) {
                                        /* setting as a string property...   */
                                        convert_to_string(*zVal);
                                        if (XMS_OK != xmsSetStringProperty((xmsHObj)message, key, Z_STRVAL_PP(zVal), Z_STRLEN_PP(zVal), errorBlock)) {
                                            setError(this_ptr TSRMLS_CC, SAM_CONNECTION_SET_PROPERTY_FAILED, SAM_CONNECTION_SET_PROPERTY_FAILED_MSG);
                                            if (SAM_DEBUG) check(errorBlock);
                                            SAM_CLEANUP_SEND();
                                            RETURN_FALSE;
                                        }
                                    } else if (strncmp(SAM_CONSTANT_BOOLEAN, Z_STRVAL_PP(zType), Z_STRLEN_PP(zType)) == 0) {
                                        /* setting as a boolean property...   */
                                        convert_to_boolean(*zVal);
                                        if (XMS_OK != xmsSetBooleanProperty((xmsHObj)message, key, Z_BVAL_PP(zVal), errorBlock)) {
                                            setError(this_ptr TSRMLS_CC, SAM_CONNECTION_SET_PROPERTY_FAILED, SAM_CONNECTION_SET_PROPERTY_FAILED_MSG);
                                            if (SAM_DEBUG) check(errorBlock);
                                            SAM_CLEANUP_SEND();
                                            RETURN_FALSE;
                                        }
                                    } else if (strncmp(SAM_CONSTANT_INTEGER, Z_STRVAL_PP(zType), Z_STRLEN_PP(zType)) == 0) {
                                        /* setting as a integer property...   */
                                        convert_to_long(*zVal);
                                        if (XMS_OK != xmsSetIntProperty((xmsHObj)message, key, Z_LVAL_PP(zVal), errorBlock)) {
                                            setError(this_ptr TSRMLS_CC, SAM_CONNECTION_SET_PROPERTY_FAILED, SAM_CONNECTION_SET_PROPERTY_FAILED_MSG);
                                            if (SAM_DEBUG) check(errorBlock);
                                            SAM_CLEANUP_SEND();
                                            RETURN_FALSE;
                                        }
                                    } else if (strncmp(SAM_CONSTANT_LONG, Z_STRVAL_PP(zType), Z_STRLEN_PP(zType)) == 0) {
                                        /* setting as a long property...   */
                                        convert_to_long(*zVal);
                                        if (XMS_OK != xmsSetLongProperty((xmsHObj)message, key, Z_LVAL_PP(zVal), errorBlock)) {
                                            setError(this_ptr TSRMLS_CC, SAM_CONNECTION_SET_PROPERTY_FAILED, SAM_CONNECTION_SET_PROPERTY_FAILED_MSG);
                                            if (SAM_DEBUG) check(errorBlock);
                                            SAM_CLEANUP_SEND();
                                            RETURN_FALSE;
                                        }
                                    } else if (strncmp(SAM_CONSTANT_BYTE, Z_STRVAL_PP(zType), Z_STRLEN_PP(zType)) == 0) {
                                        /* setting as a byte property...   */
                                        /* assume(!) the Zend conversion will magically work...  */
                                        /* -?-?- What about float, bool and long string values? -?-?-  */
                                        convert_to_long(*zVal);
                                        if (XMS_OK != xmsSetByteProperty((xmsHObj)message, key, Z_LVAL_PP(zVal), errorBlock)) {
                                            setError(this_ptr TSRMLS_CC, SAM_CONNECTION_SET_PROPERTY_FAILED, SAM_CONNECTION_SET_PROPERTY_FAILED_MSG);
                                            if (SAM_DEBUG) check(errorBlock);
                                            SAM_CLEANUP_SEND();
                                            RETURN_FALSE;
                                        }
                                    } else if (strncmp(SAM_CONSTANT_FLOAT, Z_STRVAL_PP(zType), Z_STRLEN_PP(zType)) == 0) {
                                        /* setting as a float property...   */
                                        /* assume(!) the Zend conversion will magically work...  */
                                        convert_to_double(*zVal);
                                        fVal = Z_DVAL_PP(zVal);
                                        if (XMS_OK != xmsSetFloatProperty((xmsHObj)message, key, fVal, errorBlock)) {
                                            setError(this_ptr TSRMLS_CC, SAM_CONNECTION_SET_PROPERTY_FAILED, SAM_CONNECTION_SET_PROPERTY_FAILED_MSG);
                                            if (SAM_DEBUG) check(errorBlock);
                                            SAM_CLEANUP_SEND();
                                            RETURN_FALSE;
                                        }
                                    } else if (strncmp(SAM_CONSTANT_DOUBLE, Z_STRVAL_PP(zType), Z_STRLEN_PP(zType)) == 0) {
                                        /* setting as a float property...   */
                                        /* assume(!) the Zend conversion will magically work...  */
                                        convert_to_double(*zVal);
                                        if (XMS_OK != xmsSetDoubleProperty((xmsHObj)message, key, Z_DVAL_PP(zVal), errorBlock)) {
                                            setError(this_ptr TSRMLS_CC, SAM_CONNECTION_SET_PROPERTY_FAILED, SAM_CONNECTION_SET_PROPERTY_FAILED_MSG);
                                            if (SAM_DEBUG) check(errorBlock);
                                            SAM_CLEANUP_SEND();
                                            RETURN_FALSE;
                                        }
                                    }
                                }
                            } else {
                                /* No so we have an error...    */
                                setError(this_ptr TSRMLS_CC, SAM_CONNECTION_PROPERTY_ARRAY_INVALID, SAM_CONNECTION_PROPERTY_ARRAY_INVALID_MSG);
                                if (SAM_DEBUG) check(errorBlock);
                                SAM_CLEANUP_SEND();
                                RETURN_FALSE;
                            }
                        } else {
                            /* no type info specified so we assume we need to convert the data to string format...   */
                            convert_to_string(*data);
                            if (XMS_OK != xmsSetStringProperty((xmsHObj)message, key, Z_STRVAL_PP(data), Z_STRLEN_PP(data), errorBlock)) {
                                setError(this_ptr TSRMLS_CC, SAM_CONNECTION_SET_PROPERTY_FAILED, SAM_CONNECTION_SET_PROPERTY_FAILED_MSG);
                                if (SAM_DEBUG) check(errorBlock);
                                SAM_CLEANUP_SEND();
                                RETURN_FALSE;
                            }
                        }
                    }
                }
        }

        /* Move the message body into the message...   */
        if (SAM_MSGTYPE_BYTES == msgType) {
            /* In this case we write the message as raw bytes with no character conversion */
            if (XMS_OK != xmsBytesMsgWriteBytes(message, (xmsSBYTE *) Z_STRVAL_PP(zMsg), Z_STRLEN_PP(zMsg), errorBlock)) {
                setError(this_ptr TSRMLS_CC, SAM_CONNECTION_WRITE_BYTES_FAILED, SAM_CONNECTION_WRITE_BYTES_FAILED_MSG);
                if (SAM_DEBUG) check(errorBlock);
                SAM_CLEANUP_SEND();
                RETURN_FALSE;
            }
        } else {
            if (XMS_OK != xmsTextMsgSetText(message, Z_STRVAL_PP(zMsg), Z_STRLEN_PP(zMsg), errorBlock)) {
                setError(this_ptr TSRMLS_CC, SAM_CONNECTION_WRITE_TEXT_FAILED, SAM_CONNECTION_WRITE_TEXT_FAILED_MSG);
                if (SAM_DEBUG) check(errorBlock);
                SAM_CLEANUP_SEND();
                RETURN_FALSE;
            }
        }

        /* Do we have a correlation ID to set?       */
        if ((NULL != szCorrelID) && (strlen(szCorrelID) > 0)) {
            if (XMS_OK != xmsMsgSetJMSCorrelationID(message, szCorrelID, strlen(szCorrelID), errorBlock)) {
                setError(this_ptr TSRMLS_CC, SAM_CONNECTION_SET_CORRELID_FAILED, SAM_CONNECTION_SET_CORRELID_FAILED_MSG);
                if (SAM_DEBUG) check(errorBlock);
                SAM_CLEANUP_SEND();
                RETURN_FALSE;
            }
        }

        /* Finally we can try and send the message...   */
        if (XMS_OK != xmsMsgProducerSend(producer, message, errorBlock)) {
            setError(this_ptr TSRMLS_CC, SAM_CONNECTION_SEND_FAILED, SAM_CONNECTION_SEND_FAILED_MSG);
            if (SAM_DEBUG) check(errorBlock);
            SAM_CLEANUP_SEND();
            RETURN_FALSE; 
        }

        /* Free the correlId buffer */
        if (NULL != szCorrelID) efree(szCorrelID);

        /* retrieve the id of the message so we can use this as a correlation id      */
        if (XMS_OK != xmsMsgGetJMSMessageID(message, NULL, XMSC_QUERY_SIZE, &length, errorBlock)) {
            setError(this_ptr TSRMLS_CC, SAM_CONNECTION_NO_MSG_ID, SAM_CONNECTION_NO_MSG_ID_MSG);
            if (SAM_DEBUG) check(errorBlock);
            SAM_CLEANUP_SEND();
            RETURN_FALSE;
        }

        if (length == 0) {
            setError(this_ptr TSRMLS_CC, SAM_CONNECTION_NO_MSG_ID, SAM_CONNECTION_NO_MSG_ID_MSG);
            if (SAM_DEBUG) check(errorBlock);
            SAM_CLEANUP_SEND();
            RETURN_FALSE;
        }

        msgID = emalloc(length);
        if (XMS_OK != xmsMsgGetJMSMessageID(message, msgID, length, &length, errorBlock)) {
            setError(this_ptr TSRMLS_CC, SAM_CONNECTION_NO_MSG_ID, SAM_CONNECTION_NO_MSG_ID_MSG);
            if (SAM_DEBUG) check(errorBlock);
            SAM_CLEANUP_SEND();
            RETURN_FALSE;
        }
        msgID[length] = '\0';

        if (SAM_DEBUG) zend_printf("<b>SAMXMSConnection->send()</b> message ID is %s<br/>", msgID);

        /* create the returned correlation id string from the message id   */
        MAKE_STD_ZVAL(zCorrelID);
        ZVAL_STRINGL(zCorrelID, msgID, length, 1);
        efree(msgID);

        /* push the correlation ID string value into the return value     */
        return_value->value.obj = zCorrelID->value.obj;
        return_value->type = zCorrelID->type;

        /* clean up the XMS structures we have finished with...    */
        SAM_CLEANUP_SEND();

        if (SAM_DEBUG) zend_printf("<b>SAMXMSConnection->send()</b> exit<br/>");
        return;
}
/* }}} */


/* {{{ proto PHP_NAMED_FUNCTION(SAM_Connection_setDebug)
     check if a connection is connected to a messaging server.  */
PHP_FUNCTION(SAM_XMS_Connection_setDebug) {

    zval *zvOption = NULL;

    if (SAM_DEBUG) zend_printf("<br/><b>SAMXMSConnection->setDebug()</b> enter<br/>");

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC,
        "b", &zvOption) == FAILURE) {
            return;
    }

    if (SAM_DEBUG) zend_printf("<b>SAMXMSConnection->setDebug()</b> exit<br/>");
    RETURN_FALSE;
}
/* }}} */


/* {{{ proto PHP_NAMED_FUNCTION(SAM_Connection_subscription)
     Create a subscription for a topic.  */

PHP_FUNCTION(SAM_XMS_Connection_subscribe)
{
    int rc;

    char *aDestination=NULL;
    int destLength=0;
    zval *zvOptions = NULL;

    xmsHErrorBlock          errorBlock;
    xmsHDest                dest = XMS_NULL_HANDLE;
    xmsHMsgConsumer         consumer = XMS_NULL_HANDLE;
    int                     transacted = xmsFALSE;

    php_sam_xms_connection  *connResource;
    zval                    **zConnection;

    char                    *subscriptionName = NULL;

    if (SAM_DEBUG) zend_printf("<br/><b>SAMXMSConnection->subscribe()</b> enter<br/>");

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC,
        "s|a", &aDestination, &destLength, &zvOptions) == FAILURE) {
            setError(this_ptr TSRMLS_CC, SAM_CONNECTION_WRONG_PARAMETERS_SUBSCRIBE, SAM_CONNECTION_WRONG_PARAMETERS_SUBSCRIBE_MSG);
            WRONG_PARAM_COUNT;
            RETURN_FALSE;
    }

    if (SAM_DEBUG) zend_printf("<b>SAMXMSConnection->subscribe()</b> Target destination is: %s.<br/>", aDestination);

    /* Retrieve the connection resource from this object's hash table */
    if (zend_hash_find(Z_OBJPROP_P(this_ptr), SAM_CONNECTION_CONNECTION_INDEX, sizeof(SAM_CONNECTION_CONNECTION_INDEX), (void**)&zConnection) == FAILURE) {
        setError(this_ptr TSRMLS_CC, SAM_CONNECTION_NO_CONNECTION, SAM_CONNECTION_NO_CONNECTION_MSG);
        RETURN_FALSE;
    }

    ZEND_FETCH_RESOURCE(connResource, php_sam_xms_connection*, zConnection, -1, PHP_SAM_CONNECTION_RES_NAME, le_sam_xms_connection);
    if (SAM_DEBUG) zend_printf("<b>SAMXMSConnection->subscribe()</b> connection is %d<br/>", connResource->conn);

   /* Check we have a valid XMS session handle */
    if (XMS_NULL_HANDLE == connResource->session) {
        setError(this_ptr TSRMLS_CC, SAM_CONNECTION_NULL_SESSION, SAM_CONNECTION_NULL_SESSION_MSG);
        RETURN_FALSE;
    }

    if (XMS_OK != xmsErrorCreate(&errorBlock)) {
        setError(this_ptr TSRMLS_CC, SAM_CONNECTION_NO_ERROR_BLOCK, SAM_CONNECTION_NO_ERROR_BLOCK_MSG);
        RETURN_FALSE;
    }

    if (XMS_OK != xmsDestCreate(aDestination, &dest, errorBlock)) {
        setError(this_ptr TSRMLS_CC, SAM_CONNECTION_NO_DESTINATION, SAM_CONNECTION_NO_DESTINATION_MSG);
        if (SAM_DEBUG) check(errorBlock);
        RETURN_FALSE;
    }

    if (0 == SAMXMSGetSignature(&subscriptionName, aDestination)) {
        rc = xmsSessCreateDurableSubscriber(connResource->session, dest, subscriptionName, &consumer, errorBlock);
        if (XMS_OK != rc) {
             /* Have a look at the error and see if we are using WPM check for whether the DUR_SUB_HOME option was missing*/
            if ( (XMS_E_GXIWPMPROT_NO_DUR_SUB_HOME == check(errorBlock)) &&
                 (0 == strcmp(SAM_CONSTANT_WPM, connResource->protocol)) ) {

                /* We got an error becasue there was no DUR_SUB_HOME, give a message to this effect */
                setError(this_ptr TSRMLS_CC, SAM_CONNECTION_NO_WPMDURSUBHOME, SAM_CONNECTION_NO_WPMDURSUBHOME_MSG);
            } else {
                /* Some other error */
                setError(this_ptr TSRMLS_CC, SAM_CONNECTION_NO_CONSUMER, SAM_CONNECTION_NO_CONSUMER_MSG);
            }
        } else {
            if (SAM_DEBUG) zend_printf("<b>SAMXMSConnection->subscribe()</b> subscription url is %s<br/>", subscriptionName);

            /* Put our subscriptionName into the return value ZVAL */
            ZVAL_STRING(return_value, subscriptionName, 1);
        }
    } else {
        rc = 1;
    }

    if (NULL != subscriptionName) {
        efree(subscriptionName);
    }

    xmsMsgConsumerClose(&consumer, errorBlock);
    xmsErrorDispose(&errorBlock);

    if (SAM_DEBUG) zend_printf("<b>SAMXMSConnection->subscribe()</b> exit<br/>");

    if (XMS_OK != rc) {
        RETURN_FALSE;
    }
}
/* }}} */


/* {{{ proto PHP_NAMED_FUNCTION(SAM_Connection_unsubscribe)
     Cancel a subscription agaianst a particular topic.  */

PHP_FUNCTION(SAM_XMS_Connection_unsubscribe)
{
    xmsINT rc;
    xmsHDest dest = XMS_NULL_HANDLE;
    xmsHMsgConsumer         consumer = XMS_NULL_HANDLE;
    int transacted = xmsFALSE;
    xmsHErrorBlock errorBlock;
    php_sam_xms_connection *connResource;
    zval **zConnection;

    char *aSubscriptionName = NULL;
    int subNameLength;

    if (SAM_DEBUG) zend_printf("<br/><b>SAMXMSConnection->unsubscribe()</b> enter<br/>");

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC,
        "s", &aSubscriptionName, &subNameLength) == FAILURE) {
            setError(this_ptr TSRMLS_CC, SAM_CONNECTION_WRONG_PARAMETERS_UNSUBSCRIBE, SAM_CONNECTION_WRONG_PARAMETERS_UNSUBSCRIBE_MSG);
            WRONG_PARAM_COUNT;
            RETURN_FALSE;
    }

    if (SAM_DEBUG) zend_printf("<b>SAMXMSConnection->unsubscribe()</b> Subscription Name is: %s.<br/>", aSubscriptionName);


    /* Retrieve the connection resource from this object's hash table */
    if (zend_hash_find(Z_OBJPROP_P(this_ptr), SAM_CONNECTION_CONNECTION_INDEX, sizeof(SAM_CONNECTION_CONNECTION_INDEX), (void**)&zConnection) == FAILURE) {
        setError(this_ptr TSRMLS_CC, SAM_CONNECTION_NO_CONNECTION, SAM_CONNECTION_NO_CONNECTION_MSG);
        RETURN_FALSE;
    }

    ZEND_FETCH_RESOURCE(connResource, php_sam_xms_connection*, zConnection, -1, PHP_SAM_CONNECTION_RES_NAME, le_sam_xms_connection);
    if (SAM_DEBUG) zend_printf("<b>SAMXMSConnection->unsubscribe()</b> connection is %d<br/>", connResource->conn);

    /* Check we have a valid XMS session handle */
    if (XMS_NULL_HANDLE == connResource->session) {
        setError(this_ptr TSRMLS_CC, SAM_CONNECTION_NULL_SESSION, SAM_CONNECTION_NULL_SESSION_MSG);
        RETURN_FALSE;
    }

    /* create an XMS error block to capture error info...   */
    if (XMS_OK != xmsErrorCreate(&errorBlock)) {
        setError(this_ptr TSRMLS_CC, SAM_CONNECTION_NO_ERROR_BLOCK, SAM_CONNECTION_NO_ERROR_BLOCK_MSG);
        RETURN_FALSE;
    }

    /* NB: We have to create a subscriber and close it against this session otherwise the unsubscribe fails with
    a 2019 error (otherwise we would not need destination and create subscriber code) */

    if (XMS_OK != xmsDestCreate("topic://dummyDest", &dest, errorBlock)) {
        setError(this_ptr TSRMLS_CC, SAM_CONNECTION_NO_DESTINATION, SAM_CONNECTION_NO_DESTINATION_MSG);
        if (SAM_DEBUG) check(errorBlock);
        SAM_CLEANUP_UNSUBSCRIBE();
        RETURN_FALSE;
    }

    rc = xmsSessCreateDurableSubscriber(connResource->session, dest, aSubscriptionName, &consumer, errorBlock);
    if (XMS_OK != rc) {
        setError(this_ptr TSRMLS_CC, SAM_CONNECTION_NO_CONSUMER, SAM_CONNECTION_NO_CONSUMER_MSG);
        if (SAM_DEBUG) check(errorBlock);
        SAM_CLEANUP_UNSUBSCRIBE();
        RETURN_FALSE;
    }

    xmsMsgConsumerClose(&consumer, errorBlock);

    if (XMS_OK != xmsSessUnsubscribe(connResource->session, aSubscriptionName, errorBlock)) {
        setError(this_ptr TSRMLS_CC, SAM_CONNECTION_UNSUBSCRIBE_FAILED, SAM_CONNECTION_UNSUBSCRIBE_FAILED_MSG);
        if (SAM_DEBUG) check(errorBlock);
        SAM_CLEANUP_UNSUBSCRIBE();
        RETURN_FALSE;
    }

    /* clean up the XMS structures we have finished with...    */
    SAM_CLEANUP_UNSUBSCRIBE();

    if (SAM_DEBUG) zend_printf("<b>SAMXMSConnection->unsubscribe()</b> exit<br/>");
    RETURN_TRUE;
}
/* }}} */
