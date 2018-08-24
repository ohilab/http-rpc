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
 * This library is developed in order to manage a simple
 * HTTP RPC command server using the @a libohiboard, @a timer,
 * @ethernet-serversocket, @a http-server libraries.
 *
 * @section changelog ChangeLog
 *
 * @li v1.0.0 of 2018/08/24 - First release
 *
 * @section library External Library
 *
 * The library use the following external library
 * @li libohiboard https://github.com/ohilab/libohiboard a C framework for
 * NXP Kinetis microcontroller
 * @li timer https://github.com/warcomeb/timer a C library to
 * create a timer based on PIT module of libohiboard
 * @li KSX8081RNA https://github.com/Loccioni-Electronic/KSZ8081RNA
 * @li ethernet-serversocket https://github.com/ohilab/ethernet-socket a C
 * library to manage client/server socket
 * @li CLI https://github.com/Loccioni-Electronic/cli
 * @li http-server https://github.com/ohilab/http-server a C library
 * to create a simple http server and manage http request
 *
 * @section Example
 * before starting with the example, which consist only of
 * the main.c file you MUST create <BR>
 * a board.h file in ~/httpRpc_example/Includes/board.h
 * which contains all macros in order to improve
 * code legibility.
 * <BR>It could be something
 * like this:
 *
 * @code
 * #include "libohiboard.h"
 *
 *  //macros for timer module
 *  #define WARCOMEB_TIMER_NUMBER        0
 *  #define WARCOMEB_TIMER_FREQUENCY     1000
 *  #define WARCOMEB_TIMER_CALLBACK      5
 *
 *  //macros for ethernet-serversocket module
 *  #define ETHERNET_MAX_LISTEN_CLIENT 5
 *  #define ETHERNET_MAX_SOCKET_BUFFER 1023
 *  #define ETHERNET_MAX_SOCKET_CLIENT 5
 *  #define ETHERNET_MAX_SOCKET_SERVER 5
 *
 *  //macros for http-server module
 *  #define HTTPSERVER_MAX_URI_LENGTH           99
 *  #define HTTPSERVER_HEADERS_MAX_LENGTH       1023
 *  #define HTTPSERVER_BODY_MAX_LENGTH          127
 *  #define HTTPSERVER_RX_BUFFER_DIMENSION      255
 *  #define HTTPSERVER_TX_BUFFER_DIMENSION      255
 *  #define HTTPSERVER_TIMEOUT                  3000
 *  //This macro must be define only if you want to test the http-server module
 *  //and always receive a Bad Request respose.
 *  //#define OHILAB_HTTPSERVER_MODULE_TEST       1
 *
 *  //macros for http-rpc module
 *  #define HTTPRPC_RULES_MAX_NUMBER            5
 *  #define HTTPRPC_MAX_RULE_CLASS_LENGTH       32
 *  #define HTTPRPC_MAX_RULE_FUNCTION_LENGTH    32
 *  #define HTTPRPC_MAX_ARGUMENTS_LENGTH        255
 *  #define HTTPRPC_MAX_ARGUMENT_NUMBER         5
 *  #define HTTPRPC_MAX_RULE_CLASS_LENGTH       32
 *  #define HTTPRPC_MAX_RULE_FUNCTION_LENGTH    32
 *
 *  //macros for CLI module
 *  #define PROJECT_NAME "iot-node_frdmK64"
 *  #define PROJECT_COPYRIGTH "Copyright (C) 2018 AEA s.r.l. Loccioni Group - Elctronic Design Dept."
 *  #define FW_TIME_VERSION 0
 *  #define FW_VERSION_STRING "0.3"
 *  #define PCB_VERSION_STRING "1.0"
 *  #define LOCCIONI_CLI_DEV        OB_UART0
 *  #define LOCCIONI_CLI_RX_PIN     UART_PINS_PTB16
 *  #define LOCCIONI_CLI_TX_PIN     UART_PINS_PTB17
 *  #define LOCCIONI_CLI_BAUDRATE   115200
 *  #define LOCCIONI_CLI_ETHERNET   0
 *
 * @endcode
 * <BR>
 *
 * To perform the test it has been created a new class called RgbLed which is
 * composed by this two file:<BR>
 * rgbLed.h
 * <BR>
 * @code
 * #include "libohiboard.h"
 *
 *  typedef struct _LedRgbDevice
 *  {
 *      Gpio_Pins red;
 *      Gpio_Pins green;
 *      Gpio_Pins blue;
 *
 *  }LedRgb_Device,*LedRgb_DeviceHandle;
 *
 *  void RgbLed_turnRedOn(LedRgb_DeviceHandle dev);
 *  void RgbLed_turnGreenOn(LedRgb_DeviceHandle dev);
 *  void RgbLed_turnBlueOn(LedRgb_DeviceHandle dev);
 *
 *  void RgbLed_turnRedOff(LedRgb_DeviceHandle dev);
 *  void RgbLed_turnGreenOff(LedRgb_DeviceHandle dev);
 *  void RgbLed_turnBlueOff(LedRgb_DeviceHandle dev);
 * @endcode
 * <BR>
 *  and rgbLed.c
 * @code
 * #include "rgbLed.h"
 *  void RgbLed_init(LedRgb_DeviceHandle dev)
 *  {
 *      Gpio_config(dev->red,GPIO_PINS_OUTPUT);
 *      Gpio_config(dev->green,GPIO_PINS_OUTPUT);
 *      Gpio_config(dev->blue,GPIO_PINS_OUTPUT);
 *  }
 *
 *  void RgbLed_turnRedOn(LedRgb_DeviceHandle dev)
 *  {
 *      Gpio_clear(dev->red);
 *  }
 *
 *  void RgbLed_turnGreenOn(LedRgb_DeviceHandle dev)
 *  {
 *      Gpio_clear(dev->green);
 *  }
 *
 *  void RgbLed_turnBlueOn(LedRgb_DeviceHandle dev)
 *  {
 *      Gpio_clear(dev->blue);
 *  }
 *
 *  void RgbLed_turnRedOff(LedRgb_DeviceHandle dev)
 *  {
 *      Gpio_set(dev->red);
 *  }
 *
 *  void RgbLed_turnGreenOff(LedRgb_DeviceHandle dev)
 *  {
 *      Gpio_set(dev->green);
 *  }
 *
 *  void RgbLed_turnBlueOff(LedRgb_DeviceHandle dev)
 *  {
 *      Gpio_set(dev->blue);
 *  }
 * @endcode
 * <BR>
 * and rgbLed.h
 * @code
 * #include "libohiboard.h"
 *
 *  typedef struct _LedRgbDevice
 *  {
 *      Gpio_Pins red;
 *      Gpio_Pins green;
 *      Gpio_Pins blue;
 *
 *  }LedRgb_Device,*LedRgb_DeviceHandle;
 *
 *  void RgbLed_init(LedRgb_DeviceHandle dev);
 *  void RgbLed_turnRedOn(LedRgb_DeviceHandle dev);
 *  void RgbLed_turnGreenOn(LedRgb_DeviceHandle dev);
 *  void RgbLed_turnBlueOn(LedRgb_DeviceHandle dev);
 *
 *  void RgbLed_turnRedOff(LedRgb_DeviceHandle dev);
 *  void RgbLed_turnGreenOff(LedRgb_DeviceHandle dev);
 *  void RgbLed_turnBlueOff(LedRgb_DeviceHandle dev);
 * @endcode
 *  <BR>
 * The main.c is developed only to test this library so you can manage
 * RGB led by sending a GET request like this:
 * <BR>GET 192.168.1.6/LED/accendi%20ON%20ON%20ON HTTP/1.1<BR>
 * where the first ON is for the red LED, the second for the green one and
 * the third for the blue one.<BR>
 * You can choose which one turn on or off by sending the corrisponding ON or OFF string.
 *
 * The main.c could be something like this:
 *
 * @code
 *  //Including all needed libraries
 *  #include "libohiboard.h"
 *  #include "timer/timer.h"
 *  #include "KSZ8081RNA/KSZ8081RNA.h"
 *  #include "cli/cli.h"
 *  #include "http-server/http.server.h"
 *  #include "ethernet-socket/ethernet-serversocket.h"
 *  #include "http-rpc/http-rpc.h"
 *  #include "rgbLed/rgbLed.h"
 *
 *  //declare netif struct type
 *  struct netif nettest;
 *  void ledOnOff(char* ledState);
 *
 *  int main(void)
 *  {
 *      uint32_t fout;
 *      uint32_t foutBus;
 *      Clock_State clockState;
 *
 *      //Declaring EthernetSocket_Config struct
 *      EthernetSocket_Config ethernetSocketConfig=
 *      {
 *          .timeout = 3000,
 *          .delay = Timer_delay,
 *          .currentTick = Timer_currentTick,
 *      };
 *
 *      //Declaring HttpRpc_Device
 *      HttpRpc_Device httpRpc=
 *      {
 *          .port = 80,
 *          .socketNumber = 0,
 *          .ethernetSocketConfig = &ethernetSocketConfig,
 *      };
 *
 *      //Declaring ClockConfig struct
 *      Clock_Config clockConfig =
 *      {
 *          .source         = CLOCK_EXTERNAL,
 *          .fext           = 50000000,
 *          .foutSys        = 120000000,
 *          .busDivider     = 2,
 *          .flexbusDivider = 4,
 *          .flashDivider   = 6,
 *      };
 *
 *      LedRgb_Device led=
 *      {
 *          .red = GPIO_PINS_PTB22,
 *          .green = GPIO_PINS_PTE26,
 *          .blue = GPIO_PINS_PTB21,
 *       };
 *
 *      // Enable Clock
 *      System_Errors  error = Clock_Init(&clockConfig);
 *
 *      // JUST FOR DEBUG
 *      clockState = Clock_getCurrentState();
 *      fout = Clock_getFrequency (CLOCK_SYSTEM);
 *      foutBus = Clock_getFrequency (CLOCK_BUS);
 *
 *      // Enable all ports
 *      SIM_SCGC5 |= SIM_SCGC5_PORTA_MASK |
 *                   SIM_SCGC5_PORTB_MASK |
 *                   SIM_SCGC5_PORTC_MASK |
 *                   SIM_SCGC5_PORTD_MASK |
 *                   SIM_SCGC5_PORTE_MASK;
 *
 *      // Network configurations
 *      Ethernet_NetworkConfig netConfig;
 *
 *      // fill net config
 *      IP4_ADDR(&netConfig.ip,192,168,1,6);
 *      IP4_ADDR(&netConfig.mask,255,255,255,0);
 *      IP4_ADDR(&netConfig.gateway,192,168,1,1);
 *      ETHERNET_MAC_ADDR(&netConfig.mac,0x00,0xCF,0x52,0x35,0x00,0x01);
 *
 *      netConfig.phyCallback           = KSZ8081RNA_init;
 *      netConfig.timerCallback         = Timer_currentTick;
 *      netConfig.netif_link_callback   = 0;
 *      netConfig.netif_status_callback = 0;
 *
 *      //Led pins configuration
 *      RgbLed_init(led);
 *
 *      //The default status of leds is ON, in this way RGB leds
 *
 *      RgbLed_turnRedOff(&led);
 *      RgbLed_turnGreenOff(&led);
 *      RgbLed_turnBlueOff(&led);
 *
 *      //timer initialization
 *      Timer_init();
 *
 *      //Ethernet server socket initialization
 *      Ethernet_networkConfig(&nettest, &netConfig);
 *
 *      //Http RPC initialization
 *      HttpRpc_init(&httpRpc);
 *
 *      //Turn the red LED on, now we can send some HTTP RPC command
 *      RgbLed_turnRedOn(&led);
 *
 *     while(1)
 *     {
 *         sys_check_timeouts();
 *
 *         //CLI polling function
 *         Cli_check();
 *
 *         //HTTP Rpc polling function to detect RPC incoming request
 *         HttpRpc_poll(&httpRpc);
 *
 *      }
 *      return 0;
 *  }
 *
 * void ledOnOff(void* led, char* ledState, char* result)
 * {
 *
 *      const char tokenCharacter[2] = {' ','\0'};
 *      char *token;
 *
 *      LedRgb_DeviceHandle ledP = (LedRgb_DeviceHandle) led;
 *
 *      //get the first token
 *      token = strtok(ledState, tokenCharacter);
 *      if (strcmp(token,"OFF")) RgbLed_turnRedOff(led);
 *      else if (strcmp(token,"ON")) RgbLed_turnRedOn(led);
 *
 *      token = strtok(NULL, tokenCharacter);
 *      if (strcmp(token,"OFF")) RgbLed_turnGreenOff(led);
 *      else if (strcmp(token,"ON")) RgbLed_turnGreenOn(led);
 *
 *      token = strtok(NULL,tokenCharacter);
 *      if (strcmp(token,"OFF")) RgbLed_turnBlueOff(led);
 *      else if (strcmp(token,"ON")) RgbLed_turnBlueOn(led);
 *
 *      strncpy(result,"0",1);
 *
 *  }
 * @endcode
 *
 * @section thanksto Thanks to...
 * @li Marco Giammarini
 * @li Gianluca Calignano
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
#define OHILAB_HTTP_RPC_LIBRARY_TIME        1535124886

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
 * The max number of function per @ref Htt .
 */
#ifndef HTTPRPC_MAX_FUNCTION_NUMBER
#define HTTPRPC_MAX_FUNCTION_NUMBER     3
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
 * The max length of result in json response.
 */
#ifndef HTTPRPC_MAX_JSON_RESULT_LENGTH
#define HTTPRPC_MAX_JSON_RESULT_LENGTH 5
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
    ///Rpc rules array is full
    HTTPRPC_ERROR_RULES_ARRAY_IS_FULL,
} HttpRpc_Error;

typedef struct _HttpRpc_Function
{
    ///The function string which will be compared with the incoming request
    char function[HTTPRPC_MAX_RULE_FUNCTION_LENGTH+1];
        ///The callback which is going to call if the rule is recognized
    void (*applicationCallback)(void* applicationDev,
                                char* argument,
                                char* bodyResponse);
}HttpRpc_Function, *HttpRpc_FunctionHandle;

typedef struct _HttpRpc_Rule
{
    ///The class string which will be compared with the incoming request
    char ruleClass[HTTPRPC_MAX_RULE_CLASS_LENGTH+1];
    ///The array of function per class
    HttpRpc_Function ruleFunctions[HTTPRPC_MAX_FUNCTION_NUMBER];
    ///Rule counter
    uint8_t functionCounter;
    ///The void pointer which will be pass to applicationCallback
    void* applicationDev;

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

    ///The array of rules
    HttpRpc_Rule rules[HTTPRPC_RULES_MAX_NUMBER];
    ///Rule counter
    uint8_t classCounter;
    ///The string where the request argument will be stored
    char rpcCommandArguments[HTTPRPC_MAX_ARGUMENTS_LENGTH+1];
    char rpcJsonResult[HTTPRPC_MAX_JSON_RESULT_LENGTH+1];
    uint8_t clientNumberToResponse;
    char rpcBodyResponse[HTTPSERVER_BODY_MAX_LENGTH+1];

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
void HttpRpc_poll (HttpRpc_DeviceHandle dev);

/**
 * @ingroup httpRpc_functions
 * This is the callback function which is call when a http request arrived
 * @param dev The RPC server pointer where the request arrived
 * @param message the message request arrived
 * @param clientNmber number of the client which sent the request
 * @return HTTPSERVER_ERROR_OK if there is a GET request,
 * HTTPSERVER_ERROR_WRONG_REQUEST_FORMAT if there is an other type of request.
 */
HttpServer_Error HttpRpc_performingRequest(void* dev,
                                           HttpServer_MessageHandle message,
                                           uint8_t clientNumber);
/**
 * @ingroup httpRpc_functions
 * This funcion manages every GET request parsing the URI and comparing
 * it with @ref HttpRpc_Device.rules previously stored in the relative
 * @ref HttpRpc_Device
 * @param dev The RPC server pointer there the request arrived
 * @param message The message which is arrived
 * @param clientNmber number of the client which sent the request
 * @return HTTPRPC_ERROR_OK if everything gone well,
 * HTTPRPC_ERROR_RPC_COMMAND_NOT_RECOGNIZE if the command is not recognize,
 * HTTPRPC_ERROR_RPC_COMMAND_TOO_LONG if the command is too large.
 *
 */
HttpRpc_Error HttpRpc_getHandler(HttpRpc_DeviceHandle dev,
                                 HttpServer_MessageHandle message,
                                 uint8_t clientNumber);

/**
 * @ingroup httpRpc_functions
 * This function adds a @ref HttpRpc_Rule to the @ref HttpRpc_Device.rules
 * array in the @ref HttpRpc_Device desired
 * @param dev The RPC server pointer where a new rule is going to store
 * @param[in] The void pointer which can be particularized with a pointer to
 * an application device strcut
 * @param ruleNumber The rule number in the array of rules
 * @param[in] class The char pointer which is going to use to check if the
 * class rule is matched
 * @param[in] function The char pointer which is going to use to check if the
 * function is matched
 * @param ruleCallback The callback which is going to call if the rule is arrived
 * in a request
 * @return HTTPRPC_ERROR_OK if everything gone well,
 * HTTPRPC_ERROR_RULES_ARRAY_IS_FULL if there are too much rules stored in arrays.
 */
HttpRpc_Error HttpRpc_addRule(HttpRpc_DeviceHandle dev,
                              void* applicationDev,
                              char* class,
                              char* function,
                              void (ruleCallback)(void* appDev,
                                                  char* argument,
                                                  char* result));


#endif // __OHILAB_HTTP_RPC_H
