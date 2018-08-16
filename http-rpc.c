#include "http-rpc.h"
#include "http-server/http-server.h"



//static  HttpServer_Client httpServerClients[ETHERNET_MAX_LISTEN_CLIENT];


HttpServer_Error HttpRpc_performingRequest(void* dev,HttpServer_MessageHandle message)
{
	if(message->request == HTTPSERVER_REQUEST_GET)
	{
	    HttpRpc_getHandler(dev, message);
	}
	else
	{
	    message->responseCode = HTTPSERVER_RESPONSECODE_NOTFOUND;
	}
//    switch(message->request)
//	{
//	case HTTPSERVER_REQUEST_GET:
//	    HttpRpc_getHandler(dev, message);
//	    break;
//	case HTTPSERVER_REQUEST_POST:
//	    HttpRpc_postHandler(message);
//	    break;
//	 case HTTPSERVER_REQUEST_PUT:
//	    HttpRpc_putHandler(message);
//	    break;
//	case HTTPSERVER_REQUEST_OPTIONS:
//	    HttpRpc_optionsHandler(message);
//	    break;
//	case HTTPSERVER_REQUEST_HEAD:
//	    HttpRpc_headHandler(message);
//	    break;
//	case HTTPSERVER_REQUEST_DELETE:
//		HttpRpc_deleteHandler(message);
//	    break;
//	case HTTPSERVER_REQUEST_TRACE:
//		HttpRpc_traceHandler(message);
//	    break;
//	case HTTPSERVER_REQUEST_CONNECT:
//		HttpRpc_connectHandler(message);
//	    break;
//
//	}
}

HttpRpc_Error HttpRpc_init (HttpRpc_DeviceHandle dev)
{
	dev->httpServer.ethernetSocketConfig = dev->config.ethernetSocketConfig;
	dev->httpServer.socketNumber = dev->config.socketNumber;
	dev->httpServer.port = dev->config.port;
	//httpRpc.httpServer = &httpServer;

	HttpRpc_Rule rules[HTTPRPC_RULES_MAX_NUMBER];

	dev->rules = rules;

	dev->httpServer.performingCallback = HttpRpc_performingRequest;

	HttpServer_open(&(dev->httpServer));


}

HttpRpc_Error HttpRpc_poll (HttpRpc_DeviceHandle dev, uint16_t timeout)
{
	HttpServer_poll(&(dev->httpServer), timeout);
}

HttpRpc_Error HttpRpc_getHandler(HttpRpc_DeviceHandle dev, HttpServer_MessageHandle message)
{
    const char tokenCharacter[] = {'=','\0'};
    char* token;
    uint32_t value;

    for(uint8_t i = 0; i<HTTPRPC_RULES_MAX_NUMBER; i++)
    {
        if(dev->rules[i].rule[0] =! '\0')
        {
         if(strcmp (strtok(dev->rules[i].rule, token), message->uri) == 0)
         {
             token = strtok(NULL,tokenCharacter);
             sscanf(token, "%d", &value);
             dev->rules[i].applicationCallback((uint8_t)value);
         }
        }

    }
}

//HttpRpc_Error HttpRpc_postHandler(HttpServer_MessageHandle message)
//{
//    message->responseCode = HTTPSERVER_RESPONSECODE_NOTFOUND;
//}
//
//HttpRpc_Error HttpRpc_headHandler(HttpServer_MessageHandle message)
//{
//    message->responseCode = HTTPSERVER_RESPONSECODE_NOTFOUND;
//}
//HttpRpc_Error HttpRpc_putHandler(HttpServer_MessageHandle message)
//{
//    message->responseCode = HTTPSERVER_RESPONSECODE_NOTFOUND;
//}
//HttpRpc_Error HttpRpc_DeleteHandler(HttpServer_MessageHandle message)
//{
//    message->responseCode = HTTPSERVER_RESPONSECODE_NOTFOUND;
//}
//HttpRpc_Error HttpRpc_traceHandler(HttpServer_MessageHandle message)
//{
//    message->responseCode = HTTPSERVER_RESPONSECODE_NOTFOUND;
//}
//HttpRpc_Error HttpRpc_optionsHandler(HttpServer_MessageHandle message)
//{
//    message->responseCode = HTTPSERVER_RESPONSECODE_NOTFOUND;
//}
//HttpRpc_Error HttpRpc_connectHandler(HttpServer_MessageHandle message)
//{
//    message->responseCode = HTTPSERVER_RESPONSECODE_NOTFOUND;
//}
