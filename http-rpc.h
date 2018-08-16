/*******************************************************************************
 * A simple HTTP/RPC library
 * Copyright (C) 2018 A. C. Open Hardware Ideas Lab
 *
 * Authors:
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
 *
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

#include "libohiboard.h"

#include "ethernet-socket/ethernet-socket.h"
#include "ethernet-socket/ethernet-serversocket.h"

#ifndef __NO_BOARD_H
#include "board.h"
#endif

#ifndef HTTPRRC_TIMEOUT
#define HTTPRPC_TIMEOUT                 3000
#endif

//#define HTTP_RPC_MAX_URI_LENGTH         200
//#define HTTP_RPC_BUFFER_DIMENSION		  0x3FF

#ifndef HTTPRPC_RULES_MAX_NUMBER
#define HTTPRPC_RULES_MAX_NUMBER        5
#endif

#include "http-server/http-server.h"

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
} HttpRpc_Error;

typedef struct _HttpRpc_Rule
{
    char rule[10];
    void (*applicationCallback)(uint8_t value);
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

    HttpRpc_RuleHandle rules;


} HttpRpc_Device, *HttpRpc_DeviceHandle;

HttpRpc_Error HttpRpc_init (HttpRpc_DeviceHandle httpRpc);

HttpRpc_Error HttpRpc_poll(HttpRpc_DeviceHandle httpServer, uint16_t timeout);

HttpServer_Error HttpRpc_performingRequest(void* dev,
                                           HttpServer_MessageHandle message);

HttpRpc_Error HttpRpc_getHandler(HttpRpc_DeviceHandle dev,
                                 HttpServer_MessageHandle message);

//HttpRpc_Error HttpRpc_postHandler(HttpServer_MessageHandle message);
//HttpRpc_Error HttpRpc_headHandler(HttpServer_MessageHandle message);
//HttpRpc_Error HttpRpc_putHandler(HttpServer_MessageHandle message);
//HttpRpc_Error HttpRpc_deleteHandler(HttpServer_MessageHandle message);
//HttpRpc_Error HttpRpc_traceHandler(HttpServer_MessageHandle message);
//HttpRpc_Error HttpRpc_optionsHandler(HttpServer_MessageHandle message);
//HttpRpc_Error HttpRpc_connectHandler(HttpServer_MessageHandle message);

HttpRpc_Error HttpRpc_addRule(void);


#endif // __OHILAB_HTTP_RPC_H
