/*
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
 */

/**
 * @mainpage HTTP/RPC library with @a libohiboard
 *
 * This project...
 *
 * @section changelog ChangeLog
 *
 * @li v1.0 of 2018/08/XX - First release
 *
 * @section library External Library
 *
 * The library use the following external library
 * @li libohiboard https://github.com/ohilab/libohiboard a C framework for
 * NXP Kinetis microcontroller
 * @li ethern-socket https://github.com/ohilab/ethernet-socket a C
 * library to manage client/server socket
 * @li http-server https://github.com/ohilab/http-server a C library
 * to create a simple http server and manage http request
 *
 * @section thanksto Thanks to...
 * @li Gianluca Calignano
 * @li Marco Giammarini
 *
 */

/**
 * @defgroup httpRpc_functions HTTP RPC functions
 * The HTTP RPC function group
 *
 * @defgroup httpRpc_macros HTTP RPC macros
 * @ingroup httpRpc_functions
 * Macros MUST be defined to enable the library to work properly.
 * They could be defined in board.h but there is a check in http-rpc.h which
 * avoid silly problems if macros are not defined.
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

/**
 * @ingroup httpRpc_macros
 * The max number of rules of each @ref HttpRpc_device .
 */
#ifndef HTTPRPC_RULES_MAX_NUMBER
#define HTTPRPC_RULES_MAX_NUMBER        5
#endif
/**
 * @ingroup httpRpc_macros
 * The max length of rule class string stored in
 * @ref HttpRpc_device .
 */
#ifndef HTTPRPC_MAX_RULE_CLASS_LENGTH
#define HTTPRPC_MAX_RULE_CLASS_LENGTH         32
#endif
/**
 * @ingroup httpRpc_macros
 * The max length of rule function string stored in
 * @ref HttpRpc_device .
 */
#ifndef HTTPRPC_MAX_RULE_FUNCTION_LENGTH
#define HTTPRPC_MAX_RULE_FUNCTION_LENGTH    32
#endif
/**
 * @ingroup httpRpc_macros
 * The max argument number which it can be passed
 * to @ref HttpRpc_Rule.applicationCallback .
 */
#ifndef HTTPRPC_MAX_ARGUMENT_NUMBER
#define HTTPRPC_MAX_ARGUMENT_NUMBER     2
#endif
/**
 * @ingroup httpRpc_macros
 * The max arguments string length which it can be passed
 * to @ref HttpRpc_Rule.applicationCallback .
 */
#ifndef HTTPRPC_MAX_ARGUMENTS_LENGTH
#define HTTPRPC_MAX_ARGUMENTS_LENGTH   255
#endif
/**
 * @ingroup httpRpc_macros
 * The max rule class string length which can be store
 * in @ref  HttpRpc_Rule.class .
 */
#ifndef HTTPRPC_MAX_RULE_CLASS_LENGTH
#define HTTPRPC_MAX_RULE_CLASS_LENGTH   255
#endif
/**
 * @ingroup httpRpc_macros
 * The max rule function string length which can be store
 * in @ref  HttpRpc_Rule.function .
 */
#ifndef HTTPRPC_MAX_RULE_FUNCTION_LENGTH
#define HTTPRPC_MAX_RULE_FUNCTION_LENGTH   255
#endif

/**
 * @ingroup httpRpc_functions
 * New enum types are defined to collect and monitor possible errors.
 */
typedef enum
{   ///Everithing gone well
    HTTPRPC_ERROR_OK,
    ///Wrong port
	HTTPRPC_ERROR_WRONG_PORT,
	///Wrong socket number
	HTTPRPC_ERROR_WRONG_SOCKET_NUMBER,
	///Wrong client number
    HTTPRPC_ERROR_WRONG_CLIENT_NUMBER,
    ///HttpRpc server not start
	HTTPRPC_ERROR_OPEN_FAIL,
	///Timeout occur
	HTTPRPC_ERROR_TIMEOUT,
	///Wrong request format arrived
    HTTPRPC_ERROR_WRONG_REQUEST_FORMAT,
    ///Rpc command not recognize
    HTTPRPC_ERROR_RPC_COMMAND_NOT_RECOGNIZE,
    ///Rpc command too long
    HTTPRPC_ERROR_RPC_COMMAND_TOO_LONG,
} HttpRpc_Error;

typedef struct _HttpRpc_Rule
{
    /** The class string which will be compared with the incoming request */
    char ruleClass[HTTPRPC_MAX_RULE_CLASS_LENGTH+1];
    /** The function string which will be compared with the incoming request */
    char ruleFunction[HTTPRPC_MAX_RULE_FUNCTION_LENGTH+1];
    /** The callback which is going to call if the rule is recognized */
    void (*applicationCallback)(char* argument);
} HttpRpc_Rule, *HttpRpc_RuleHandle;

typedef struct _HttpRpc_Device
{
	HttpServer_Device httpServer;  /**< An internal http server device where
	                                    config will be store if httpRpc_init is
	                                    called*/
	struct HttpRpc_Config
    {
        uint16_t port;        /**< The number of the port for the http server*/
	    uint8_t socketNumber;     /** < The socket number for the http server*/
	    EthernetSocket_Config* ethernetSocketConfig;/** < The pointer to the ethernet config*/
    }config;

    HttpRpc_Rule rules[HTTPRPC_RULES_MAX_NUMBER];/**< The array of rules*/
    char rpcCommandArguments[HTTPRPC_MAX_ARGUMENTS_LENGTH+1];/**< The string where the request argument will be stored*/

} HttpRpc_Device, *HttpRpc_DeviceHandle;

/**
 * @ingroup httpRpc_functions
 * This is the function which initializes the @ref HttpRpc_Device previously
 * defined
 * @param dev The RPC server pointer which is previously definited
 */
HttpRpc_Error HttpRpc_init (HttpRpc_DeviceHandle dev);

/**
 * @ingroup httpRpc_functions
 * This is the polling function which MUST be called in loop
 * @param dev The RPC server pointer where the polling is do
 */
HttpRpc_Error HttpRpc_poll (HttpRpc_DeviceHandle dev);

/**
 * @ingroup httpRpc_functions
 * This is the callback function which is call when a http request arrived
 * @param dev The RPC server pointer where the request is arrived
 */
HttpServer_Error HttpRpc_performingRequest (void* dev,
                                            HttpServer_MessageHandle message);
/**
 * @ingroup httpRpc_functions
 * This funcion manages every GET request parsing the URI and comparing
 * it with @ref HttpRpc_Device.rules previously stored in the relative
 * @ref HttpRpc_Device
 * @param dev The RPC server pointer there the request is arrived
 * @param message The message which is arrived
 */
HttpRpc_Error HttpRpc_getHandler (HttpRpc_DeviceHandle dev,
                                  HttpServer_MessageHandle message);

/**
 * @ingroup httpRpc_functions
 * This function adds a @ref HttpRpc_Rule to the @ref HttpRpc_Device.rules
 * array in the @ref HttpRpc_Device desired
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
