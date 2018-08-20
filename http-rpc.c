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

#include "http-rpc.h"
#include <string.h>

HttpServer_Error HttpRpc_performingRequest(void* dev,
                                           HttpServer_MessageHandle message)
{
	if(message->request == HTTPSERVER_REQUEST_GET)
	{
	    HttpRpc_getHandler(dev, message);
	}
	else
	{
	    message->responseCode = HTTPSERVER_RESPONSECODE_NOTFOUND;
	}
}

HttpRpc_Error HttpRpc_init (HttpRpc_DeviceHandle dev)
{
	dev->httpServer.ethernetSocketConfig = dev->config.ethernetSocketConfig;
	dev->httpServer.socketNumber = dev->config.socketNumber;
	dev->httpServer.port = dev->config.port;
	//httpRpc.httpServer = &httpServer;

//	HttpRpc_Rule rules[HTTPRPC_RULES_MAX_NUMBER];
//
//	dev->rules = rules;

	dev->httpServer.performingCallback = HttpRpc_performingRequest;
	dev->httpServer.appDevice = dev;

	HttpServer_open(&(dev->httpServer));


}

HttpRpc_Error HttpRpc_poll (HttpRpc_DeviceHandle dev, uint16_t timeout)
{
	HttpServer_poll(&(dev->httpServer), timeout);
}

HttpRpc_Error HttpRpc_getHandler(HttpRpc_DeviceHandle dev,
                                 HttpServer_MessageHandle message)
{
    char tokenCharacter[4] = {'\0','\0','0','\0'};
    char* token;
    uint8_t i = 0;
    uint8_t j = 0;
    uint16_t rpcCommandArgumentsIndex = 0;
    uint16_t argumentLength = 0;

    /**
     * check if a rule match the rpc command arrived*/
    for (i = 0; i<HTTPRPC_RULES_MAX_NUMBER; i++)
    {
        /**
         * dummy check to know if there is a ruleClass*/
        if (dev->rules[i].ruleClass[0] != '\0')
        {
            for(j = 0; j < HTTPRPC_MAX_ARGUMENT_NUMBER + 2; j++)
            {
                if (j < 2)
                    {
                     //check if a ruleClass match the rpc command class arrived
                        if (j == 0)
                        {
                            // first token character MUST be '/'
                            tokenCharacter[0] = '/';
                            token = strtok(message->uri,
                                           tokenCharacter);
                            if(strcmp (token,
                                       dev->rules[i].ruleClass) == 0)
                            {
                                continue;
                            }
                            else
                            {
                                message->responseCode = HTTPSERVER_RESPONSECODE_BADREQUEST;
                                return HTTPRPC_ERROR_RPC_COMMAND_NOT_RECOGNIZE;
                            }
                        }
                        /*
                         *check if a ruleFunction match
                         *the rpc command function arrived
                         */
                        else if (j == 1)
                        {
                            // next tokens characters MUST be ' '
                            tokenCharacter[0] = '%';
                            tokenCharacter[1] = '2';
                            tokenCharacter[2] = '0';
                            token = strtok(NULL, tokenCharacter);
                            if(strcmp (token, dev->rules[i].ruleFunction) == 0)
                            {
                                continue;
                            }
                            else
                            {
                                message->responseCode = HTTPSERVER_RESPONSECODE_BADREQUEST;
                                return HTTPRPC_ERROR_RPC_COMMAND_NOT_RECOGNIZE;
                            }
                        }
                    }

                        token = strtok(NULL, tokenCharacter);
                        argumentLength = strlen(token);
                        if((argumentLength + rpcCommandArgumentsIndex) < HTTPRPC_MAX_ARGUMENTS_LENGTH)
                        {
                            if(token==NULL) break;
                            strncpy(&(dev->rpcCommandArguments[rpcCommandArgumentsIndex]),
                                    token,
                                    argumentLength);
                            rpcCommandArgumentsIndex += argumentLength;
                            dev->rpcCommandArguments[rpcCommandArgumentsIndex] = ' ';
                            rpcCommandArgumentsIndex += 1;
                        }

                        else
                        {
                            message->responseCode = HTTPSERVER_RESPONSECODE_REQUESTENTITYTOOLARGE;
                            return HTTPRPC_ERROR_RPC_COMMAND_TOO_LONG;

                        }

                    }

            dev->rules[i].applicationCallback(dev->rpcCommandArguments);
            message->responseCode = HTTPSERVER_RESPONSECODE_OK;

            return HTTPRPC_ERROR_OK;
        }



    }
}

HttpRpc_Error HttpRpc_addRule(HttpRpc_DeviceHandle dev,
                              uint8_t ruleNumber,
                              char* class,
                              char* function,
                              void (ruleCallback)(char* argument))
{
    strncpy(dev->rules[ruleNumber].ruleClass, class, strlen(class));
    strncpy(dev->rules[ruleNumber].ruleFunction, function, strlen(function));
    dev->rules[ruleNumber].applicationCallback = ruleCallback;
}

