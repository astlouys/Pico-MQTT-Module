/* ============================================================================================================================================================= *\
   Pico-MQTT-Example.c
   St-Louys Andre - August 2024
   astlouys@gmail.com
   Revision 05-OCT-2024
   Langage: C
   Version 2.02

   Raspberry Pi Pico example on how to use the Pico-MQTT-Module to integrate the MQTT protocol to a C-Language program.
   From an original software version 1.00 by "ellebi2000" on GitHub

   NOTE:
   THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
   WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
   TIME. AS A RESULT, THE AUTHOR SHALL NOT BE HELD LIABLE FOR ANY DIRECT, 
   INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM
   THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING
   INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCT.


   REVISION HISTORY:
   =================
       MAR-2024 1.00 - Initial release by "ellebi2000" on GitHub
    11-AUG-2024 2.00 - Modified / adapted / translated from Italian by Andre St-Louys.
    16-SEP-2024 2.01 - Add identification of the PicoW' IP address.
    30-SEP-2024 2.02 - Wifi SSID, WiFi password and broker IP address are now read from environment variables by CMakeLists.txt during the build process.
\* ============================================================================================================================================================= */



/* $PAGE */
/* $TITLE=Definitions and macros. */
/* ============================================================================================================================================================= *\
                                                                       Definitions and macros.
\* ============================================================================================================================================================= */



/* $PAGE */
/* $TITLE=Include files. */
/* ============================================================================================================================================================= *\
                                                                          Include files
\* ============================================================================================================================================================= */
#include "baseline.h"
#include "hardware/gpio.h"
#include "hardware/irq.h"
#include "hardware/watchdog.h"
#include "pico/bootrom.h"
#include "pico/cyw43_arch.h"
#include "pico/stdlib.h"
#include "pico/unique_id.h"
#include "stdarg.h"
#include <stdio.h>

#include "Pico-WiFi-Module.h"
#include "Pico-MQTT-Module.h"



/* $PAGE */
/* $TITLE=Global variables declaration / definition. */
/* ============================================================================================================================================================= *\
                                                            Global variables declaration / definition.
\* ============================================================================================================================================================= */



/* $PAGE */
/* $TITLE=Function prototypes. */
/* ============================================================================================================================================================= *\
                                                                     Function prototypes.
\* ============================================================================================================================================================= */
/* Find the specific Pico Identifier string from its Unique ID. */
void get_pico_identifier(UCHAR *PicoUniqueId, UCHAR *PicoIdentifier);

/* Retrieve Pico's Unique ID from its flash IC. */
void get_pico_unique_id(UCHAR *PicoUniqueId);

/* Read a string from stdin. */
void input_string(UCHAR *String);

/* Send data to log file. */
void log_info(UINT LineNumber, const UCHAR *FunctionName, UCHAR *Format, ...);

/* Callback to receive the result for a MQTT connection. */
static void mqtt_connection_cb(mqtt_client_t *LocalClient, void *ExtraArgument, mqtt_connection_status_t Status);

/* Display MQTT client information. */
/// void mqtt_display_client(struct struct_mqtt *StructMQTT);

/* Callback to process the data received from a subscribed topic. */
static void mqtt_incoming_data_cb(void *arg, const UINT8 *Data, UINT16 DataLength, UINT8 flags);

/* Callback to receive a topic for which we did subscribe. */
static void mqtt_incoming_pub_cb(void *arg, const char *Topic, UINT32 tot_len);

/* Callback to receive the response to a publishing on a specific topic. */
static void mqtt_pub_request_cb(void *arg, err_t Result);

/* Callback to receive the response to a subscribe request. */
static void mqtt_sub_request_cb(void *arg, err_t Result);

/* Terminal menu when a CDC USB connection is detected during power up sequence. */
void term_menu(struct struct_mqtt *StructMQTT);

/* Convert a string to lowercase. */
UCHAR *util_to_lower(UCHAR *String);





/* $PAGE */
/* $TITLE=Main program entry point. */
/* ============================================================================================================================================================= *\
                                                                      Main program entry point.
\* ============================================================================================================================================================= */
UINT main()
{
  UCHAR *Dum1Ptr;
  UCHAR  PicoUniqueId[40];    // Pico Unique ID ("serial number") used for MQTT client ID.
  UCHAR  PicoIdentifier[40];  // "human string" to describe / identify the PicoW client device from its Unique Number.
  UCHAR  String[128];

  UINT8 Delay;

  UINT16 Loop1UInt16;
  UINT16 ReturnCode;

  ip_addr_t TestAddress;  // Temp buffer to test entered IP address.

  struct struct_mqtt StructMQTT;
  struct struct_wifi StructWiFi;


  /* --------------------------------------------------------------------------------------------------------------------------- *\
                                                          Initializations.
  \* --------------------------------------------------------------------------------------------------------------------------- */
  stdio_init_all();



  /* --------------------------------------------------------------------------------------------------------------------------- *\
                                               Initialize user-defined WiFi parameters.
  \* --------------------------------------------------------------------------------------------------------------------------- */
  Dum1Ptr = (UCHAR *)&StructWiFi;
  for (Loop1UInt16 = 0; Loop1UInt16 < sizeof(StructWiFi); ++Loop1UInt16)
    Dum1Ptr[Loop1UInt16] = '\0';

  strcpy(StructWiFi.NetworkName,     WIFI_SSID);      // should have been read from an environment variable (see User Guide).
  strcpy(StructWiFi.NetworkPassword, WIFI_PASSWORD);  // should have been read from an environment variable (see User Guide).
  StructWiFi.CountryCode = CYW43_COUNTRY_CANADA;      // see User Guide.



  /* Give some time to start a terminal emulator program. */
  Delay = 0;
  while (stdio_usb_connected() == 0)
  {
    ++Delay;
    sleep_ms(50);  // 50 msec added to current wait time for a USB CDC connection.

    /* If we waited for more than this number of "50 msec" for a USB CDC connection, get out of the loop and continue. */
    if (Delay > 2400) break;
  }



  get_pico_unique_id(PicoUniqueId);
  get_pico_identifier(PicoUniqueId, PicoIdentifier);

  log_info(__LINE__, __func__, "==============================================================================================================\r");
  log_info(__LINE__, __func__, "                                              Pico-MQTT-Example\r");
  log_info(__LINE__, __func__, "                                    Part of the ASTL Smart Home ecosystem.\r");
  log_info(__LINE__, __func__, "                                    Pico unique ID: <%s>.\r", PicoUniqueId);

  log_info(__LINE__, __func__, "");
  for (Loop1UInt16 = 0; Loop1UInt16 < (43 - (strlen(PicoIdentifier) / 2)); ++Loop1UInt16)
    printf(" ");
  printf("Device identifier: <%s>.\r", PicoIdentifier);

  log_info(__LINE__, __func__, "==============================================================================================================\r");
  log_info(__LINE__, __func__, "Main program entry point (Delay: %u msec waiting for USB CDC connection).\r", (Delay * 50));



  /* --------------------------------------------------------------------------------------------------------------------------- *\
                                                Initialize CYW43 driver and lwIP stack.
  \* --------------------------------------------------------------------------------------------------------------------------- */
  log_info(__LINE__, __func__, "Before initializing cyw43.\r", __LINE__);
  wifi_init(&StructWiFi);



  /* --------------------------------------------------------------------------------------------------------------------------- *\
                                         Give a chance for user to change network name (SSID).
  \* --------------------------------------------------------------------------------------------------------------------------- */
  printf("\r\r");
  log_info(__LINE__, __func__, "Current network name is <%s>\r", StructWiFi.NetworkName);
  log_info(__LINE__, __func__, "Enter new network name or <Enter> to keep current one: ");
  input_string(String);
  if ((String[0] != 0x0D) && (String[0] != 0x1B))
  {
    strcpy(StructWiFi.NetworkName, String);
    log_info(__LINE__, __func__, "Network name has been changed to: <%s>.\r", StructWiFi.NetworkName);
  }
  else
  {
    log_info(__LINE__, __func__, "Network name has not been changed: <%s>.\r", StructWiFi.NetworkName);
  }
  log_info(__LINE__, __func__, "Press <Enter> to continue: ");
  input_string(String);



  /* --------------------------------------------------------------------------------------------------------------------------- *\
                                          Give a chance for user to change network password.
  \* --------------------------------------------------------------------------------------------------------------------------- */
  printf("\r\r");
  log_info(__LINE__, __func__, "Current network password is <%s>\r", StructWiFi.NetworkPassword);
  log_info(__LINE__, __func__, "Enter new network password or <Enter> to keep current one: ");
  input_string(String);
  if ((String[0] != 0x0D) && (String[0] != 0x1B))
  {
    strcpy(StructWiFi.NetworkPassword, String);
    log_info(__LINE__, __func__, "Network password has been changed to: <%s>.\r", StructWiFi.NetworkPassword);
  }
  else
  {
    log_info(__LINE__, __func__, "Network password has not been changed: <%s>.\r", StructWiFi.NetworkPassword);
  }
  log_info(__LINE__, __func__, "Press <Enter> to continue: ");
  input_string(String);



  /* --------------------------------------------------------------------------------------------------------------------------- *\
                                                     Establish WiFi connection.
  \* --------------------------------------------------------------------------------------------------------------------------- */
  printf("\r\r");
  log_info(__LINE__, __func__, "Before establishing WiFi connection.\r\r");
  if (wifi_connect(&StructWiFi))
  {
    log_info(__LINE__, __func__, "Error while trying to establish a WiFi connection (ReturnCode: %d).\r", ReturnCode);
    log_info(__LINE__, __func__, "Aborting Firmware...\r");
    return 1;
  }
  // log_info(__LINE__, __func__, "Wi-Fi connection established successfully.\r");  // already displayed by Wi-Fi module.



  /* --------------------------------------------------------------------------------------------------------------------------- *\
                                             Retrieve and display PicoW's IP address.
  \* --------------------------------------------------------------------------------------------------------------------------- */
  // log_info(__LINE__, __func__, "PicoW IP address is: <%s>\r", ip4addr_ntoa(netif_ip4_addr(netif_default)));  // already displayed by Wi-Fi module.
  ip4addr_aton(ip4addr_ntoa(netif_ip4_addr(netif_default)), &StructWiFi.PicoIPAddress);



  /* --------------------------------------------------------------------------------------------------------------------------- *\
                                                   Initialize MQTT structure.
  \* --------------------------------------------------------------------------------------------------------------------------- */
  mqtt_init(&StructMQTT);



  /* --------------------------------------------------------------------------------------------------------------------------- *\
                                               Initialize user-defined MQTT parameters.
  \* --------------------------------------------------------------------------------------------------------------------------- */
  log_info(__LINE__, __func__, "Before setting up user-defined MQTT client parameters.\r");

  StructMQTT.PicoIPAddress  = StructWiFi.PicoIPAddress;
  strcpy(StructMQTT.PicoUniqueId,   PicoUniqueId);
  strcpy(StructMQTT.PicoIdentifier, PicoIdentifier);


  /* Wipe structure on entry. */
  /// Dum1Ptr = (UCHAR *)&StructMQTT;
  /// for (Loop1UInt16 = 0; Loop1UInt16 < sizeof(StructMQTT); ++Loop1UInt16)
  ///   Dum1Ptr[Loop1UInt16] = '\0';
    
  /* -------------------------------------------------------------------------------------------------- *\
     Setting default MQTT client info parameters. For reference purpose:
     struct mqtt_connect_client_info_t
     {
       const char *client_id;    // client identifier, must be set by caller.
       const char* client_user;  // user name, set to NULL if not used.
       const char* client_pass;  // password, set to NULL if not used.
       u16_t keep_alive;         // keep alive time in seconds, 0 to disable keep alive functionality.
       const char* will_topic;   // will topic, set to NULL if will is not to be used, will_msg, will_qos and will retain are then ignored.
       const char* will_msg;     // will_msg, see will_topic.
       u8_t will_qos;            // will_qos, see will_topic.
       u8_t will_retain;         // will_retain, see will_topic.
       #if LWIP_ALTCP && LWIP_ALTCP_TLS
         struct altcp_tls_config *tls_config;  // TLS configuration for secure connections.
     }
  \* -------------------------------------------------------------------------------------------------- */
  StructMQTT.MqttClientInfo.client_id = StructMQTT.PicoIdentifier;
  StructMQTT.MqttClientInfo.client_user = "pi";
  StructMQTT.MqttClientInfo.client_pass = MQTT_PASSWORD;
  StructMQTT.MqttClientInfo.keep_alive  = 60;  // Keep alive period in seconds.
  StructMQTT.MqttClientInfo.will_topic  = NULL;
  /// StructMQTT.MqttClientInfo.will_msg    = "MQTT Connection terminated";
  StructMQTT.MqttClientInfo.will_msg    = NULL;
  StructMQTT.MqttClientInfo.will_qos    = 0;
  StructMQTT.MqttClientInfo.will_retain = 0;

  strcpy(StructMQTT.Password, MQTT_PASSWORD);


  /* --------------------------------------------------------------------------------------------------------------------------- *\
                                                  Validate default broker IP address.
  \* --------------------------------------------------------------------------------------------------------------------------- */
  printf("\r\r");
  if (!ip4addr_aton(MQTT_BROKER_IP, &StructMQTT.BrokerAddress))
  {
    log_info(__LINE__, __func__, "Invalid MQTT broker IP address: <%s>.\r", ip4addr_ntoa(&StructMQTT.BrokerAddress));
    /* Do not abort the Firmware since it is possible to change the MQTT broker IP address from the terminal menu. */
  }
  else
  {
    log_info(__LINE__, __func__, "MQTT broker IP address seems valid:  <%s>\r", ip4addr_ntoa(&StructMQTT.BrokerAddress));
  }



  /* --------------------------------------------------------------------------------------------------------------------------- *\
                                        Give a chance for user to change MQTT broker IP address.
  \* --------------------------------------------------------------------------------------------------------------------------- */
  log_info(__LINE__, __func__, "Current MQTT broker IP address is:   <%s>\r", ip4addr_ntoa(&StructMQTT.BrokerAddress));
  log_info(__LINE__, __func__, "Enter new IP address for MQTT broker or <Enter> to keep current IP address: ");
  input_string(String);
  if ((String[0] != 0x0D) && (String[0] != 0x1B))
  {
    if (!ip4addr_aton(String, &TestAddress))
    {
      log_info(__LINE__, __func__, "Invalid IP address entered... IP address has not been changed: <%s>.\r", ip4addr_ntoa(&StructMQTT.BrokerAddress));
    }
    else
    {
      ip4addr_aton(String, &StructMQTT.BrokerAddress);
      log_info(__LINE__, __func__, "MQTT server IP address has been set to: <%s>\r", ip4addr_ntoa(&StructMQTT.BrokerAddress));
    }
  }
  else
  {
    log_info(__LINE__, __func__, "No change to MQTT broker IP address: <%s>.\r", ip4addr_ntoa(&StructMQTT.BrokerAddress));
  }
  log_info(__LINE__, __func__, "Press <Enter> to continue: ");
  input_string(String);



  /* --------------------------------------------------------------------------------------------------------------------------- *\
                                             Give a chance for user to change MQTT password.
  \* --------------------------------------------------------------------------------------------------------------------------- */
  printf("\r\r");
  log_info(__LINE__, __func__, "Current MQTT password is:   <%s>\r", StructMQTT.Password);
  log_info(__LINE__, __func__, "Enter new MQTT password or <Enter> to keep current password: ");
  input_string(String);
  if ((String[0] != 0x0D) && (String[0] != 0x1B))
  {
    strcpy(StructMQTT.Password, String);
    log_info(__LINE__, __func__, "MQTT password has been changed to: <%s>.\r", StructMQTT.Password);
  }
  else
  {
    log_info(__LINE__, __func__, "MQTT password has not been changed: <%s>.\r", StructMQTT.Password);
  }
  log_info(__LINE__, __func__, "Press <Enter> to continue: ");
  input_string(String);
  printf("\r\r");



  /// StructMQTT.MqttClientInstance = mqtt_client_new();
  /// if (!StructMQTT.MqttClientInstance)
  /// {
  ///   log_info(__LINE__, __func__, "Error while trying to create an MQTT client instance.\r");
  ///   log_info(__LINE__, __func__, "Aborting Firmware...\r");
  ///   return 1;
  /// }
  /// log_info(__LINE__, __func__, "Successful creation of an MQTT client instance (0x%p).\r\r\r", StructMQTT.MqttClientInstance);

#if 0
#if LWIP_ALTCP
  printf("LWIP_ALTCP = ON\r");
#else
  printf("LWIP_ALTCP = OFF\r");
#endif
#if LWIP_ALTCP_TLS
  printf("LWIP_ALTCP_TLS = ON\r");
#else
  printf("LWIP_ALTCP_TLS = OFF\r");
#endif
#endif  // 0


  /***
  struct mqtt_client_s (mqtt_client_t)
    u16_t cyclic_tick;      // timers and timeouts.
    u16_t keep_alive;       // idem.
    u16_t server_watchdog;  // idem.
    u16_t pkt_id_seq;       // packet identifier generator.
    u16_t inpub_pkt_id;     // packet identifier of pending incoming publish.
    u8_t conn_state;        // connection state
    struct altcp_pcb *conn;
    void *connect_arg;
    mqtt_connection_cb_t connect_cb;        // connection callback 
    struct mqtt_request_t *pend_req_queue;  // pending requests to server.
    struct mqtt_request_t req_list[MQTT_REQ_MAX_IN_FLIGHT];
    void *inpub_arg;
    mqtt_incoming_data_cb_t data_cb;        // incoming data callback
    mqtt_incoming_publish_cb_t pub_cb;
    u32_t msg_idx;                          // input.
    u8_t rx_buffer[MQTT_VAR_HEADER_BUFFER_LEN];
    struct mqtt_ringbuf_t output;           // output ring-buffer
  ***/
  mqtt_set_inpub_callback(StructMQTT.MqttClientInstance, mqtt_incoming_pub_cb, mqtt_incoming_data_cb, &StructMQTT);
  mqtt_display_client(&StructMQTT);
  sleep_ms(300);  // give some time to complete logging before going on...



  /* --------------------------------------------------------------------------------------------------------------------------- *\
                                                       Connecting to MQTT broker
  \* --------------------------------------------------------------------------------------------------------------------------- */
  log_info(__LINE__, __func__, "Before connecting to MQTT broker.\r");
  /***
  err_t mqtt_client_connect(mqtt_client_t *client, const ip_addr_t *ip_addr, u16_t port, mqtt_connection_cb_t cb, void *arg, const struct mqtt_connect_client_info_t *client_info)
  ***/
  ReturnCode = mqtt_client_connect(StructMQTT.MqttClientInstance, &StructMQTT.BrokerAddress, PORT, mqtt_connection_cb, &StructMQTT, &StructMQTT.MqttClientInfo);
  if (ReturnCode != ERR_OK)
  {
    log_info(__LINE__, __func__, "Error while trying to connect to MQTT broker (return code: %d).\r", ReturnCode);
    log_info(__LINE__, __func__, "Aborting Firmware...\r");
    return 1;
  }
  log_info(__LINE__, __func__, "Connection request sent to MQTT broker without error (return code: %d).\r", ReturnCode);
  sleep_ms(400);  // wait for the callback to complete to get a clean log.



  /* --------------------------------------------------------------------------------------------------------------------------- *\
                                                      Loop on the terminal menu.
  \* --------------------------------------------------------------------------------------------------------------------------- */
  while (1)
  {
    term_menu(&StructMQTT);
  }

  return 0;
}





#include "get_pico_identifier.c"
/* $PAGE */
/* $TITLE=get_pico_unique_id() */
/* ============================================================================================================================================================= *\
                                                           Retrieve Pico's Unique ID from the flash IC.
\* ============================================================================================================================================================= */
void get_pico_unique_id(UCHAR *PicoUniqueId)
{
  UINT8 Loop1UInt8;

  pico_unique_board_id_t board_id;


  /* Retrieve Pico Unique ID from its flash memory IC. */
  pico_get_unique_board_id(&board_id);

  /* Build the Unique ID string in hex. */
  PicoUniqueId[0] = '\0';  // initialize as null string on entry.
  for (Loop1UInt8 = 0; Loop1UInt8 < PICO_UNIQUE_BOARD_ID_SIZE_BYTES; ++Loop1UInt8)
  {
    sprintf(&PicoUniqueId[strlen(PicoUniqueId)], "%2.2X", board_id.id[Loop1UInt8]);
    if ((Loop1UInt8 % 2) && (Loop1UInt8 != 7)) sprintf(&PicoUniqueId[strlen(PicoUniqueId)], "-");
  }

  return;
}





/* $PAGE */
/* $TITLE=input_string() */
/* ============================================================================================================================================================= *\
                                                                    Read a string from stdin.
\* ============================================================================================================================================================= */
void input_string(UCHAR *String)
{
  INT8 DataInput;

  UINT8 FlagLocalDebug = FLAG_OFF;
  UINT8 Loop1UInt8;

  UINT32 IdleTimer;


  if (FlagLocalDebug) log_info(__LINE__, __func__, "Entering input_string().\r");

  Loop1UInt8 = 0;
  IdleTimer  = time_us_32();  // initialize time-out timer with current system timer.
  do
  {
    DataInput = getchar_timeout_us(50000);

    switch (DataInput)
    {
      case (PICO_ERROR_TIMEOUT):
      case (0):
#if 0
        /* This section if we want input_string() to return after a timeout wait time. */ 
        if ((time_us_32() - IdleTimer) > 300000000l)
        {
          printf("[%5u] - Input timeout %lu - %lu = %lu!!\r\r\r", __LINE__, time_us_32(), IdleTimer, time_us_32() - IdleTimer);
          String[0]  = 0x1B;  // time-out waiting for a keystroke.
          Loop1UInt8 = 1;     // end-of-string will be added when exiting while loop.
          DataInput  = 0x0D;
        }
#endif  // 0
        continue;
      break;

      case (8):
        /* <Backspace> */
        IdleTimer = time_us_32();  // restart time-out timer.
        if (Loop1UInt8 > 0)
        {
          --Loop1UInt8;
          String[Loop1UInt8] = 0x00;
          printf("%c %c", 0x08, 0x08);  // erase character under the cursor.
        }
      break;

      case (27):
        /* <ESC> */
        IdleTimer = time_us_32();  // restart time-out timer.
        if (Loop1UInt8 == 0)
        {
          String[Loop1UInt8++] = (UCHAR)DataInput;
          String[Loop1UInt8++] = 0x00;
        }
        printf("\r");
      break;

      case (0x0D):
        /* <Enter> */
        IdleTimer = time_us_32();  // restart time-out timer.
        if (Loop1UInt8 == 0)
        {
          String[Loop1UInt8++] = (UCHAR)DataInput;
          String[Loop1UInt8++] = 0x00;
        }
        printf("\r");
      break;

      default:
        IdleTimer = time_us_32();  // restart time-out timer.
        printf("%c", (UCHAR)DataInput);
        String[Loop1UInt8] = (UCHAR)DataInput;
        // printf("Loop1UInt8: %3u   %2.2X - %c\r", Loop1UInt8, DataInput, DataInput);  /// for debugging purposes.
        ++Loop1UInt8;
      break;
    }
    sleep_ms(10);
  } while((Loop1UInt8 < 128) && (DataInput != 0x0D));

  String[Loop1UInt8] = '\0';  // end-of-string
  /// printf("\r\r\r");

  /* Optionally display each character entered. */
  /***
  for (Loop1UInt8 = 0; Loop1UInt8 < 10; ++Loop1UInt8)
    printf("%2u:[%2.2X]   ", Loop1UInt8, String[Loop1UInt8]);
  printf("\r");
  ***/

  if (FlagLocalDebug) log_info(__LINE__, __func__, "Exiting input_string().\r");

  return;
}





/* $PAGE */
/* $TITLE=log_info() */
/* ============================================================================================================================================================= *\
                                                                         Send data to log file.
\* ============================================================================================================================================================= */
void log_info(UINT LineNumber, const UCHAR *FunctionName, UCHAR *Format, ...)
{
  UCHAR Dum1Str[512];
  UCHAR Dum2Str[512];
  UCHAR TimeStamp[128];

  UINT FunctionSize = 25;  // specify space reserved to display function name including the two "[]".
  UINT Loop1UInt;
  UINT StartChar;

  va_list argp;


  /* If there is no terminal connected, bypass the logging. */
  if (!stdio_usb_connected()) return;


  /* Keep an original copy of the data to log. */
  va_start(argp, Format);
  vsnprintf(Dum1Str, sizeof(Dum1Str), Format, argp);
  va_end(argp);
  strcpy(Dum2Str, Dum1Str);  // make a copy to a working variable (Dum2Str).



  /* ----------------------------------------------------------------------------------------------------------------------- *\
                                              Handling of <HOME> special control code.
  \* ----------------------------------------------------------------------------------------------------------------------- */
  /* Trap special control code for <HOME>. Replace "home" by appropriate control characters for "Home" on a VT101. */
  if (strcmp(strlwr(Dum2Str), "home") == 0)
  {
    Dum1Str[0] = 0x1B; // ESC code
    Dum1Str[1] = '[';
    Dum1Str[2] = 'H';
    Dum1Str[3] = 0x00;
  }



  /* ----------------------------------------------------------------------------------------------------------------------- *\
                                              Handling of <CLS> special control code.
  \* ----------------------------------------------------------------------------------------------------------------------- */
  /* Trap special control code for <CLS>. Replace "cls" by appropriate control characters for "Clear screen" on a VT101. */
  if (strcmp(strlwr(Dum2Str), "cls") == 0)  // has been converted to lowercase above.
  {
    Dum1Str[0] = 0x1B; // ESC code
    Dum1Str[1] = '[';
    Dum1Str[2] = '2';
    Dum1Str[3] = 'J';
    Dum1Str[4] = 0x00;
  }



  /* ----------------------------------------------------------------------------------------------------------------------- *\
                                  Displaying source code line number and caller Pico's core number
                                           followed by optional time stamp if available
        then caller's function name (will be truncated if longer than the size specified at the beginning of the function)
                                      finally, the text to be logged as specified in the call.  
  \* ----------------------------------------------------------------------------------------------------------------------- */
  /* If first character is a line feed ('\r'), display the text as is. */
  if (Dum2Str[0] == '\r') printf(Dum1Str);

  /* Print line number and caller Pico's core number. */
  printf("[%5u %u] ", LineNumber, get_core_num());  // if program is longer than 99999 lines of code, replace [%5u] by [%6u]

#if 0
  /* ----------------------------------------------------------------------------------------------------------------------- *\
                                                Optionally display current time stamp.
  \* ----------------------------------------------------------------------------------------------------------------------- */
  if (FlagDS3231Init)
  {
    /* Retrieve current time stamp. */
    ds3231_get_time(&CurrentTime);

    /* Print time stamp. */
    printf("[%2.2d-%s-%2.2d  %2.2d:%2.2d:%2.2d] ", CurrentTime.DayOfMonth, ShortMonth[CurrentTime.Month], (CurrentTime.Year % 1000), CurrentTime.Hour, CurrentTime.Minute, CurrentTime.Second);
  }
  else
  {
    printf("                      ");  // to properly align function name and log text.
  }
#endif  // 0

  /* Send function name to log file and align them all (if function name is too long to fit in the size given, truncate it). */
  sprintf(Dum2Str, "[%s]", FunctionName);

  /* Check if function name is too long for a clean format in the allocated space. */
  if (strlen(Dum2Str) > FunctionSize)
  {
    /// printf("Dum2Str > 27: <%s>\r", Dum2Str);
    Dum2Str[FunctionSize - 3] = '~';   // add a tilde, indicating function name has been truncated.
    Dum2Str[FunctionSize - 2] = ']';   // truncate function name when it is too long.
    Dum2Str[FunctionSize - 1] = '\0';  // end-of-string.
  }

  printf("%s", Dum2Str);  // display caller's function name.
  /***
  printf("Loop %u = ");
  for (Loop1UInt = 0; Loop1UInt < 20; ++Loop1UInt)
    printf("%2u   ", Dum2Str[Loop1UInt]);
  ***/


  /* Pad function name with blanks when it is shorter than specified length. */
  for (Loop1UInt = strlen(FunctionName); Loop1UInt < (FunctionSize - 3); ++Loop1UInt)
    printf(" ");

  printf("- ");

  /* Send data to log file. */
  // uart_write_blocking(uart0, (UINT8 *)LineString, strlen(LineString));
  printf(Dum1Str);  // send to stdout.

  return;
}





/* $PAGE */
/* $TITLE=mqtt_connection_cb() */
/* ============================================================================================================================================================= *\
                                                             Receiving the result for a MQTT connection.
\* ============================================================================================================================================================= */
void mqtt_connection_cb(mqtt_client_t *LocalClient, void *ExtraArgument, mqtt_connection_status_t Status)
{
  UCHAR String[32];


  log_info(__LINE__, __func__, "Entering mqtt_connection_cb().\r");

  /// MQTT_CLIENT_DATA_T* mqtt_client = (MQTT_CLIENT_DATA_T*)arg;

  switch(Status)
  {
    case(MQTT_CONNECT_ACCEPTED):
      /* Connection accepted by MQTT broker. */
      log_info(__LINE__, __func__, "Connection accepted by MQTT broker (Status: %d).\r", Status);

      /* Since the connection has been accepted, register the callback in charge of receiving any topic for which we have registered. */
      /// mqtt_set_inpub_callback(StructMQTT->MqttClientInstance, mqtt_incoming_pub_cb, mqtt_incoming_data_cb, StructMQTT);
    break;

    case(MQTT_CONNECT_REFUSED_PROTOCOL_VERSION):
      /* Connection refused: bad protocol version. */
      log_info(__LINE__, __func__, "Connection FAILED to MQTT broker: bad protocol version (Status: %d).\r", Status);
    break;

    case(MQTT_CONNECT_REFUSED_IDENTIFIER):
      /* Connection refused: refused identifier. */
      log_info(__LINE__, __func__, "Connection FAILED to MQTT broker: bad identifier (Status: %d).\r", Status);
    break;

    case(MQTT_CONNECT_REFUSED_SERVER):
      /* Connection refused: refused server. */
      log_info(__LINE__, __func__, "Connection FAILED to MQTT broker: bad server (Status: %d).\r", Status);
    break;

    case(MQTT_CONNECT_REFUSED_USERNAME_PASS):
      /* Connection refused: refused user credentials. */
      log_info(__LINE__, __func__, "Connection FAILED to MQTT broker: bad user credentials (Status: %d).\r", Status);
    break;

    case(MQTT_CONNECT_REFUSED_NOT_AUTHORIZED_):
      /* Connection refused: refused not authorized. */
      log_info(__LINE__, __func__, "Connection FAILED to MQTT broker: not authorized (Status: %d).\r", Status);
    break;

    case(MQTT_CONNECT_DISCONNECTED):
      /* Connection disconnected. */
      log_info(__LINE__, __func__, "MQTT connection has been disconnected (Status: %d).\r", Status);
    break;

    case(MQTT_CONNECT_TIMEOUT):
      /* Connection timed out. */
      log_info(__LINE__, __func__, "MQTT connection timed out (Status: %d).\r", Status);
    break;

    default:
      /* Undefined return code. */
      log_info(__LINE__, __func__, "MQTT connection problem: undefined return code (%d).\r", Status);
    break;
  }

  log_info(__LINE__, __func__, "Exiting mqtt_connection_cb().\r", __LINE__);

  return;  
}





/* $PAGE */
/* $TITLE=mqtt_incoming_data_cb() */
/* ============================================================================================================================================================= *\
                                                        Processing the data received from a subscribed topic.
\* ============================================================================================================================================================= */
void mqtt_incoming_data_cb(void *arg, const UINT8 *Payload, UINT16 PayloadLength, UINT8 Flags)
{
  printf("\r\r");
  log_info(__LINE__, __func__, "Entering mqtt_incoming_data_cb().\r");

  struct struct_mqtt *mqtt_client = (struct struct_mqtt *)arg;
  strncpy(mqtt_client->Payload, Payload, PayloadLength);
  mqtt_client->PayloadLength = PayloadLength;
  mqtt_client->Payload[PayloadLength] = '\0';

  /* Print the message and topic. */ 
  log_info(__LINE__, __func__, "********** Topic: [%s]   Message: [%s]\r", mqtt_client->Topic, mqtt_client->Payload);

  log_info(__LINE__, __func__, "Exiting mqtt_incoming_data_cb().\r");

  return;
}





/* $PAGE */
/* $TITLE=mqtt_incoming_pub_cb() */
/* ============================================================================================================================================================= *\
                                                               Receiving a topic for which we did subscribe.
\* ============================================================================================================================================================= */
void mqtt_incoming_pub_cb(void *arg, const char *Topic, UINT32 tot_len)
{
  log_info(__LINE__, __func__, "Entering mqtt_incoming_pub_cb().\r");

  struct struct_mqtt * mqtt_client = (struct struct_mqtt *)arg;
  strcpy(mqtt_client->Topic, Topic);

  log_info(__LINE__, __func__, "Exiting mqtt_incoming_pub_cb().\r");

  return;
}





/* $PAGE */
/* $TITLE=mqtt_pub_request_cb() */
/* ============================================================================================================================================================= *\
                                                     Callback triggered when publishing on a specific topic.
\* ============================================================================================================================================================= */
void mqtt_pub_request_cb(void *arg, err_t Result)
{
  struct struct_mqtt *StructMQTT;


  StructMQTT = (struct struct_mqtt *)arg;

  log_info(__LINE__, __func__, "Entering mqtt_pub_request_cb()\r");

  if (!Result)
    log_info(__LINE__, __func__, "Successfully published to Topic: <%s>   Payload: <%s>   (ReturnCode: %d)\r", StructMQTT->Topic, StructMQTT->Payload, Result);
  else
    log_info(__LINE__, __func__, "Error while trying to publish to Topic: <%s>   Payload: <%s>   (ReturnCode: %d)\r", StructMQTT->Topic, StructMQTT->Payload, Result);

  log_info(__LINE__, __func__, "Exiting mqtt_pub_request_cb().\r");

  return;
}





/* $PAGE */
/* $TITLE=mqtt_sub_request_cb() */
/* ============================================================================================================================================================= *\
                                                        Callback in response to a subscribe or unsibscribe request.
\* ============================================================================================================================================================= */
void mqtt_sub_request_cb(void *arg, err_t Result)
{
  struct struct_mqtt *StructMQTT;


  StructMQTT = (struct struct_mqtt *)arg;

  log_info(__LINE__, __func__, "Entering mqtt_sub_request_cb().\r");

  if (Result == 0)
  {
    if (StructMQTT->Payload[0] == '1')
    {
      log_info(__LINE__, __func__, "Successfully subscribed to topic <%s>   (ReturnCode: %d)\r", StructMQTT->Topic, Result);
    }
    else
    {
      log_info(__LINE__, __func__, "Successfully unsubscribed to topic <%s>   (ReturnCode: %d)\r", StructMQTT->Topic, Result);
    }
  }
  else
  {
    if (StructMQTT->Payload[0] == '1')
    {
      log_info(__LINE__, __func__, "Error while trying to subscribe to topic <%s>   (ReturnCode: 0x%X)\r", StructMQTT->Topic, Result);
    }
    else
    {
      log_info(__LINE__, __func__, "Error while trying to unsubscribe to topic <%s>   (ReturnCode: 0x%X)\r", StructMQTT->Topic, Result);
    }
  }

  log_info(__LINE__, __func__, "Exiting mqtt_sub_request_cb().\r");

  return;
}





/* $PAGE */
/* $TITLE=term_menu()) */
/* ============================================================================================================================================================= *\
                                          Terminal menu when a CDC USB connection is detected during power up sequence.
\* ============================================================================================================================================================= */
void term_menu(struct struct_mqtt *StructMQTT)
{
  UCHAR *Dum1UCharPtr;

  UCHAR Payload[256];
  UCHAR String[256];
  UCHAR Topic[256];

  UINT8 Loop1UInt16;
  UINT8 Menu;

  UINT16 Dum1UInt16;
  UINT16 ReturnCode;

  UINT64 Dum1UInt64;

  ip_addr_t TestAddress;  // IP address of the MQTT broker.



  while (1)
  {
    printf("\r\r\r");
    log_info(__LINE__, __func__, "      Terminal menu\r");
    log_info(__LINE__, __func__, " \r");
    log_info(__LINE__, __func__, " 1) - Set the IP address of the MQTT broker.\r");
    log_info(__LINE__, __func__, " 2) - Set MQTT client parameters.\r");
    log_info(__LINE__, __func__, " 3) - Display MQTT client information.\r");
    log_info(__LINE__, __func__, " 4) - MQTT Connect.\r");
    log_info(__LINE__, __func__, " 5) - Subscribe to a MQTT topic.\r");
    log_info(__LINE__, __func__, " 6) - Publish on a MQTT topic.\r");
    log_info(__LINE__, __func__, " 7) - Unsubscribe from a MQTT topic.\r");
    log_info(__LINE__, __func__, " 8) - Disconnect client from MQTT broker.\r");
    log_info(__LINE__, __func__, " 9) - Find memory pattern for a given number.\r");
    log_info(__LINE__, __func__, "88) - Restart the Firmware.\r");
    log_info(__LINE__, __func__, "99) - Switch Pico in upload mode\r");
    log_info(__LINE__, __func__, " \r");
    log_info(__LINE__, __func__, "      Enter your choice: ");
    input_string(String);

    /* If user pressed <Enter> only, loop back to menu. */
    if (String[0] == 0x0D) continue;

    /* If user pressed <ESC>, loop back to menu. */
    if (String[0] == 0x1B)
    {
      String[0] = 0x00;
      printf("\r\r\r");

      return;
    }


    /* User pressed a menu option, execute it. */
    Menu = atoi(String);

    switch(Menu)
    {
       case (1):
        /* Set MQTT broker IP address. */
        printf("\r\r");
        log_info(__LINE__, __func__, "Setting MQTT broker IP address.\r");
        log_info(__LINE__, __func__, "===============================\r");
        log_info(__LINE__, __func__, "Current MQTT broker IP address: %s\r", ip4addr_ntoa(&StructMQTT->BrokerAddress));
        log_info(__LINE__, __func__, "Enter new IP address for MQTT broker or <Enter> to keep current IP address: ");
        input_string(String);
        if ((String[0] != 0x0D) && (String[0] != 0x1B))
        {
          if (!ip4addr_aton(String, &TestAddress))
          {
            log_info(__LINE__, __func__, "Invalid IP address entered... IP address has not been changed.\r");
          }
          else
          {
            ip4addr_aton(String, &StructMQTT->BrokerAddress);
            log_info(__LINE__, __func__, "MQTT server IP address has been set to: <%s>\r", ip4addr_ntoa(&StructMQTT->BrokerAddress));
          }
          printf("\r\r");
        }
        else
        {
          log_info(__LINE__, __func__, "No change to MQTT broker IP address <%s>.\r", ip4addr_ntoa(&StructMQTT->BrokerAddress));
        }
        log_info(__LINE__, __func__, "Press <Enter> to continue: ");
        input_string(String);
      break;

      case (2):
        /* Set MQTT client  parameters. */
        printf("\r\r");
        log_info(__LINE__, __func__, "Set MQTT client parameters.\r");
        log_info(__LINE__, __func__, "===========================\r");
        log_info(__LINE__, __func__, "To be completed\r");
        log_info(__LINE__, __func__, "Press <Enter> to continue: ");
        input_string(String);
        printf("\r\r");
      break;

      case (3):
        /* Display MQTT client information. */
        printf("\r\r");
        log_info(__LINE__, __func__, "Display current MQTT client information.\r");
        mqtt_display_client(StructMQTT);
        log_info(__LINE__, __func__, "Press <Enter> to continue: ");
        input_string(String);
        printf("\r\r");
      break;

      case (4):
        /* MQTT Connect. */
        printf("\r\r");
        log_info(__LINE__, __func__, "Sending <Connect> command to MQTT broker.\r");
        log_info(__LINE__, __func__, "=========================================\r");
        log_info(__LINE__, __func__, "Press <G> to proceed: ");
        input_string(String);
        if ((String[0] == 'G') || (String[0] == 'g'))
        {
          log_info(__LINE__, __func__, "Connecting client to MQTT broker.\r");
          ReturnCode = mqtt_client_connect(StructMQTT->MqttClientInstance, &StructMQTT->BrokerAddress, PORT, mqtt_connection_cb, StructMQTT, &StructMQTT->MqttClientInfo);
          if (ReturnCode != ERR_OK)
            log_info(__LINE__, __func__, "Error 0x%X while trying to connect to MQTT broker.\r", ReturnCode);
          else
            log_info(__LINE__, __func__, "Connection request sent to MQTT broker without error (return code: %d).\r", ReturnCode);

          sleep_ms(800);
        }
        else
        {
          log_info(__LINE__, __func__, "Connect command has NOT been sent to MQTT broket.\r");
        }
        log_info(__LINE__, __func__, "Press <Enter> to continue: ");
        input_string(String);
        printf("\r\r");
      break;

      case (5):
        /* Subscribe to a MQTT topic. */
        printf("\r\r");
        log_info(__LINE__, __func__, "Subscribe to a MQTT topic.\r");
        log_info(__LINE__, __func__, "==========================\r");
        log_info(__LINE__, __func__, "Enter topic to subscribe to: ");
        input_string(Topic);
        log_info(__LINE__, __func__, "Subscribing to topic: <%s>\r", Topic);
        strcpy(StructMQTT->Topic, Topic);
        strcpy(StructMQTT->Payload, "1");  // to indicate <subscribe>.
        /// mqtt_sub_unsub(MqttClient->MqttClientInstance, Topic, 1, mqtt_sub_request_cb, MqttClient, 1);  // QoS is 3rd parameter.
        if (ReturnCode = mqtt_subscribe(StructMQTT->MqttClientInstance, Topic, 1, mqtt_sub_request_cb, StructMQTT) != 0)
        if (ReturnCode)
        {
          log_info(__LINE__, __func__, "Error 0x%X while trying to subscribe to topic <%s>.\r", ReturnCode, Topic);
        }
        else
        {
          log_info(__LINE__, __func__, "Successfully sent subscribe request.\r");
        }
        sleep_ms(100);
        log_info(__LINE__, __func__, "Press <Enter> to continue: ");
        input_string(String);
        printf("\r\r");
      break;

      case (6):
        /* Publish on a specific MQTT topic. */
        printf("\r\r");
        log_info(__LINE__, __func__, "Publish on a specific MQTT topic.\r");
        log_info(__LINE__, __func__, "=================================\r");
        log_info(__LINE__, __func__, "Enter Topic to publish on: ");
        input_string(Topic);
        log_info(__LINE__, __func__, "Enter Payload to publish (on Topic <%s>): ", Topic);
        input_string(Payload);
        strcpy(StructMQTT->Topic, Topic);
        strcpy(StructMQTT->Payload, Payload);
        log_info(__LINE__, __func__, "Publishing on Topic: <%s>   Payload: <%s>\r", Topic, Payload);
        ReturnCode = mqtt_publish(StructMQTT->MqttClientInstance, Topic, Payload, strlen(Payload), 0, 0, mqtt_pub_request_cb, StructMQTT);
        if (ReturnCode)
        {
          log_info(__LINE__, __func__, "Error 0x%X while trying to publish on Topic <%s>   Payload: <%s>.\r", ReturnCode, Topic, Payload);
        }
        sleep_ms(100);
        log_info(__LINE__, __func__, "Press <Enter> to continue: ");
        input_string(String);
        printf("\r\r");
      break;

      case (7):
        /* Unsubscribe from a MQTT topic. */
        printf("\r\r");
        log_info(__LINE__, __func__, "Unsubscribe from a MQTT topic.\r");
        log_info(__LINE__, __func__, "==============================\r");
        log_info(__LINE__, __func__, "Enter topic to unsubscribe from: ");
        input_string(Topic);
        log_info(__LINE__, __func__, "Unsubscribing from topic: <%s>\r", Topic);
        strcpy(StructMQTT->Topic, Topic);
        strcpy(StructMQTT->Payload, "0");  // to indicate <unsubscribe>.
         /// mqtt_sub_unsub(MqttClient->MqttClientInstance, Topic, 1, NULL, arg, 0);
        if ((ReturnCode = mqtt_unsubscribe(StructMQTT->MqttClientInstance, Topic, mqtt_sub_request_cb, StructMQTT)) != 0)
        {
          log_info(__LINE__, __func__, "Error %d while trying to unsubscribe from topic: <%s>\r", ReturnCode, Topic);
          break;
        }
        sleep_ms(100);
        log_info(__LINE__, __func__, "Press <Enter> to continue: ");
        input_string(String);
        printf("\r\r");
      break;

      case (8):
        /* Disconnect client from MQTT broker. */
        printf("\r\r");
        log_info(__LINE__, __func__, "Disconnect client from MQTT broker.\r");
        log_info(__LINE__, __func__, "===================================\r");
        log_info(__LINE__, __func__, "Press <G> to proceed: ");
        input_string(String);
        if ((String[0] == 'G') || (String[0] == 'g'))
        {
          log_info(__LINE__, __func__, "Disconnecting client from MQTT broker.\r");
          mqtt_disconnect(StructMQTT->MqttClientInstance);
        }
        log_info(__LINE__, __func__, "Press <Enter> to continue: ");
        input_string(String);
        printf("\r\r");
      break;

      case (9):
        /* Find memory pattern for a given number. */
        printf("\r\r");
        log_info(__LINE__, __func__, "Find the memory pattern for a given number.\r");
        log_info(__LINE__, __func__, "===========================================\r");
        log_info(__LINE__, __func__, "Enter the number to modelize: ");
        input_string(String);
        Dum1UInt16 = atoi(String);
        Dum1UInt64 = atoll(String);

        Dum1UCharPtr = (UCHAR*)&Dum1UInt16;
        log_info(__LINE__, __func__, "UINT16 (address: %p):\r", Dum1UCharPtr);
        for (Loop1UInt16 = 0; Loop1UInt16 < sizeof(UINT16); ++Loop1UInt16)
        {
          log_info(__LINE__, __func__, "address: %p   0x%2.2X  (decimal: %u)\r", &Dum1UCharPtr[Loop1UInt16], Dum1UCharPtr[Loop1UInt16], Dum1UCharPtr[Loop1UInt16]);
        }
        printf("\r\r\r");

        Dum1UCharPtr = (UCHAR *)&Dum1UInt64;
        log_info(__LINE__, __func__, "UINT64 (address: %p):\r", Dum1UCharPtr);
        for (Loop1UInt16 = 0; Loop1UInt16 < sizeof(UINT64); ++Loop1UInt16)
        {
          log_info(__LINE__, __func__, "Offset: %u   %3u (0x%2.2X)\r", Loop1UInt16, Dum1UCharPtr[Loop1UInt16], Dum1UCharPtr[Loop1UInt16]);
        }
        printf("\r\r\r");
      break;

      case (88):
        /* Restart the Firmware. */
        printf("\r\r");
        log_info(__LINE__, __func__, "Restart the Firmware.\r");
        log_info(__LINE__, __func__, "=====================\r");
        log_info(__LINE__, __func__, "Press <G> to proceed: ");
        input_string(String);
        if ((String[0] == 'G') || (String[0] == 'g'))
        {
          log_info(__LINE__, __func__, "Disconnecting client from MQTT broker.\r");
          mqtt_disconnect(StructMQTT->MqttClientInstance);
          log_info(__LINE__, __func__, "Restarting the Firmware...\r");
          watchdog_enable(1, 1);
        }
        sleep_ms(3000);  // prevent beginning of menu redisplay.
      break;

      case (99):
        /* Switch the Pico in upload mode. */
        printf("\r\r");
        log_info(__LINE__, __func__, "Switch Pico in upload mode.\r");
        log_info(__LINE__, __func__, "===========================\r");
        log_info(__LINE__, __func__, "Press <G> to proceed: ");
        input_string(String);
        if ((String[0] == 'G') || (String[0] == 'g'))
        {
          log_info(__LINE__, __func__, "Disconnecting client from MQTT broker.\r");
          mqtt_disconnect(StructMQTT->MqttClientInstance);
          log_info(__LINE__, __func__, "Setting the Pico in upload mode...\r");
          reset_usb_boot(0l, 0l);
        }
        printf("\r\r");
      break;

      default:
        printf("\r\r");
        log_info(__LINE__, __func__, "Invalid choice... please re-enter [%s]  [%u]\r\r\r\r\r", String, Menu);
        printf("\r\r");
      break;
    }
  }

  return;
}





/* $PAGE */
/* $TITLE=util_to_lower() */
/* ============================================================================================================================================================= *\
                                                              Convert a string to lowercase.
\* ============================================================================================================================================================= */
UCHAR *util_to_lower(UCHAR *String)
{
  UINT8 CharPointer;


  CharPointer = 0;
  while (String[CharPointer])
  {
    if ((String[CharPointer] >= 0x41) && (String[CharPointer] <= 0x5A))
      String[CharPointer] |= 0x20;

    ++CharPointer;
  }

  return String;
}
