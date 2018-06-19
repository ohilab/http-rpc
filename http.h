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

#ifndef __OHILAB_HTTP_H
#define __OHILAB_HTTP_H

#define HTTP_MAX_URI_LENGTH            200

#define HTTP_STRING_REQUEST_GET        "GET"
#define HTTP_STRING_REQUEST_POST       "POST"
#define HTTP_STRING_REQUEST_PUT        "PUT"
#define HTTP_STRING_REQUEST_OPTIONS    "OPTIONS"
#define HTTP_STRING_REQUEST_HEAD       "HEAD"
#define HTTP_STRING_REQUEST_DELETE     "DELETE"
#define HTTP_STRING_REQUEST_TRACE      "TRACE"
#define HTTP_STRING_REQUEST_CONNECT    "CONNECT"
typedef enum
{
    HTTPREQUEST_GET,
    HTTPREQUEST_POST,
    HTTPREQUEST_PUT,
    HTTPREQUEST_OPTIONS,
    HTTPREQUEST_HEAD,
    HTTPREQUEST_DELETE,
    HTTPREQUEST_TRACE,
    HTTPREQUEST_CONNECT,

} Http_Request;

#define HTTP_STRING_VERSION_1_0        "HTTP/1.0"
#define HTTP_STRING_VERSION_1_1        "HTTP/1.1"

typedef enum
{
    HTTPVERSION_1_0,
    HTTPVERSION_1_1,

} Http_Version;

typedef struct _Http_Message
{
    Http_Request request;            /**< Specifies the request type received */
    Http_Version version;       /**< Contains the version requested by client */

    char uri[HTTP_MAX_URI_LENGTH];   /**< The uri associated with the request */

} Http_Message;

/**
 * @see https://www.w3.org/Protocols/rfc2616/rfc2616-sec10.html
 */
typedef enum
{
    HTTPRESPONSECODE_OK = 200,                            /**< Successful: OK */
    HTTPRESPONSECODE_BADREQUEST = 400,         /**< Client Error: Bad Request */
    HTTPRESPONSECODE_UNAUTHORIZED = 401,      /**< Client Error: Unauthorized */
    HTTPRESPONSECODE_FORBIDDEN = 403,            /**< Client Error: Forbidden */
    HTTPRESPONSECODE_NOTFOUND = 404,             /**< Client Error: Not found */
} Http_ResponseCode;


#endif // __OHILAB_HTTP_H
