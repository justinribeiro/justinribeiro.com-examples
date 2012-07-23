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

$Id: SAM_XMS_Utils.c,v 1.1 2007/02/02 15:40:00 dsr Exp $

*/

#ifdef LINUX
#include <stdlib.h>     /* for random */
#include <sys/types.h>  /* for getpid */
#include <linux/unistd.h> /* for gettid */
#include <unistd.h>     /* for getpid */
#include <time.h>       /* for time() call */

#include <netdb.h>  /* For gethostbyname */
#include <sys/socket.h> /* For inet_ntoa */
#include <netinet/in.h> /* For inet_ntoa */
#include <arpa/inet.h>  /* For inet_ntoa */
#include <string.h>     /* For memcpy */
#include <stdio.h>

/* Includes for the Zend PHP API */
#include <php.h>
#include <php_ini.h>
#include <ext/standard/info.h>

/* Includes for our implementation */
#include "php_sam.h"
#include "php_sam_xms.h"

extern int SAM_DEBUG;

/* {{{ proto char* SAMXMSGetMyIPAddress()
  Gets the machine's IP address as a string (Linux version)  */

char *SAMXMSGetMyIPAddress()
{
    char *myIPAddress = NULL;
    char hostName[128];
    struct hostent *h;

    if (0 == gethostname(hostName, sizeof(hostName)))
    {
        if (NULL != (h = gethostbyname(hostName)))
        {
            struct in_addr sinAddr;

            memcpy(&sinAddr, h->h_addr_list[0], h->h_length);
            myIPAddress = inet_ntoa(sinAddr);
        }
    }
    return(myIPAddress);
}
/* }}} */


/* {{{ proto int SAMGetSignature(char** pASig, char* aDestination)
  Creates a unique subscription id (Linux version)  */

int SAMXMSGetSignature(char **pASig, char *aDestination)
{
    pid_t myPid;
    int aRand;
    time_t thisTime = 0;
    char *myIPAddress;
    char *aSig = NULL;

   /* Initially allocate the buffer we are doing to return based on 128 bytes plus the destination name length */
    if (NULL != (aSig = emalloc(128 + strlen(aDestination)))) {
        myIPAddress = SAMXMSGetMyIPAddress();
        if (myIPAddress != NULL)
        {
            /* On Linux the process id returned by getpid() is actually unique per thread */
            myPid = getpid();

            time(&thisTime);

            /* Seed the random number generator from the time of day */
            srand(thisTime);

            aRand = rand();

            sprintf(aSig, "%s%s_%d_%d_%d%s", SAM_CONSTANT_SUBPREFIX, myIPAddress, myPid, thisTime, aRand, aDestination);

            /* Reallocate the length of the buffer according to the actual final length */
            erealloc(aSig, 1 + strlen(aSig));

            *pASig = aSig;

            return 0;
        }
        else
            return 1;
    }
    return 1;
}
/* }}} */

#else

/* Includes for the Zend PHP API */
#include <php.h>
#include <php_ini.h>
#include <ext/standard/info.h>

/* Includes for our implementation */
#include "php_sam.h"
#include "php_sam_xms.h"

extern int SAM_DEBUG;


/* {{{ proto char* SAMXMSGetMyIPAddress()
  Gets the machine's IP address as a string (Windows version)  */

char *SAMXMSGetMyIPAddress()
{
    WSADATA wsd;
    char *myIPAddress = NULL;
    char hostName[128];

    if ((0 == WSAStartup( MAKEWORD( 2, 2 ), &wsd )) &&
        (0 == gethostname(hostName, sizeof(hostName)))) {
            struct hostent* FAR h;

            if (NULL != (h = gethostbyname(hostName)))
            {
                struct in_addr sinAddr;

                memcpy( &sinAddr, h->h_addr_list[0], h->h_length );
                myIPAddress = inet_ntoa( sinAddr );
            }
        }

        WSACleanup();
        return myIPAddress;
}
/* }}} */


/* {{{ proto int SAMGetSignature(char** pASig, char* aDestination)
  Creates a unique subscription id (Windows version)  */

int SAMXMSGetSignature(char **pASig, char *aDestination)
{
    int myPid;
    int myTid;
    unsigned int myRand;
    FILETIME ft;
    char *myIPAddress;
    char *aSig=NULL;

    /* Initially allocate the buffer we are doing to return based on 128 bytes plus the destination name length */
    if (NULL != (aSig = emalloc(128 + strlen(aDestination)))) {
        myIPAddress = SAMXMSGetMyIPAddress();
        if (myIPAddress != NULL)
        {
            srand( (unsigned) time(NULL) );
            myPid = GetCurrentProcessId();
            myTid = GetCurrentThreadId();

            /* rand_s is a better function on VS2005 */
            myRand = rand();

            GetSystemTimeAsFileTime(&ft);

            sprintf(aSig, "%s%s_%d_%d_%u_%u_%d%s", SAM_CONSTANT_SUBPREFIX, myIPAddress, myPid, myTid, ft.dwHighDateTime, ft.dwLowDateTime, myRand, aDestination);

            /* Reallocate the length of the buffer according to the actual final length */
            erealloc(aSig, 1 + strlen(aSig));

            *pASig = aSig;
            return 0;
        }
        else
            return 1;
    }
    return 1;
}
/* }}} */

#endif

/* {{{ proto xmsCHAR* SAMXMSBuildSelectorString(char* correlId, char* messageId)
  Creates a selector definition for peek and receive methods  */

xmsCHAR *SAMXMSBuildSelectorString(char *correlId, char *messageId) {
    xmsCHAR *mySelector = NULL;

    /* Add any correlid part of the selector */
    if ( (NULL != correlId) && (*correlId != '\0')) {
        mySelector = emalloc(22 + strlen(correlId));
        sprintf(mySelector, "JMSCorrelationID = '%s'", correlId);
    }

    /* Add any message id part of the selector */
    if ( ( NULL != messageId) && (*messageId != '\0'))
    {
        if ( ( NULL != mySelector) && (0 != strlen(mySelector)) )
        {
            mySelector = erealloc(mySelector, strlen(mySelector) + 22 + strlen(messageId));
            strcat(mySelector, " AND ");
            strcat(mySelector, "JMSMessageID = '");
        }
        else
        {
            mySelector = emalloc(18 + strlen(messageId));
            strcpy(mySelector, "JMSMessageID = '");
        }

        strcat(mySelector, messageId);
        strcat(mySelector, "'");
    }

    return mySelector;
}
/* }}} */

/* Set the errno and error properties on an object
(Should probably be in a separate error module)
*/
void setError(zval *objvar TSRMLS_DC, int errorCode, char *errorString) {
    add_property_long(objvar, "errno", errorCode);
    add_property_string(objvar, "error", errorString, 1);
}

/* Reset the errno and error properties
*/
void resetError(zval *objvar TSRMLS_DC) {
    add_property_long(objvar, "errno", 0);
    add_property_string(objvar, "error", "", 1);
}
