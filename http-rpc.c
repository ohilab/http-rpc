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

#include "http-rpc.h"
#include "http.h"

#ifdef OHILAB_HTTPRPC_DEBUG
#include "cli/cli.h"
#endif

#define HTTPRPC_BUFFER_DIMENSION 0x3FF

typedef uint32_t (*HttpRpc_CurrentTick) (void);
typedef void (*HttpRpc_Delay) (uint32_t);

static HttpRpc_CurrentTick HttpRpc_currentTick;
static HttpRpc_Delay HttpRpc_delay;

static uint8_t HttpRpc_socketNumber = 0;

static char HttpRpc_buffer[256];

typedef struct _HttpRpc_Client
{
    uint8_t rxBuffer[HTTPRPC_BUFFER_DIMENSION+1];
    uint16_t rxIndex;
    Http_Message message;

} HttpRpc_Client;

static HttpRpc_Client HttpRpc_clients [ETHERNET_MAX_LISTEN_CLIENT];

HttpRpc_Error HttpRpc_open (uint16_t port, uint8_t number, EthernetSocket_Config* config)
{
	// Check if the port is valid
	if (port == 0)
		return HTTPRPC_ERROR_WRONG_PORT;

    // Check if the socket exist
    if (number >= ETHERNET_MAX_SOCKET_SERVER)
        return HTTPRPC_ERROR_WRONG_SOCKET_NUMBER;
    // Save number
    HttpRpc_socketNumber = number;

    // Check if user want configure the server
    if (config != 0)
    {
    	EthernetServerSocket_init(config);
    	// Save callback functions for timing
    	HttpRpc_currentTick = config->currentTick;
    	HttpRpc_delay = config->delay;
    }

    // Open the server socket
    if (EthernetServerSocket_connect(number,port) != ETHERNETSOCKET_ERROR_OK)
    	return HTTPRPC_ERROR_OPEN_FAIL;

    // Reset all buffer
    for (uint8_t i = 0; i < ETHERNET_MAX_LISTEN_CLIENT; ++i)
    	HttpRpc_clients[i].rxIndex = 0;

    return HTTPRPC_ERROR_OK;
}

HttpRpc_Error HttpRpc_addRule (const char* rule, void (*callback)(void))
{

}

static HttpRpc_Error HttpRpc_getLine (char* buffer, uint16_t maxLength, uint16_t timeout, uint8_t client, int16_t* received)
{
	int16_t i = 0, count = 0;
	*received = -1; // Default

    if ((buffer == NULL) || (maxLength == 0))
        return HTTPRPC_ERROR_WRONG_PARAM;

    uint32_t nextTimeout = HttpRpc_currentTick() + timeout;
    while ((nextTimeout > HttpRpc_currentTick()) && (i < maxLength))
    {
        if (EthernetServerSocket_available(HttpRpc_socketNumber,client) > 0)
    	{
    		EthernetServerSocket_read(HttpRpc_socketNumber,client,&buffer[i]);

            if (buffer[i] == '\n')
            {
                // Clear last char
                buffer[i] == '\0';
                --i;
                break;
            }
            i++;
    	}
    }

    // Check if timeout occur
    if (nextTimeout < HttpRpc_currentTick())
    {
        *received = -1;
        return HTTPRPC_ERROR_TIMEOUT;
    }

    // Clear '\r'
    if ((i > 0) && (buffer[i] == '\r'))
    {
        buffer[i] = '\0';
        i--;
    }

    // Empty line
    if (i == 0)
    {
        *received = -2;
        return HTTPRPC_ERROR_OK;
    }
    *received = i;
    return HTTPRPC_ERROR_OK;
}

static HttpRpc_Error HttpRpc_parseHeader (char* buffer, uint16_t length, uint8_t client)
{
    char tmp[256];
    uint8_t numArgs = 0;
    uint16_t argCounter = 0;

    // Add end string to the last char
    buffer[length] = '\0';
    // Increase the length to detect and parse the last char
    length++;

    if (client >= ETHERNET_MAX_LISTEN_CLIENT) return HTTPRPC_ERROR_WRONG_CLIENT_NUMBER;

    for (uint16_t i = 0; i < length; ++i)
    {
        if ((buffer[i] == ' ') || (buffer[i] == 0))
        {
            tmp[argCounter] = 0;
            if (numArgs == 0) // Choose request type
            {
                if (strcmp(tmp,HTTP_STRING_REQUEST_GET) == 0)
                {
                    HttpRpc_clients[client].message.request = HTTPREQUEST_GET;
                }
                else if (strcmp(tmp,HTTP_STRING_REQUEST_POST) == 0)
                {
                    HttpRpc_clients[client].message.request = HTTPREQUEST_POST;
                }
                else
                {
                    return HTTPRPC_ERROR_WRONG_REQUEST_FORMAT;
                }
            }
            else if (numArgs == 1) // Uri
            {
                strcpy(HttpRpc_clients[client].message.uri,tmp);
            }
            else if (numArgs == 2) // HTTP Version
            {
                if (strcmp(tmp,HTTP_STRING_VERSION_1_0) == 0)
                {
                    HttpRpc_clients[client].message.version = HTTPVERSION_1_0;
                }
                else if (strcmp(tmp,HTTP_STRING_VERSION_1_1) == 0)
                {
                    HttpRpc_clients[client].message.version = HTTPVERSION_1_1;
                }
                else
                {
                    return HTTPRPC_ERROR_WRONG_REQUEST_FORMAT;
                }
            }
            else if (numArgs > 2)
            {
                return HTTPRPC_ERROR_WRONG_REQUEST_FORMAT;
            }
            // Reset counter for the next argument
            argCounter = 0;
            // Increment argument numbers
            numArgs++;
            continue;
        }
        tmp[argCounter] = buffer[i];
        argCounter++;
    }
    return HTTPRPC_ERROR_OK;
}

static void HttpRpc_sendError (Http_ResponseCode code, uint8_t client)
{
    uint16_t wrote = 0;
    sprintf(HttpRpc_buffer, "HTTP/1.1 %d Error\r\nContent-Length: 0\r\nServer: OHILab\r\n\n\r", code);
    EthernetServerSocket_writeBytes(HttpRpc_socketNumber,client,HttpRpc_buffer,strlen(HttpRpc_buffer),&wrote);
}

void HttpRpc_poll (void)
{
	uint8_t data;
	int16_t received = 0;
	uint16_t wrote = 0;

	HttpRpc_Error error = HTTPRPC_ERROR_OK;

    for (uint8_t i = 0; i < ETHERNET_MAX_LISTEN_CLIENT; ++i)
    {
        // When a client is not connected, jump to the next
        if (!EthernetServerSocket_isConnected(HttpRpc_socketNumber,i))
            continue;

        // Clear index
        received = 0;
        // Get first line
        error = HttpRpc_getLine(HttpRpc_buffer,255,3000,i,&received);
        // When there isn't message, jump to the next
        if (received < 0)
            continue;

        // Parse the header
        error = HttpRpc_parseHeader(HttpRpc_buffer,strlen(HttpRpc_buffer),i);
        if (error != HTTPRPC_ERROR_OK)
        {
#ifdef OHILAB_HTTPRPC_DEBUG
            Cli_sendMessage("HTTP-RPC","Request header not correct",CLI_MESSAGETYPE_WARNING);
#endif
            // FIXME: sendResponse?
            // Jump to the next client
            continue;
        }

#ifdef OHILAB_HTTPRPC_DEBUG
        Cli_sendMessage("HTTP-RPC","Parsing the request body...",CLI_MESSAGETYPE_INFO);
#endif
        // The request header was received
        do
        {
            HttpRpc_getLine(HttpRpc_buffer,255,3000,i,&received);
            //  If we received an empty line, this would indicate the end of the message
            if (received < 0)
            {
                // Performing the request
#ifdef OHILAB_HTTPRPC_DEBUG
                Cli_sendMessage("HTTP-RPC","Request message was received",CLI_MESSAGETYPE_INFO);
#endif
                // Just for test
                HttpRpc_sendError(404,i);
                break;
            }
            // Otherwise parse the message params
#ifdef OHILAB_HTTPRPC_DEBUG
            Cli_sendMessage("HTTP-RPC",HttpRpc_buffer,CLI_MESSAGETYPE_INFO);
#endif

        } while (received > 0);
    }
}
