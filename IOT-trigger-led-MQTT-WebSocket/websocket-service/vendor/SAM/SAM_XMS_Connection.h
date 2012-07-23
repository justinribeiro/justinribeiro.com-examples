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

$Id: SAM_XMS_Connection.h,v 1.1 2007/02/02 15:40:00 dsr Exp $

*/

#if !defined(__SAMXMSPHPCONN_H__)
#define __SAMXMSPHPCONN_H__

#include <xms.h>

/* Resource struct...                       */
typedef struct _php_sam_xms_connection {
    char protocol[128];
    xmsHConn conn;
    xmsHSess session;
    xmsINT   transacted;
} php_sam_xms_connection;


extern function_entry sam_xms_conn_functions[];

PHP_NAMED_FUNCTION(SAM_XMS_Connection_ctor);
PHP_FUNCTION(SAM_XMS_Connection_commit);
PHP_FUNCTION(SAM_XMS_Connection_connect);
PHP_FUNCTION(SAM_XMS_Connection_disconnect);
PHP_FUNCTION(SAM_XMS_Connection_isConnected);
PHP_FUNCTION(SAM_XMS_Connection_peek);
PHP_FUNCTION(SAM_XMS_Connection_peekAll);
PHP_FUNCTION(SAM_XMS_Connection_receive);
PHP_FUNCTION(SAM_XMS_Connection_remove);
PHP_FUNCTION(SAM_XMS_Connection_rollback);
PHP_FUNCTION(SAM_XMS_Connection_send);
PHP_FUNCTION(SAM_XMS_Connection_setDebug);
PHP_FUNCTION(SAM_XMS_Connection_subscribe);
PHP_FUNCTION(SAM_XMS_Connection_unsubscribe);


#define PHP_SAM_CONNECTION_RES_NAME "__SAM_XMS_Connection_pointer"

/* Oddball constants             */
#define SAM_CONNECTION_CONNECTION_INDEX  "__CONNECTION"

#define SAM_CONNECTION_PROTOCOL_SEPERATOR ':'

/* Error codes and messages    */
#define SAM_CONNECTION_WRONG_PARAMETERS_CONNECT        101
#define SAM_CONNECTION_WRONG_PARAMETERS_CONNECT_MSG    "Incorrect number of parameters on connect call!"
#define SAM_CONNECTION_UNSUPPORTED_PROTOCOL            102
#define SAM_CONNECTION_UNSUPPORTED_PROTOCOL_MSG        "Unsupported protocol!"
#define SAM_CONNECTION_UNSUPPORTED_PROTOCOL_OPTION     103
#define SAM_CONNECTION_UNSUPPORTED_PROTOCOL_OPTION_MSG "Unsupported protocol option!"
#define SAM_CONNECTION_WRONG_PARAMETERS_RECEIVE        104
#define SAM_CONNECTION_WRONG_PARAMETERS_RECEIVE_MSG    "Incorrect number of parameters on receive call!"
#define SAM_CONNECTION_WRONG_PARAMETERS_SEND           105
#define SAM_CONNECTION_WRONG_PARAMETERS_SEND_MSG       "Incorrect number of parameters on send call!"
#define SAM_CONNECTION_NO_CONNECTION                   106
#define SAM_CONNECTION_NO_CONNECTION_MSG               "No currently active XMS connection!"
#define SAM_CONNECTION_INVALID_MESSAGE                 107
#define SAM_CONNECTION_INVALID_MESSAGE_MSG             "Parameter 2 is not a SAM_Message object!"
#define SAM_CONNECTION_NO_ERROR_BLOCK                  201
#define SAM_CONNECTION_NO_ERROR_BLOCK_MSG              "Unable to create XMS error block!"
#define SAM_CONNECTION_NO_CONNECTION_FACTORY           202
#define SAM_CONNECTION_NO_CONNECTION_FACTORY_MSG       "Unable to create XMS connection factory!"
#define SAM_CONNECTION_SET_CONNECTION_TYPE             203
#define SAM_CONNECTION_SET_CONNECTION_TYPE_MSG         "Unable to set XMS connection type property!"
#define SAM_CONNECTION_SET_CONNECTION_MODE             204
#define SAM_CONNECTION_SET_CONNECTION_MODE_MSG         "Unable to set XMS connection mode property!"
#define SAM_CONNECTION_SET_HOST_NAME                   205
#define SAM_CONNECTION_SET_HOST_NAME_MSG               "Unable to set XMS host name property!"
#define SAM_CONNECTION_SET_PORT                        206
#define SAM_CONNECTION_SET_PORT_MSG                    "Unable to set XMS port property!"
#define SAM_CONNECTION_SET_QUEUE_MANAGER_NAME          207
#define SAM_CONNECTION_SET_QUEUE_MANAGER_NAME_MSG      "Unable to set XMS host queue manager property!"
#define SAM_CONNECTION_CONNECTION_CREATE               208
#define SAM_CONNECTION_CONNECTION_CREATE_MSG           "Unable to create XMS connection!"
#define SAM_CONNECTION_NO_DESTINATION                  209
#define SAM_CONNECTION_NO_DESTINATION_MSG              "Unable to create XMS destination!"
#define SAM_CONNECTION_NO_SESSION                      210
#define SAM_CONNECTION_NO_SESSION_MSG                  "Unable to create XMS session!"
#define SAM_CONNECTION_NO_CONSUMER                     211
#define SAM_CONNECTION_NO_CONSUMER_MSG                 "Unable to create XMS consumer!"
#define SAM_CONNECTION_RECEIVE_FAILED                  212
#define SAM_CONNECTION_RECEIVE_FAILED_MSG              "XMS receive failed!"
#define SAM_CONNECTION_NO_PRODUCER                     213
#define SAM_CONNECTION_NO_PRODUCER_MSG                 "Unable to create XMS producer!"
#define SAM_CONNECTION_NO_BYTES_MESSAGE                214
#define SAM_CONNECTION_NO_BYTES_MESSAGE_MSG            "Unable to create XMS bytes message!"
#define SAM_CONNECTION_WRITE_UTF_FAILED                215
#define SAM_CONNECTION_WRITE_UTF_FAILED_MSG            "Unable to write UTF data to XMS bytes message!"
#define SAM_CONNECTION_SEND_FAILED                     216
#define SAM_CONNECTION_SEND_FAILED_MSG                 "XMS send failed!"
#define SAM_CONNECTION_SET_CONNECTION_PROTOCOL         217
#define SAM_CONNECTION_SET_CONNECTION_PROTOCOL_MSG     "Unable to set connection protocol property!"
#define SAM_CONNECTION_CONNECTION_START                218
#define SAM_CONNECTION_CONNECTION_START_MSG            "Unable to start XMS connection!"
#define SAM_CONNECTION_NO_MSG_BODY                     219
#define SAM_CONNECTION_NO_MSG_BODY_MSG                 "No body property defined in SAM_Message object!"
#define SAM_CONNECTION_DISCONNECT_FAILED               220
#define SAM_CONNECTION_DISCONNECT_FAILED_MSG           "XMS connection close failed!"
#define SAM_CONNECTION_NO_MSG_ID                       221
#define SAM_CONNECTION_NO_MSG_ID_MSG                   "Could not retrieve message ID of sent message!"
#define SAM_CONNECTION_SET_CORRELID_FAILED             222
#define SAM_CONNECTION_SET_CORRELID_FAILED_MSG         "XMS set JMSCorrelationID failed!"
#define SAM_CONNECTION_READBYTES_FAILED                223
#define SAM_CONNECTION_READBYTES_FAILED_MSG            "XMS read of UTF from message failed!"
#define SAM_CONNECTION_SET_ENDPOINTS                   224
#define SAM_CONNECTION_SET_ENDPOINTS_MSG               "XMS set endpoints on Connection Factory failed!"
#define SAM_CONNECTION_SET_BUS                         225
#define SAM_CONNECTION_SET_BUS_MSG                     "XMS set bus name on Connection Factory failed!"
#define SAM_CONNECTION_NO_REPLY_DESTINATION            226
#define SAM_CONNECTION_NO_REPLY_DESTINATION_MSG        "Unable to create XMS destination for SAM_REPLY_TO value!"
#define SAM_CONNECTION_REPLY_TO_SET_FAILED             227
#define SAM_CONNECTION_REPLY_TO_SET_FAILED_MSG         "Unable to set JMSReplyTo property!"
#define SAM_CONNECTION_SET_PROPERTY_FAILED             228
#define SAM_CONNECTION_SET_PROPERTY_FAILED_MSG         "Unable to set user property on message!"
#define SAM_CONNECTION_PROPERTY_ARRAY_INVALID          239
#define SAM_CONNECTION_PROPERTY_ARRAY_INVALID_MSG      "Property incorrectly specified. Should have 2 entries (value, typeString)!"
#define SAM_CONNECTION_PROPERTY_GET_NAME_FAILED        240
#define SAM_CONNECTION_PROPERTY_GET_NAME_FAILED_MSG    "Unable to retrieve XMS property name from message!"
#define SAM_CONNECTION_PROPERTY_GET_STRING_FAILED      241
#define SAM_CONNECTION_PROPERTY_GET_STRING_FAILED_MSG  "Unable to retrieve XMS property value as string from message!"
#define SAM_CONNECTION_MESSAGE_PROPERTY_SET_FAILED     242
#define SAM_CONNECTION_MESSAGE_PROPERTY_SET_FAILED_MSG "Unable to set property into SAM message!"
#define SAM_CONNECTION_HEADER_CREATE_FAILED            243
#define SAM_CONNECTION_HEADER_CREATE_FAILED_MSG        "Unable to create header property in message!"
#define SAM_CONNECTION_ILLEGAL_DELIVERY_MODE           244
#define SAM_CONNECTION_ILLEGAL_DELIVERY_MODE_MSG       "Illegal value for delivery mode!"
#define SAM_CONNECTION_ILLEGAL_PRIORITY                245
#define SAM_CONNECTION_ILLEGAL_PRIORITY_MSG            "Illegal value for priority!"
#define SAM_CONNECTION_SET_DELIVERY_MODE               246
#define SAM_CONNECTION_SET_DELIVERY_MODE_MSG           "Unable to set the XMS delivery mode!"
#define SAM_CONNECTION_SET_PRIORITY                    247
#define SAM_CONNECTION_SET_PRIORITY_MSG                "Unable to set the priority on the XMS message producer!"
#define SAM_CONNECTION_WRITE_BYTES_FAILED              248
#define SAM_CONNECTION_WRITE_BYTES_FAILED_MSG          "Unable to write UTF data to XMS bytes message!"
#define SAM_CONNECTION_ILLEGAL_TIMETOLIVE              249
#define SAM_CONNECTION_ILLEGAL_TIMETOLIVE_MSG          "Illegal value for time to live!"
#define SAM_CONNECTION_SET_TIMETOLIVE                  250
#define SAM_CONNECTION_SET_TIMETOLIVE_MSG              "Unable to set time to live on the XMS message producer!"
#define SAM_CONNECTION_MSG_TYPE_INVALID                251
#define SAM_CONNECTION_MSG_TYPE_INVALID_MSG            "Value specified for SAM_TYPE property not recognised!"
#define SAM_CONNECTION_NO_TEXT_MESSAGE                 252
#define SAM_CONNECTION_NO_TEXT_MESSAGE_MSG             "Unable to create XMS text message!"
#define SAM_CONNECTION_WRITE_TEXT_FAILED               253
#define SAM_CONNECTION_WRITE_TEXT_FAILED_MSG           "Unable to write string data to XMS text message!"
#define SAM_CONNECTION_TYPE_GET_FAILED                 254
#define SAM_CONNECTION_TYPE_GET_FAILED_MSG             "Unable to get message type!"
#define SAM_CONNECTION_GETTEXT_FAILED                  255
#define SAM_CONNECTION_GETTEXT_FAILED_MSG              "XMS get of text from message failed!"
#define SAM_CONNECTION_WRONG_PARAMETERS_UNSUBSCRIBE     256
#define SAM_CONNECTION_WRONG_PARAMETERS_UNSUBSCRIBE_MSG "Incorrect number of parameters on unsubscribe call!"
#define SAM_CONNECTION_UNSUBSCRIBE_FAILED               257
#define SAM_CONNECTION_UNSUBSCRIBE_FAILED_MSG           "Unable to unsubscribe from specified subscription!"
#define SAM_CONNECTION_WRONG_PARAMETERS_SUBSCRIBE       258
#define SAM_CONNECTION_WRONG_PARAMETERS_SUBSCRIBE_MSG   "Incorrect number of parameters on send call!"
#define SAM_CONNECTION_WRONG_PARAMETERS_PEEK            259
#define SAM_CONNECTION_WRONG_PARAMETERS_PEEK_MSG        "Incorrect number of parameters on peek call!"
#define SAM_CONNECTION_WRONG_PARAMETERS_REMOVE          260
#define SAM_CONNECTION_WRONG_PARAMETERS_REMOVE_MSG      "Incorrect number of parameters on remove call!"
#define SAM_CONNECTION_NO_QBROWSER                      261
#define SAM_CONNECTION_NO_QBROWSER_MSG                  "Unable to create XMS Queue Browser!"
#define SAM_CONNECTION_QBROWSE_FAILED                   262
#define SAM_CONNECTION_QBROWSE_FAILED_MSG               "XMS queue browse failed!"
#define SAM_CONNECTION_HASNEXT_FAILED                   263
#define SAM_CONNECTION_HASNEXT_FAILED_MSG               "XMS queue browse failed!"
#define SAM_CONNECTION_GETNEXT_FAILED                   264
#define SAM_CONNECTION_GETNEXT_FAILED_MSG               "XMS queue browse failed!"
#define SAM_CONNECTION_REMOVE_FAILED                    265
#define SAM_CONNECTION_REMOVE_FAILED_MSG                "XMS receive to remove message failed!"
#define SAM_CONNECTION_ILLEGAL_TARGETCLIENT             266
#define SAM_CONNECTION_ILLEGAL_TARGETCLIENT_MSG         "Illegal value for SAM_WMQ_TARGET_CLIENT!"
#define SAM_CONNECTION_ILLEGAL_TARGETCLIENT_MODE        267
#define SAM_CONNECTION_ILLEGAL_TARGETCLIENT_MODE_MSG    "SAM_WMQ_TARGET_CLIENT can only be used with WebSphere MQ!"
#define SAM_CONNECTION_SET_TARGET_CLIENT                268
#define SAM_CONNECTION_SET_TARGET_CLIENT_MSG            "Unable to set XMS target client property!"
#define SAM_CONNECTION_WRONG_PARAMETERS_PEEKALL         269
#define SAM_CONNECTION_WRONG_PARAMETERS_PEEKALL_MSG     "Incorrect number of parameters on peekAll call!"
#define SAM_CONNECTION_MSG_BUFFER_ALLOC_FAILED          270
#define SAM_CONNECTION_MSG_BUFFER_ALLOC_FAILED_MSG      "Unable to allocate buffer for message to be retrieved!"
#define SAM_CONNECTION_CORRELID_ALLOC_FAILED            271
#define SAM_CONNECTION_CORRELID_ALLOC_FAILED_MSG        "Unable to allocate buffer for correlation id!"
#define SAM_CONNECTION_PROPVAL_ALLOC_FAILED             272
#define SAM_CONNECTION_PROPVAL_ALLOC_FAILED_MSG         "Unable to allocate buffer for property value!"
#define SAM_CONNECTION_MESSAGEID_ALLOC_FAILED           273
#define SAM_CONNECTION_MESSAGEID_ALLOC_FAILED_MSG       "Unable to allocate buffer for message id!"
#define SAM_CONNECTION_PROPNAME_ALLOC_FAILED            274
#define SAM_CONNECTION_PROPNAME_ALLOC_FAILED_MSG         "Unable to allocate buffer for property name!"
#define SAM_CONNECTION_HEADERS_ALLOC_FAILED             275
#define SAM_CONNECTION_HEADERS_ALLOC_FAILED_MSG         "Unable to allocate buffer for SAM headers buffer!"
#define SAM_CONNECTION_CONNECTION_STOP                  276
#define SAM_CONNECTION_CONNECTION_STOP_MSG              "Unable to stop XMS connection!"
#define SAM_CONNECTION_CONNECTION_CLOSE                 277
#define SAM_CONNECTION_CONNECTION_CLOSE_MSG             "Unable to close XMS connection!"
#define SAM_CONNECTION_CONNFACT_DELETE                  278
#define SAM_CONNECTION_CONNFACT_DELETE_MSG              "Unable to delete XMS connection factory!"
#define SAM_CONNECTION_ILLEGAL_DEST_PREFIX              279
#define SAM_CONNECTION_ILLEGAL_DEST_PREFIX_MSG          "Specified destination is not a valid queue, topic or subscription"
#define SAM_CONNECTION_WPMDURSUBHOME_ALLOC_FAILED       280
#define SAM_CONNECTION_WPMDURSUBHOME_ALLOC_FAILED_MSG   "Unable to allocate buffer for WPM durable subscription home!"
#define SAM_CONNECTION_ILLEGAL_WPMDURSUBHOME_MODE       281
#define SAM_CONNECTION_ILLEGAL_WPMDURSUBHOME_MODE_MSG   "SAM_WPM_DUR_SUB_HOME can only be used with WPM!"
#define SAM_CONNECTION_NO_WPMDURSUBHOME                 282
#define SAM_CONNECTION_NO_WPMDURSUBHOME_MSG             "SAM_WPM_DUR_SUB_HOME must be set as a connect option when using WPM!"
#define SAM_CONNECTION_SET_WPMDURSUBHOME                283
#define SAM_CONNECTION_SET_WPMDURSUBHOME_MSG            "Unable to set XMS WPM durable subscription home property!"
#define SAM_CONNECTION_NULL_SESSION                     284
#define SAM_CONNECTION_NULL_SESSION_MSG                 "XMS Session handle is NULL!"
#define SAM_CONNECTION_SESSION_CLOSE                    285
#define SAM_CONNECTION_SESSION_CLOSE_MSG                "Unable to close XMS session!"
#define SAM_CONNECTION_TRANSACTIONS_ALLOC_FAILED        286
#define SAM_CONNECTION_TRANSACTIONS_ALLOC_FAILED_MSG    "Unable to allocate buffer for transaction options!"
#define SAM_CONNECTION_ILLEGAL_TRANSACTION_MODE         287
#define SAM_CONNECTION_ILLEGAL_TRANSACTION_MODE_MSG     "Illegal value for SAM_TRANSACTIONS|"
#define SAM_CONNECTION_ROLLBACK_FAILED                  288
#define SAM_CONNECTION_ROLLBACK_FAILED_MSG              "XMS Session rollback failed!"
#define SAM_CONNECTION_COMMIT_FAILED                    289
#define SAM_CONNECTION_COMMIT_FAILED_MSG                "XMS Session commit failed!"
#define SAM_CONNECTION_USERID_ALLOC_FAILED              290
#define SAM_CONNECTION_USERID_ALLOC_FAILED_MSG          "Unable to allocate buffer for userid"
#define SAM_CONNECTION_PASSWORD_ALLOC_FAILED            291
#define SAM_CONNECTION_PASSWORD_ALLOC_FAILED_MSG        "Unable to allocate buffer for password!"
#define SAM_CONNECTION_SET_USERID                       292
#define SAM_CONNECTION_SET_USERID_MSG                   "Unable to set XMS userid property!"
#define SAM_CONNECTION_SET_PASSWORD                     293
#define SAM_CONNECTION_SET_PASSWORD_MSG                 "Unable to set XMS password property!"

#define SAM_MESSAGE_BODY                  "body"
#define SAM_MESSAGE_HEADER                "header"

/* We make available a constant SAM_RECEIVE_TIMEDOUT in php_sam.h and php_sam.c which maps to the following value. */
#define SAM_CONNECTION_RECEIVE_TIMEDOUT                 500
#define SAM_CONNECTION_RECEIVE_TIMEDOUT_MSG             "No message available to receive!"

/* internal constants                                             */
#define SAM_MSGTYPE_TEXT     1
#define SAM_MSGTYPE_BYTES    2

/* Macros used to clean up XMS objects in the various methods */

#define SAM_CLEANUP_PEEK() \
        /* Free the buffer we allocated for the data retrieved from the message */ \
        if (NULL != text) efree(text);  \
        text = NULL; \
        \
        xmsMsgDispose(&message, errorBlock); \
        xmsQueueBrowserClose(&qbrowser, errorBlock); \
        xmsErrorDispose(&errorBlock);

#define SAM_CLEANUP_PEEKALL() SAM_CLEANUP_PEEK()

#define SAM_CLEANUP_RECEIVE() \
            xmsMsgDispose(&message, errorBlock);  \
            xmsMsgConsumerClose(&consumer, errorBlock); \
            xmsErrorDispose(&errorBlock);

#define SAM_CLEANUP_REMOVE() SAM_CLEANUP_RECEIVE()

#define SAM_CLEANUP_SEND() \
        xmsMsgDispose(&message, errorBlock); \
        xmsMsgProducerClose(&producer, errorBlock); \
        xmsErrorDispose(&errorBlock);

#define SAM_CLEANUP_UNSUBSCRIBE() \
        xmsMsgConsumerClose(&consumer, errorBlock); \
        xmsErrorDispose(&errorBlock);

#endif /*__SAMPHPCONN_H__) */


