/*******************************************************************************
 * A simple HTTP/RPC library
 * Copyright (C) 2018 A. C. Open Hardware Ideas Lab
 *
 * Authors:
 *  Gianluca Calignano <g.calignano97@gmail.com>
 *  Marco Giammarini <m.giammarini@warcomeb.it>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 ******************************************************************************/

/**
 * @mainpage HTTP/RPC library with @a libohiboard
 *
 * This project...
 *
 * @section changelog ChangeLog
 *
 * @li v1.0 of 2018/03/XX - First release
 *
 * @section library External Library
 *
 * The library use the following external library
 * @li libohiboard https://github.com/ohilab/libohiboard a C framework for
 * NXP Kinetis microcontroller
 * @li ethern-socket https://github.com/ohilab/ethernet-socket a C
 * library to manage client/server socket
 *
 * @section thanksto Thanks to...
 * @li Gianluca Calignano
 * @li Marco Giammarini
 *
 */

#ifndef __OHILAB_HTTP_RPC_H
#define __OHILAB_HTTP_RPC_H

#define OHILAB_HTTP_RPC_LIBRARY_VERSION     "1.0.0"
#define OHILAB_HTTP_RPC_LIBRARY_VERSION_M   1
#define OHILAB_HTTP_RPC_LIBRARY_VERSION_m   0
#define OHILAB_HTTP_RPC_LIBRARY_VERSION_bug 0
#define OHILAB_HTTP_RPC_LIBRARY_TIME        0

#ifndef __NO_BOARD_H
#include "board.h"
#endif

// Ethernet server socket
#include "ethernet-socket/ethernet-serversocket.h"

// HTTP Server
#include "http-server/http-server.h"

#ifndef HTTPRRC_TIMEOUT
#define HTTPRPC_TIMEOUT                 3000
#endif

//#define HTTP_RPC_MAX_URI_LENGTH         200
//#define HTTP_RPC_BUFFER_DIMENSION		  0x3FF

#ifndef HTTPRPC_RULES_MAX_NUMBER
#define HTTPRPC_RULES_MAX_NUMBER        5
#endif
#ifndef HTTPRPC_MAX_RULE_CLASS_LENGTH
#define HTTPRPC_MAX_RULE_CLASS_LENGTH         32
#endif
#ifndef HTTPRPC_MAX_RULE_FUNCTION_LENGTH
#define HTTPRPC_MAX_RULE_FUNCTION_LENGTH    32
#endif
#ifndef HTTPRPC_MAX_ARGUMENT_NUMBER
#define HTTPRPC_MAX_ARGUMENT_NUMBER     2
#endif
#ifndef HTTPRPC_MAX_ARGUMENTS_LENGTH
#define HTTPRPC_MAX_ARGUMENTS_LENGTH   255
#endif
#ifndef HTTPRPC_MAX_RULE_CLASS_LENGTH
#define HTTPRPC_MAX_RULE_CLASS_LENGTH   255
#endif
#ifndef HTTPRPC_MAX_RULE_FUNCTION_LENGTH
#define HTTPRPC_MAX_RULE_FUNCTION_LENGTH   255
#endif

typedef enum
{
    HTTPRPC_ERROR_OK,
	HTTPRPC_ERROR_WRONG_PORT,
	HTTPRPC_ERROR_WRONG_SOCKET_NUMBER,
    HTTPRPC_ERROR_WRONG_CLIENT_NUMBER,
	HTTPRPC_ERROR_OPEN_FAIL,
	HTTPRPC_ERROR_WRONG_PARAM,
	HTTPRPC_ERROR_TIMEOUT,
    HTTPRPC_ERROR_WRONG_REQUEST_FORMAT,
    HTTPRPC_ERROR_RPC_COMMAND_NOT_RECOGNIZE,
    HTTPRPC_ERROR_RPC_COMMAND_TOO_LONG,
} HttpRpc_Error;

typedef struct _HttpRpc_Rule
{
    char ruleClass[HTTPRPC_MAX_RULE_CLASS_LENGTH+1];
    char ruleFunction[HTTPRPC_MAX_RULE_FUNCTION_LENGTH+1];
    void (*applicationCallback)(char* argument);
} HttpRpc_Rule, *HttpRpc_RuleHandle;

typedef struct _HttpRpc_Device
{
	HttpServer_Device httpServer;

	struct HttpRpc_Config
    {
        uint16_t port;
	    uint8_t socketNumber;
	    EthernetSocket_Config* ethernetSocketConfig;
    }config;

    HttpRpc_Rule rules[HTTPRPC_RULES_MAX_NUMBER];
    char rpcCommandArguments[HTTPRPC_MAX_ARGUMENTS_LENGTH+1];

} HttpRpc_Device, *HttpRpc_DeviceHandle;

/**
 * @param dev The RPC server pointer which is previously definited
 */
HttpRpc_Error HttpRpc_init (HttpRpc_DeviceHandle httpRpc);

/**
 * @param dev The RPC server pointer where the polling is do
 */
HttpRpc_Error HttpRpc_poll (HttpRpc_DeviceHandle dev);

/**
 * @param dev The RPC server pointer where the request is arrived
 */
HttpServer_Error HttpRpc_performingRequest (void* dev,
                                            HttpServer_MessageHandle message);
/**
 * @param dev The RPC server pointer there the request is arrived
 * @param nmessage The message which is arrived
 */
HttpRpc_Error HttpRpc_getHandler (HttpRpc_DeviceHandle dev,
                                  HttpServer_MessageHandle message);

/**
 * @param dev The RPC server pointer where a new rule is going to store
 * @param ruleNumber The rule number in the array of rules
 * @param[in] class The char pointer which is going to use to check if the
 * class rule is matched
 * @param[in] function The char pointer which is going to use to check if the
 * function is matched
 * @param ruleCallback The callback which is going to call if the rule is arrived
 * in a request
 */
HttpRpc_Error HttpRpc_addRule(HttpRpc_DeviceHandle dev,
                              uint8_t ruleNumber,
                              char* class,
                              char* function,
                              void (ruleCallback)(char* argument));


#endif // __OHILAB_HTTP_RPC_H
