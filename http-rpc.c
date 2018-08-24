/*
 * A simple HTTP/RPC library
 * Copyright (C) 2018 A. C. Open Hardware Ideas Lab
 *
 * Authors:
 * Marco Giammarini <m.giammarini@warcomeb.it>
 * Gianluca Calignano <g.calignano97@gmail.com>
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

#include "http-rpc.h"
#include <string.h>

#ifdef OHILAB_HTTPSERVER_DEBUG
#include "cli/cli.h"
#endif

HttpServer_Error HttpRpc_performingRequest(void* dev,
                                           HttpServer_MessageHandle message,
                                           uint8_t clientNumber)
{
	if (message->request == HTTPSERVER_REQUEST_GET)
	{
	    HttpRpc_getHandler(dev, message, clientNumber);
	    return HTTPSERVER_ERROR_OK;
	}
	else
	{
	    message->responseCode = HTTPSERVER_RESPONSECODE_NOTFOUND;
	    return HTTPSERVER_ERROR_WRONG_REQUEST_FORMAT;
	}
}

HttpRpc_Error HttpRpc_init (HttpRpc_DeviceHandle dev)
{
	dev->httpServer.ethernetSocketConfig = dev->config.ethernetSocketConfig;
	dev->httpServer.socketNumber = dev->config.socketNumber;
	dev->httpServer.port = dev->config.port;

	dev->httpServer.performingCallback = HttpRpc_performingRequest;
	dev->httpServer.appDevice = dev;

	return HttpServer_open(&(dev->httpServer));
}

void HttpRpc_poll (HttpRpc_DeviceHandle dev)
{
	HttpServer_poll(&(dev->httpServer));
}

HttpRpc_Error HttpRpc_getHandler(HttpRpc_DeviceHandle dev,
                                 HttpServer_MessageHandle message,
                                 uint8_t clientNumber)
{
    char tokenCharacter[4] = {'/','\0','0','\0'};
    char* token;
    uint8_t i = 0;
    uint8_t ruleNumber = 0;
    uint8_t functionNumber = 0;
    uint16_t rpcCommandArgumentsIndex = 0;
    uint16_t argumentLength = 0;

    token = strtok(message->uri,
                   tokenCharacter);
    // check if a rule match the rpc command arrived
    for (i = 0; i<HTTPRPC_RULES_MAX_NUMBER; i++)
    {
        if (dev->rules[i].ruleClass[0] != '\0')
        {
            uint8_t stringIsMatching = strcmp (token,
                                               dev->rules[i].ruleClass);
            if (stringIsMatching == 0)
            {
                ruleNumber = i;
                break;
            }
            else if ((stringIsMatching != 0) && (i == HTTPRPC_RULES_MAX_NUMBER-1))
            {
                //RPC command class definitively not recognize
                message->responseCode = HTTPSERVER_RESPONSECODE_BADREQUEST;
                return HTTPRPC_ERROR_RPC_COMMAND_NOT_RECOGNIZE;
            }
            else
            {
                continue;
            }
        }
    }

    // next tokens characters MUST be ' '
    tokenCharacter[0] = '%';
    tokenCharacter[1] = '2';
    tokenCharacter[2] = '0';
    token = strtok(NULL,
                   tokenCharacter);

    for (i = 0; i < HTTPRPC_MAX_FUNCTION_NUMBER; i++)
    {
        if (dev->rules[ruleNumber].ruleFunctions[i].function != '\0')
        {
            uint8_t stringIsMatching = strcmp (token,
                                               dev->rules[ruleNumber].ruleFunctions[i].function);
            if (stringIsMatching == 0)
            {
                functionNumber = i;
                break;
            }
            else if ((stringIsMatching != 0) && (i == HTTPRPC_MAX_FUNCTION_NUMBER-1))
            {
                //RPC command class definitively not recognize
                message->responseCode = HTTPSERVER_RESPONSECODE_BADREQUEST;
                return HTTPRPC_ERROR_RPC_COMMAND_NOT_RECOGNIZE;
            }
            else
            {
                continue;
            }
        }
    }

    for (i = 0; i < HTTPRPC_MAX_ARGUMENT_NUMBER; i++)
    {
        // Take every parameter and put it in the argument string
        token = strtok(NULL, tokenCharacter);
        argumentLength = strlen(token);
        if ((argumentLength + rpcCommandArgumentsIndex) < HTTPRPC_MAX_ARGUMENTS_LENGTH)
        {
            // Check if the parameters are finished
            if (token==NULL) break;
            // Put the parameter in the argument string
            strncpy(&(dev->rpcCommandArguments[rpcCommandArgumentsIndex]),
                    token,
                    argumentLength);
            rpcCommandArgumentsIndex += argumentLength;
            dev->rpcCommandArguments[rpcCommandArgumentsIndex] = ' ';
            rpcCommandArgumentsIndex += 1;
        }

        else
        {
            // Rpc command is too large
            message->responseCode = HTTPSERVER_RESPONSECODE_REQUESTENTITYTOOLARGE;
            return HTTPRPC_ERROR_RPC_COMMAND_TOO_LONG;

        }
    }
    // Performing the callback
    dev->rules[ruleNumber].ruleFunctions[functionNumber].applicationCallback(dev->rules[ruleNumber].applicationDev,
                                                         dev->rpcCommandArguments,
                                                         dev->rpcJsonResult);

#ifdef OHILAB_HTTPSERVER_DEBUG
    Cli_sendMessage("HttpRpc_getHandler:",
                    dev->rpcJsonResult,
                    CLI_MESSAGETYPE_INFO);
#endif

    // Everything gone well
    message->responseCode = HTTPSERVER_RESPONSECODE_OK;

    //building the body of the response
    strncpy(dev->rpcBodyResponse,"{\"result\": ",11);
    strncpy(&dev->rpcBodyResponse[strlen(dev->rpcBodyResponse)],
            dev->rpcJsonResult,
            strlen(dev->rpcJsonResult));
    strncpy(&dev->rpcBodyResponse[strlen(dev->rpcBodyResponse)],
            ", \"error\": 0, ",
            14);
    strncpy(&dev->rpcBodyResponse[strlen(dev->rpcBodyResponse)],
            "\"id\":",
            5);
    sprintf(&dev->rpcBodyResponse[strlen(dev->rpcBodyResponse)],
            "%d}\0",
            clientNumber);
    strncpy(message->body,
            dev->rpcBodyResponse,
            strlen(dev->rpcBodyResponse));

    //building the headers of the response
    strncpy(message->header,"Content-type: application/jsonRpc\r\n",36);
    strncpy(&message->header[strlen(message->header)],
            "Content-length: ",
            17);
    sprintf(&message->header[strlen(message->header)],
           "%d\r\n",strlen(message->body));
    strncpy(&message->header[strlen(message->header)],
            "Accept: application/jsonRpc",
            28);

    memset(dev->rpcBodyResponse,
                           0,
                           sizeof(dev->rpcBodyResponse));
    memset(dev->rpcCommandArguments,
                           0,
                           sizeof(dev->rpcCommandArguments));
    memset(dev->rpcJsonResult,
                           0,
                           sizeof(dev->rpcJsonResult));

    return HTTPRPC_ERROR_OK;
}


HttpRpc_Error HttpRpc_addRule(HttpRpc_DeviceHandle dev,
                              void* applicationDev,
                              char* class,
                              char* function,
                              void (ruleCallback)(void* appDev,
                                                  char* argument,
                                                  char* result))
{
    uint8_t stringIsMatching;
    uint8_t i;
    uint8_t ruleNumber;

    if(dev->classCounter == 0)
    {
        strncpy(dev->rules[0].ruleClass, class, strlen(class)+1);
        dev->rules[0].applicationDev = applicationDev;
        strncpy(dev->rules[0].ruleFunctions[dev->rules[0].functionCounter].function,
                function,
                strlen(function)+1);
        dev->rules[0].ruleFunctions[0].applicationCallback = ruleCallback;
        dev->classCounter++ ;
        dev->rules[0].functionCounter;
        return HTTPRPC_ERROR_OK;

    }

    if(dev->classCounter < HTTPRPC_RULES_MAX_NUMBER)
    {
        // check if a rule match the rpc command arrived
        for (i = 0; i<HTTPRPC_RULES_MAX_NUMBER; i++)
        {
            if (dev->rules[i].ruleClass[0] != '\0')
            {
                uint8_t stringIsMatching = strcmp (class,
                                                   dev->rules[i].ruleClass);
                if (stringIsMatching == 0)
                {
                    // A class is matched
                    if(dev->rules[i].functionCounter < HTTPRPC_MAX_FUNCTION_NUMBER)
                    {
                        strncpy(dev->rules[i].ruleFunctions[dev->rules[i].functionCounter].function,
                                function,
                                strlen(function)+1);
                        dev->rules[i].ruleFunctions[dev->rules[i].functionCounter].applicationCallback = ruleCallback;
                        dev->rules[i].functionCounter++;
                        return HTTPRPC_ERROR_OK;
                    }
                    break;
                }
                else if ((stringIsMatching != 0) && (i == HTTPRPC_RULES_MAX_NUMBER-1))
                {
                    break;
                }
                else
                {
                    continue;
                }
            }
        }

            strncpy(dev->rules[dev->classCounter].ruleClass, class, strlen(class)+1);
            dev->rules[dev->classCounter].applicationDev = applicationDev;
            strncpy(dev->rules[dev->classCounter].ruleFunctions[0].function,
                    function,
                    strlen(function)+1);
            dev->rules[dev->classCounter].ruleFunctions[0].applicationCallback = ruleCallback;
            dev->rules[dev->classCounter].functionCounter++;
            dev->classCounter++ ;

            return HTTPRPC_ERROR_OK;
        }
        return HTTPRPC_ERROR_RULES_ARRAY_IS_FULL;

}

