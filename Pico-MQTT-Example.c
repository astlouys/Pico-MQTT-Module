/* ============================================================================================================================================================= *\
   Pico-MQTT-Example.c
   St-Louys Andre - August 2024
   astlouys@gmail.com
   https://github.com/astlouys/Pico-MQTT-Module
   Revision 04-JAN-2026
   Langage: C
   Version 2.04

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
    14-JUL-2025 2.03 - Rename function log_info() to log_printf()
                     - Use common functions: get_pico_identifier(), input_string(), log_printf().
                     - Convert all <\r> to <\n>.
    04-JAN-2026 2.04 - Transfer MQTT initialisaton and setup in the function mqtt_check_connection() to make it much easier to implement and support MQTT health status.
\* ============================================================================================================================================================= */



/* $PAGE */
/* $TITLE=List of GPIO usage. */
/* ============================================================================================================================================================= *\
                                                                     List of GPIO usage and pin-out.
\* ============================================================================================================================================================= */
/* ------------------------------------------------------------------------------------------------------------------------------------------------------------- *\
Using Waveshare's 2-inch LCD display and Waveshare's BME280 module.
Pin 01 - GPIO  0  - Reserved for UART0 TX (Pico-ASTL-CallerId modem).
Pin 02 - GPIO  1  - Reserved for UART0 RX (Pico-ASTL-CallerId modem).
Pin 03 - GND      - Ground
Pin 04 - GPIO  2  - Button 2 (on Waveshare's 2-inch LCD display).
Pin 05 - GPIO  3  - Button 3 (on Waveshare's 2-inch LCD display).
Pin 06 - GPIO  4  - Reserved for UART1 TX (Pico-ASTL-SoundServer and Pico-ASTL-SPrinter).
Pin 07 - GPIO  5  - Reserved for UART1 RX (Pico-ASTL-SoundServer and Pico-ASTL-SPrinter).
Pin 08 - GND      - Ground
Pin 09 - GPIO  6  - Reserved for I2C1 SDA (data)  for BME280, LTR559 and other I2C devices.
Pin 10 - GPIO  7  - Reserved for I2C1 SCL (clock) for BME280, LTR559 and other I2C devices.
Pin 11 - GPIO  8  - LCD_DATA_COMMAND - data / command control pin  (on Waveshare's 2-inch LCD display).
Pin 12 - GPIO  9  - LCD_CHIP_SELECT  - chip select pin, active low (on Waveshare's 2-inch LCD display).
Pin 13 - GND      - Ground
Pin 14 - GPIO 10  - LCD_CLOCK        - SPI clock pin               (on Waveshare's 2-inch LCD display).
Pin 15 - GPIO 11  - LCD_DATA         - SPI  data pin               (on Waveshare's 2-inch LCD display).
Pin 16 - GPIO 12  - LCD_RESET        - reset pin, active low       (on Waveshare's 2-inch LCD display).
Pin 17 - GPIO 13  - LCD_BRIGHTNESS   - backlight pwm pin           (on Waveshare's 2-inch LCD display).
Pin 18 - GND      - Ground
Pin 19 - GPIO 14  - Reserved for Pico-ASTL-SoundServer status pin.
Pin 20 - GPIO 15  - Button 0 (on Waveshare's 2-inch LCD display).
Pin 21 - GPIO 16  - Reserved for infrared sensor / receiver (VS1838).
Pin 22 - GPIO 17  - Button 1 (on Waveshare's 2-inch LCD display).
Pin 23 - GND      - Ground
Pin 24 - GPIO 18  - Reserved for indicator LED.
Pin 25 - GPIO 19  - Reserved for tri-color LED (RED).
Pin 26 - GPIO 20  - Reserved for tri-color LED (GREEN).
Pin 27 - GPIO 21  - Reserved for tri-color LED (BLUE).
Pin 28 - GND      - Ground
Pin 29 - GPIO 22  - Reserved for Active or Passive buzzer.
Pin 30 - RUN      - Enable pin for RP2040. May be used to reset the Pico.
Pin 31 - GPIO 26  - Reserved for analog device (ADC0).
Pin 32 - GPIO 27  - Reserved for analog device (ADC1).
Pin 33 - GND      - ADC ground
Pin 34 - GPIO 28  - Reserved for analog device (ADC2).
Pin 35 - ADC Vref - ADC voltage reference.
Pin 36 - 3V3 out  - Regulated 3V3 to feed 3V3 devices (max 300mA).
Pin 37 - 3V3 en   - Connected to the SMPS enable pin and pulled high by a 100K resistor. Put it to Low to turn Off 3V3 to the RP2040.
Pin 38 - GND      - Ground
Pin 39 - VSYS 5V  - 5V out if powered by USB or main system input voltage (1.8V to 5.5V) if powered externally. SMPS will generate 3V3 for the RP2040 and GPIOs.
Pin 40 - VBUS 5V  - 5V input voltage coming from Pico's USB connector.

GPIO 23 - Low for high efficiency (PFM) or High for improved ripple (PWM).
GPIO 24 - VBUS sense. Detect USB power or VBus pin (High if VBUS is present, Low otherwise).
GPIO 25 - Pico's LED (output)
GPIO 29 - ADC3 - VSYS sense. Read VSYS / 3 through resistor divider and Q1.
GPIO 30 - ADC4 - Read Pico's internal temperature sensor.

Test point 1 - Ground.
Test point 2 - USB DM  - USB data negative pin.
Test point 3 - USB DP  - USB data positive pin.
Test point 4 - GPIO 23 - Switched Mode Power Supply (SMPS) Pin. Controls the on-board SMPS power save pin.
Test point 5 - GPIO 25 - Pico's LED.
Test point 6 - Bootsel (upload mode).

SPI port: spi1
\* ------------------------------------------------------------------------------------------------------------------------------------------------------------- */



/* $PAGE */
/* $TITLE=Definitions and macros. */
/* ============================================================================================================================================================= *\
                                                                       Definitions and macros.
\* ============================================================================================================================================================= */
#define CYW43_COUNTRY_CODE  CYW43_COUNTRY_CANADA



/* $PAGE */
/* $TITLE=Include files. */
/* ============================================================================================================================================================= *\
                                                                          Include files
\* ============================================================================================================================================================= */
#include "baseline.h"
#include "hardware/gpio.h"
#include "hardware/irq.h"
#include "hardware/rtc.h"
#include "hardware/watchdog.h"
#include "pico/bootrom.h"
#include "pico/cyw43_arch.h"
#include "pico/multicore.h"
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
UCHAR  PicoUniqueId[40];    // Pico Unique ID ("serial number") used for MQTT client ID.
UCHAR  PicoIdentifier[40];  // "human string" to describe / identify the PicoW client device from its Unique Number.

UINT8 FlagTimeSet = FLAG_OFF;

datetime_t DateTime;

struct struct_mqtt StructMQTT;
struct struct_wifi StructWiFi;

/* Day names. */
UCHAR DayName[7][13] =
{
  {"Sunday"}, {"Monday"}, {"Tuesday"}, {"Wednesday"}, {"Thursday"}, {"Friday"}, {"Saturday"}
};

/* Short month names (3 letters). */
UCHAR ShortMonth[13][4] =
{
  {" "}, {"JAN"}, {"FEB"}, {"MAR"}, {"APR"}, {"MAY"}, {"JUN"}, {"JUL"}, {"AUG"}, {"SEP"}, {"OCT"}, {"NOV"}, {"DEC"}
};



/* $PAGE */
/* $TITLE=Function prototypes. */
/* ============================================================================================================================================================= *\
                                                                     Function prototypes.
\* ============================================================================================================================================================= */
/* Thread to be run on Pico's core 1. */
void core1_loop(void);

/* Find the specific Pico Identifier string from its Unique ID. */
void get_pico_identifier(UCHAR *PicoUniqueId, UCHAR *PicoIdentifier);

/* Read a string from stdin. */
void input_string(UCHAR *String, UINT16 StringSize, UINT32 TimeOutMSec);

/* Display log header. */
void log_header(void);

/* Send data to log file. */
void log_printf(UINT LineNumber, const UCHAR *FunctionName, UCHAR *Format, ...);

/* Callback to receive the result for a MQTT connection. */
static void mqtt_connection_cb(mqtt_client_t *LocalClient, void *ExtraArgument, mqtt_connection_status_t Status);

/* Subscribe to all required MQTT topics for this device. */
void mqtt_device_subscribe(void);

/* Callback to process the data received from a subscribed topic. */
static void mqtt_incoming_data_cb(void *arg, const UINT8 *Data, UINT16 DataLength, UINT8 flags);

/* Callback to receive a topic for which we did subscribe. */
static void mqtt_incoming_publish_cb(void *arg, const char *Topic, UINT32 tot_len);

/* Initialize MQTT client and setup connection with MQTT broker. */
static void mqtt_initialization(void);

/* Callback to receive the response to a publishing on a specific topic. */
static void mqtt_pub_request_cb(void *arg, err_t Result);

/* Callback to receive the response to a subscribe request. */
static void mqtt_sub_request_cb(void *arg, err_t Result);

/* Terminal menu when a CDC USB connection is detected during power up sequence. */
void term_menu(void);





/* $PAGE */
/* $TITLE=Main program entry point. */
/* ============================================================================================================================================================= *\
                                                                      Main program entry point.
\* ============================================================================================================================================================= */
UINT main()
{
#ifdef RELEASE_VERSION
  UINT8 FlagLocalDebug = FLAG_OFF;  // must remain OFF at all time.
#else   // RELEASE_VERSION
  UINT8 FlagLocalDebug = FLAG_OFF;  // may be turned ON for debug purposes.
#endif  // RELEASE_VERSION

  UCHAR  String[128];

  UINT8 Delay;

  UINT16 ReturnCode;
  UINT16 WaitTime;

  UINT64 CurrentTimer;
  UINT64 Last15SecTimer;
  UINT64 Last60SecTimer;

  /// ip_addr_t TestAddress;  // Temp buffer to test entered IP address.


  /* ----------------------------------------------------------------------------------------------------------------------------------------------------------- *\
                                                                          Initializations.
  \* ----------------------------------------------------------------------------------------------------------------------------------------------------------- */
   stdio_init_all();


  /* ----------------------------------------------------------------------------------------------------------------------------------------------------------- *\
                                                                 Initialize user-defined WiFi parameters.
  \* ----------------------------------------------------------------------------------------------------------------------------------------------------------- */
  StructWiFi.CountryCode = CYW43_COUNTRY_CODE;            // see User Guide.



  /* ----------------------------------------------------------------------------------------------------------------------------------------------------------- *\
                                                                      Wait for USB CDC connection.
  \* ----------------------------------------------------------------------------------------------------------------------------------------------------------- */
  log_printf(__LINE__, __func__, "Waiting for a USB CDC connection.\n");  // message will not show up on terminal if connection is not established.

  Delay = 0;
  WaitTime = 50;  // number of msec delay on each pass.
  while (stdio_usb_connected() == 0)
  {
    ++Delay;
    sleep_ms(WaitTime);  // 50 msec added to current wait time for a USB CDC connection.

    /* If we waited for more than this number of seconds for a USB CDC connection, get out of the loop and continue. */
    if (((Delay * WaitTime) / 1000) > 5) break;
  }


  get_pico_identifier(PicoUniqueId, PicoIdentifier);

  log_printf(__LINE__, __func__, "cls");   // clear terminal emulator screen on entry.
  log_printf(__LINE__, __func__, "home");  // "home" terminal emulator cursor on entry.
  log_printf(__LINE__, __func__, "LOG MASK 0x11");  // turn Off date and time in log file on entry.
  log_header();
  log_printf(__LINE__, __func__, "Main program entry point (Delay: %u msec waiting for USB CDC connection).\n", (Delay * 50));


  /* ----------------------------------------------------------------------------------------------------------------------------------------------------------- *\
                                                              Initialize and set Pico's real-time clock.
  \* ----------------------------------------------------------------------------------------------------------------------------------------------------------- */
  /* During development phase, when MQTT Time Server is not available, let's initialize date and time with valid values and proceed quickly with startup sequence. */
  DateTime.dotw  = 1;
  DateTime.day   = 1;
  DateTime.month = 1;
  DateTime.year  = 2026;
  DateTime.hour  = 12;
  DateTime.min   = 00;
  DateTime.sec   = 00;
  /* end of valid and temporary time and date. */

  log_printf(__LINE__, __func__, "Setting Pico's real-time clock:\n");
  log_printf(__LINE__, __func__, "Enter Day-of-week (Sunday = 0   Monday = 1   Tuesday = 2   Wednesday = 3   Thursday = 4   Friday = 5   Saturday = 6): ");
  input_string(String, sizeof(String), 0);
  if (String[0] != 0x0D) DateTime.dotw = atol(String);

  log_printf(__LINE__, __func__, "Enter Day-of-month (1 to 31): ");
  input_string(String, sizeof(String), 0);
  if (String[0] != 0x0D) DateTime.day = atol(String);

  log_printf(__LINE__, __func__, "Enter Month (1 to 12)       : ");
  input_string(String, sizeof(String), 0);
  if (String[0] != 0x0D) DateTime.month = atol(String);

  log_printf(__LINE__, __func__, "Enter Year (ex: 2025)       : ");
  input_string(String, sizeof(String), 0);
  if (String[0] != 0x0D) DateTime.year = atol(String);

  log_printf(__LINE__, __func__, "Enter Hour (0 to 23)        : ");
  input_string(String, sizeof(String), 0);
  if (String[0] != 0x0D) DateTime.hour = atol(String);

  log_printf(__LINE__, __func__, "Enter Minutes (0 to 59)     : ");
  input_string(String, sizeof(String), 0);
  if (String[0] != 0x0D) DateTime.min = atol(String);

  log_printf(__LINE__, __func__, "Enter Seconds (0 to 59)     : ");
  input_string(String, sizeof(String), 0);
  if (String[0] != 0x0D) DateTime.sec = atol(String);

  log_printf(__LINE__, __func__, "Current date and time:   %s   %u-%s-%4.4u   %2.2u:%2.2u:%2.2u\n", 
                               DayName[DateTime.dotw],
                               DateTime.day,  ShortMonth[DateTime.month], DateTime.year,
                               DateTime.hour, DateTime.min,               DateTime.sec);

  rtc_init();                   // initialize Pico's real-time clock.
  rtc_set_datetime(&DateTime);  // set current time on Pico's real-time clock.
  log_printf(__LINE__, __func__, "LOG MASK 0x1D");
  log_printf(__LINE__, __func__, "Now that Pico's real-time clock has been initialized, logged data will be time stamped.\n");


  /* ----------------------------------------------------------------------------------------------------------------------------------------------------------- *\
                                                    Give instructions to user on how to display main terminal menu.
  \* ----------------------------------------------------------------------------------------------------------------------------------------------------------- */
  log_printf(__LINE__, __func__, ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>><<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<\n");
  log_printf(__LINE__, __func__, "                          NOTE: Press <Enter> at any time to display the main terminal menu...\n");
  log_printf(__LINE__, __func__, ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>><<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<\n");


  /* ----------------------------------------------------------------------------------------------------------------------------------------------------------- *\
                                Starting core1 in charge of handling terminal requests (if such a terminal is connected to USB port).
  \* ----------------------------------------------------------------------------------------------------------------------------------------------------------- */
  if (FlagLocalDebug) log_printf(__LINE__, __func__, "Before starting core 1 thread.\n");
  if (stdio_usb_connected())
  {
    /* Start terminal menu only if a terminal has been connected to USB CDC (it SHOULD have been for Pico-ASTL-Control device to be able to read MQTT traffic info !!). */
    multicore_launch_core1(core1_loop);
    sleep_ms(100);  // give some time to log data from core 1 startup sequence.
  }


  /* ----------------------------------------------------------------------------------------------------------------------------------------------------------- *\
                                        Main core 0 forever loop. Will be executed in core 0 context and will loop forever.
  \* ----------------------------------------------------------------------------------------------------------------------------------------------------------- */
  CurrentTimer   = time_us_64();
  Last15SecTimer = 0ll;
  Last60SecTimer = CurrentTimer;

  while (1)
  {
    CurrentTimer = time_us_64();
    watchdog_update();  // kick the watchdog to keep the Firmware alive.
    rtc_get_datetime(&DateTime);
    // log_printf(__LINE__, __func__, "Date: %s %2.2u-%3s-%4.4u   %2.2u:%2.2u:%2.2u\n", DayName[DateTime.dotw], DateTime.day, ShortMonth[DateTime.month], DateTime.year, DateTime.hour, DateTime.min, DateTime.sec);



    /* --------------------------------------------------------------------------------------------------------------------------------------------------------- *\
                                                                       15 seconds tag point.
    \* --------------------------------------------------------------------------------------------------------------------------------------------------------- */
    if (CurrentTimer > (Last15SecTimer + 15000000ll))
    {
      Last15SecTimer = CurrentTimer;


      /* ------------------------------------------------------------------------------------------------------------------------------------------------------- *\
                                                                 Check Wi-Fi (network) connection health.
      \* ------------------------------------------------------------------------------------------------------------------------------------------------------- */
      ReturnCode = wifi_check_connection();
      if (ReturnCode)
      {
        if (FlagLocalDebug) log_printf(__LINE__, __func__, "Problem with Wi-Fi connection...\n");
      }
      else
      {
        if (FlagLocalDebug) log_printf(__LINE__, __func__, "Wi-Fi connection OK.\n");
      }



      /* ------------------------------------------------------------------------------------------------------------------------------------------------------- *\
                                                                         Check MQTT connection health.
      \* ------------------------------------------------------------------------------------------------------------------------------------------------------- */
      ReturnCode = mqtt_check_connection(StructWiFi.FlagHealth);
      switch (ReturnCode)
      {
        case (0):
          if (FlagLocalDebug) log_printf(__LINE__, __func__, "MQTT connection OK.\n\n");
        break;

        case (1):
          log_printf(__LINE__, __func__, "MQTT connection just restored, proceed with mqtt_initialization() and mqtt_device_subscribe().\n\n");
          mqtt_initialization();
          mqtt_device_subscribe();
        break;

        default:
          if (FlagLocalDebug) log_printf(__LINE__, __func__, "Problem with MQTT connection.\n\n");
        break;
      }
    }





    /* --------------------------------------------------------------------------------------------------------------------------------------------------------- *\
                                                                       60 seconds tag point.
    \* --------------------------------------------------------------------------------------------------------------------------------------------------------- */
    if (CurrentTimer > (Last60SecTimer + 60000000ll))
    {
      Last60SecTimer = CurrentTimer;
    }


    sleep_ms(200);  // slow down endless loop to keep Pico relax...
  }

  return 0;
}





/* $PAGE */
/* $TITLE=core1_loop() */
/* ============================================================================================================================================================= *\
                                                              Thread running on core1 - Loop on terminal menu.
\* ============================================================================================================================================================= */
void core1_loop(void)
{
  sleep_ms(300);
  while (1)
  {
    term_menu();
  }

  return;
}


#include "get_pico_identifier.c"


/* $PAGE */
/* $TITLE=log_header() */
/* ============================================================================================================================================================= *\
                                                                     Display log header.
\* ============================================================================================================================================================= */
void log_header(void)
{
  log_printf(__LINE__, __func__, "========================================================================================================================\n");
  log_printf(__LINE__, __func__, "<120>Pico-MQTT-Example\n");
  log_printf(__LINE__, __func__, "<120>Part of the ASTL Smart Home ecosystem.\n");
  log_printf(__LINE__, __func__, "<120>Pico unique ID: <%s>.\n", PicoUniqueId);
  log_printf(__LINE__, __func__, "<120>Device identifier: <%s>\n", PicoIdentifier);
  log_printf(__LINE__, __func__, "========================================================================================================================\n");
  return;
}


#include "input_string.c"

#include "log_printf.c"


/* $PAGE */
/* $TITLE=mqtt_connection_cb() */
/* ============================================================================================================================================================= *\
                                                             Receiving the result for a MQTT connection.
\* ============================================================================================================================================================= */
void mqtt_connection_cb(mqtt_client_t *, void *, mqtt_connection_status_t Status)
{
#ifdef RELEASE_VERSION
  UINT8 FlagLocalDebug = FLAG_OFF;  // must remain OFF at all time.
#else   // RELEASE_VERSION
  UINT8 FlagLocalDebug = FLAG_OFF;  // may be turned ON for debug purposes.
#endif  // RELEASE_VERSION


  if (FlagLocalDebug) log_printf(__LINE__, __func__, "Entering mqtt_connection_cb().\n");

  switch(Status)
  {
    case(MQTT_CONNECT_ACCEPTED):
      /* Connection accepted by MQTT broker. */
      log_printf(__LINE__, __func__, "Connection accepted by MQTT broker (Status: %d).\n", Status);

      /* Since the connection has been accepted, register the callback in charge of receiving any topic for which we have registered. */
      /// mqtt_set_inpub_callback(StructMQTT->MqttClientInstance, mqtt_incoming_publish_cb, mqtt_incoming_data_cb, StructMQTT);
    break;

    case(MQTT_CONNECT_REFUSED_PROTOCOL_VERSION):
      /* Connection refused: bad protocol version. */
      log_printf(__LINE__, __func__, "Connection FAILED to MQTT broker: bad protocol version (Status: %d).\n", Status);
    break;

    case(MQTT_CONNECT_REFUSED_IDENTIFIER):
      /* Connection refused: refused identifier. */
      log_printf(__LINE__, __func__, "Connection FAILED to MQTT broker: bad identifier (Status: %d).\n", Status);
    break;

    case(MQTT_CONNECT_REFUSED_SERVER):
      /* Connection refused: refused server. */
      log_printf(__LINE__, __func__, "Connection FAILED to MQTT broker: bad server (Status: %d).\n", Status);
    break;

    case(MQTT_CONNECT_REFUSED_USERNAME_PASS):
      /* Connection refused: refused user credentials. */
      log_printf(__LINE__, __func__, "Connection FAILED to MQTT broker: bad user credentials (Status: %d).\n", Status);
    break;

    case(MQTT_CONNECT_REFUSED_NOT_AUTHORIZED_):
      /* Connection refused: refused not authorized. */
      log_printf(__LINE__, __func__, "Connection FAILED to MQTT broker: not authorized (Status: %d).\n", Status);
    break;

    case(MQTT_CONNECT_DISCONNECTED):
      /* Connection disconnected. */
      log_printf(__LINE__, __func__, "MQTT connection has been disconnected (Status: %d).\n", Status);
    break;

    case(MQTT_CONNECT_TIMEOUT):
      /* Connection timed out. */
      log_printf(__LINE__, __func__, "MQTT connection timed out (Status: %d).\n", Status);
    break;

    default:
      /* Undefined return code. */
      log_printf(__LINE__, __func__, "MQTT connection problem: undefined return code (%d).\n", Status);
    break;
  }

  if (FlagLocalDebug) log_printf(__LINE__, __func__, "Exiting mqtt_connection_cb().\n", __LINE__);
  printf("\n");

  return;  
}





/* $PAGE */
/* $TITLE=mqtt_device_subscribe() */
/* ============================================================================================================================================================= *\
                                                        Subscribe to all required MQTT topics for this device.
\* ============================================================================================================================================================= */
void mqtt_device_subscribe(void)
{
  UINT8 QoS;

  INT16 ReturnCode;


  /* ----------------------------------------------------------------------------------------------------------------------------------------------------------- *\
                                                       Request current time from ASTL Smart Home MQTT Time Server.
  \* ----------------------------------------------------------------------------------------------------------------------------------------------------------- */
  if ((StructMQTT.FlagHealth == FLAG_ON) && (FlagTimeSet == FLAG_OFF))
  {
    sprintf(StructMQTT.Topic, "TimeServer/TimeRequest/%s", StructMQTT.PicoIdentifier);  // include source of MQTT message as the last topic item.
    ReturnCode = mqtt_publish(StructMQTT.MqttClientInstance, StructMQTT.Topic, StructMQTT.Payload, strlen(StructMQTT.Payload), 0, 0, mqtt_pub_request_cb, &StructMQTT);
    if (ReturnCode)
    {
      log_printf(__LINE__, __func__, "Error 0x%X while trying to publish on Topic <%s>   Payload: <%s>.\n", ReturnCode, StructMQTT.Topic, StructMQTT.Payload);
    }
    else
    {
      log_printf(__LINE__, __func__, "Publish request sent successfully to broker for Topic <%s>   Payload: <%s>.\n", StructMQTT.Topic, StructMQTT.Payload);
    }
    sleep_ms(300);  // wait for response callback to complete to get a clean log and also to get the right "topic being subscribed to" in log file.
  }



  /* ----------------------------------------------------------------------------------------------------------------------------------------------------------- *\
                                                                    Subscribe to specific topics.               
  \* ----------------------------------------------------------------------------------------------------------------------------------------------------------- */
  sprintf(StructMQTT.Topic, "All");
  QoS = 2;
  StructMQTT.FlagSubscribe = FLAG_ON;
  ReturnCode = mqtt_subscribe(StructMQTT.MqttClientInstance, StructMQTT.Topic, QoS, mqtt_sub_request_cb, &StructMQTT);
  if (ReturnCode)
  {
    log_printf(__LINE__, __func__, "Error while sending subscribe request for Topic <%s> (ReturnCode: %d).\n", StructMQTT.Topic, ReturnCode);
  }
  else
  {
    log_printf(__LINE__, __func__, "Subscribe request sent successfully to broker for topic: <%s> (ReturnCode: %d).\n", StructMQTT.Topic, ReturnCode);
  }
  sleep_ms(300);  // wait for response callback to complete to get a clean log and also to get the right "topic being subscribed to" in log file.

  return;
}





/* $PAGE */
/* $TITLE=mqtt_incoming_data_cb() */
/* ============================================================================================================================================================= *\
                                                        Processing the data received from a subscribed topic.
\* ============================================================================================================================================================= */
void mqtt_incoming_data_cb(void *arg, const UINT8 *Payload, UINT16 PayloadLength, UINT8)
{
  printf("\n\n");
  log_printf(__LINE__, __func__, "Entering mqtt_incoming_data_cb().\n");

  struct struct_mqtt *mqtt_client = (struct struct_mqtt *)arg;
  strncpy(mqtt_client->Payload, Payload, PayloadLength);
  mqtt_client->PayloadLength = PayloadLength;
  mqtt_client->Payload[PayloadLength] = '\0';

  /* Print the message and topic. */ 
  log_printf(__LINE__, __func__, "********** Topic: [%s]   Message: [%s]\n", mqtt_client->Topic, mqtt_client->Payload);

  log_printf(__LINE__, __func__, "Exiting mqtt_incoming_data_cb().\n");

  return;
}





/* $PAGE */
/* $TITLE=mqtt_incoming_publish_cb() */
/* ============================================================================================================================================================= *\
                                                               Receiving a topic for which we did subscribe.
\* ============================================================================================================================================================= */
void mqtt_incoming_publish_cb(void *arg, const char *Topic, UINT32)
{
  log_printf(__LINE__, __func__, "Entering mqtt_incoming_publish_cb().\n");

  struct struct_mqtt * mqtt_client = (struct struct_mqtt *)arg;
  strcpy(mqtt_client->Topic, Topic);

  log_printf(__LINE__, __func__, "Exiting mqtt_incoming_publish_cb().\n");

  return;
}





/* $PAGE */
/* $TITLE=mqtt_initialization()) */
/* ============================================================================================================================================================= *\
                                                     Initialize MQTT client and setup connection with MQTT broker.
\* ============================================================================================================================================================= */
static void mqtt_initialization(void)
{
#ifdef RELEASE_VERSION
  UINT8 FlagLocalDebug = FLAG_OFF;  // should be OFF at all times
#else  // RELEASE_VERSION
  UINT8 FlagLocalDebug = FLAG_ON;  // may be turned On for debug purposes.
#endif  // RELEASE_VERSION

  /// UINT8 StepCount;

  INT16 ReturnCode;


  /* ----------------------------------------------------------------------------------------------------------------------------------------------------------- *\
                                                                     Initialize MQTT client.
  \* ----------------------------------------------------------------------------------------------------------------------------------------------------------- */
  if (mqtt_init())
  {
    log_printf(__LINE__, __func__, "Error while trying to create an MQTT client instance.\n");
    log_printf(__LINE__, __func__, "Firmware will restart when progress line reach the end of LCD display...\n");
  }


  strcpy(StructMQTT.Password,  MQTT_PASSWORD);              // MQTT password should have been read from an environment variable (see User Guide).
  ip4addr_aton(MQTT_BROKER_IP, &StructMQTT.BrokerAddress);  // MQTT broker address should have been read from an environment variable (see User Guide).
  StructMQTT.PicoIPAddress = StructWiFi.PicoIPAddress;


  /* ----------------------------------------------------------------------------------------------------------------------------------------------------------- *\
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
  \* ----------------------------------------------------------------------------------------------------------------------------------------------------------- */
 

  if (FlagLocalDebug) log_printf(__LINE__, __func__, "Setting up <MQTT client info> parameters.\n");
  // NOTE: QoS (Quality of Service):
  //       QoS = 0 -> Message received at most once.
  //       QoS = 1 -> Message received at least once.
  //       QoS = 2 -> Message received exactly once.
  StructMQTT.MqttClientInfo.client_id   = StructMQTT.PicoIdentifier;
  StructMQTT.MqttClientInfo.client_user = "pi";
  StructMQTT.MqttClientInfo.client_pass = MQTT_PASSWORD;
  StructMQTT.MqttClientInfo.keep_alive  = 60;  // Keep alive frequency in seconds.
  StructMQTT.MqttClientInfo.will_topic  = "MQTT-Example";
  StructMQTT.MqttClientInfo.will_msg    = "MQTT Connection terminated";
  // StructMQTT.MqttClientInfo.will_msg    = NULL;
  StructMQTT.MqttClientInfo.will_qos    = 2;  // request to receive message exactly once for Will message on Pico-ASTL-Control.
  StructMQTT.MqttClientInfo.will_retain = 0;


  /* Initialize the callback in charge of processing incoming "publishes" for which we did subscribe. */
  mqtt_set_inpub_callback(StructMQTT.MqttClientInstance, mqtt_incoming_publish_cb, mqtt_incoming_data_cb, &StructMQTT);
  if (FlagLocalDebug)
  {
    log_printf(__LINE__, __func__, "MQTT information before trying to connect to MQTT broker:\n");
    mqtt_display_client();
  }
  sleep_ms(300);  // give some time to complete logging callback before continuing...
  printf("\n");



  /* ----------------------------------------------------------------------------------------------------------------------------------------------------------- *\
                                                                     Connect to MQTT broker.
  \* ----------------------------------------------------------------------------------------------------------------------------------------------------------- */
  ReturnCode = mqtt_client_connect(StructMQTT.MqttClientInstance, &StructMQTT.BrokerAddress, PORT, mqtt_connection_cb, &StructMQTT, &StructMQTT.MqttClientInfo);
  if (ReturnCode != ERR_OK)
  {
    log_printf(__LINE__, __func__, "Error while trying to connect to MQTT broker (return code: %d).\n", ReturnCode);
    log_printf(__LINE__, __func__, "Firmware will restart...\n");
  }
  else
  {
    log_printf(__LINE__, __func__, "Connection request sent to MQTT broker without error (return code: %d).\n", ReturnCode);
    sleep_ms(500);  // wait for response callback to complete to get a clean log.
  }

  return;
}





/* $PAGE */
/* $TITLE=mqtt_pub_request_cb() */
/* ============================================================================================================================================================= *\
                                                     Callback triggered when publishing on a specific topic.
\* ============================================================================================================================================================= */
void mqtt_pub_request_cb(void *, err_t Result)
{
  /// struct struct_mqtt *StructMQTT;
  /// StructMQTT = (struct struct_mqtt *)arg;

  /// log_printf(__LINE__, __func__, "Entering mqtt_pub_request_cb()\n");

  if (!Result)
    log_printf(__LINE__, __func__, "Successfully published to Topic: <%s>   Payload: <%s>   (ReturnCode: %d)\n\n", StructMQTT.Topic, StructMQTT.Payload, Result);
  else
    log_printf(__LINE__, __func__, "Error while trying to publish to Topic: <%s>   Payload: <%s>   (ReturnCode: %d)\n\n", StructMQTT.Topic, StructMQTT.Payload, Result);

  /// log_printf(__LINE__, __func__, "Exiting mqtt_pub_request_cb().\n");

  return;
}





/* $PAGE */
/* $TITLE=mqtt_sub_request_cb() */
/* ============================================================================================================================================================= *\
                                                        Callback in response to a subscribe or unsibscribe request.
\* ============================================================================================================================================================= */
void mqtt_sub_request_cb(void *, err_t Result)
{
#ifdef RELEASE_VERSION
  UINT8 FlagLocalDebug = FLAG_OFF;  // must remain OFF at all time.
#else   // RELEASE_VERSION
  UINT8 FlagLocalDebug = FLAG_OFF;  // may be turned On for debug purposes.
#endif  // RELEASE_VERSION

  /// struct struct_mqtt *StructMQTT;
  /// StructMQTT = (struct struct_mqtt *)arg;

  if (FlagLocalDebug) log_printf(__LINE__, __func__, "Entering mqtt_sub_request_cb().\n");

  if (Result == 0)
  {
    if (StructMQTT.FlagSubscribe == FLAG_ON)
    {
      log_printf(__LINE__, __func__, "Successfully subscribed to topic <%s>   (ReturnCode: %d)\n", StructMQTT.Topic, Result);
    }
    else
    {
      log_printf(__LINE__, __func__, "Successfully unsubscribed to topic <%s>   (ReturnCode: %d)\n", StructMQTT.Topic, Result);
    }
  }
  else
  {
    if (StructMQTT.FlagSubscribe == FLAG_ON)
    {
      log_printf(__LINE__, __func__, "Error while trying to subscribe to topic <%s>   (ReturnCode: 0x%X)\n", StructMQTT.Topic, Result);
    }
    else
    {
      log_printf(__LINE__, __func__, "Error while trying to unsubscribe to topic <%s>   (ReturnCode: 0x%X)\n", StructMQTT. Topic, Result);
    }
  }

  if (FlagLocalDebug) log_printf(__LINE__, __func__, "Exiting mqtt_sub_request_cb().\n");
  printf("\n");

  return;
}





/* $PAGE */
/* $TITLE=term_menu()) */
/* ============================================================================================================================================================= *\
                                          Terminal menu when a CDC USB connection is detected during power up sequence.
\* ============================================================================================================================================================= */
void term_menu()
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
    input_string(String, 1, 0ll);
    log_header();
    log_printf(__LINE__, __func__, "      Terminal menu\n");
    log_printf(__LINE__, __func__, " \n");
    log_printf(__LINE__, __func__, " 1) - Set the IP address of the MQTT broker.\n");
    log_printf(__LINE__, __func__, " 2) - Set MQTT client parameters.\n");
    log_printf(__LINE__, __func__, " 3) - Display MQTT client information.\n");
    log_printf(__LINE__, __func__, " 4) - MQTT Connect.\n");
    log_printf(__LINE__, __func__, " 5) - Subscribe to a MQTT topic.\n");
    log_printf(__LINE__, __func__, " 6) - Publish on a MQTT topic.\n");
    log_printf(__LINE__, __func__, " 7) - Unsubscribe from a MQTT topic.\n");
    log_printf(__LINE__, __func__, " 8) - Disconnect client from MQTT broker.\n");
    log_printf(__LINE__, __func__, " 9) - Find memory pattern for a given number.\n");
    log_printf(__LINE__, __func__, " \n");
    log_printf(__LINE__, __func__, "88) - Restart the Firmware.\n");
    log_printf(__LINE__, __func__, "99) - Switch Pico in upload mode\n");
    log_printf(__LINE__, __func__, " \n");
    log_printf(__LINE__, __func__, "      Enter your choice: ");
    input_string(String, sizeof(String), 0);

    /* If user pressed <Enter> only, loop back to menu. */
    if (String[0] == 0x0D) continue;

    /* If user pressed <ESC>, loop back to menu. */
    if (String[0] == 0x1B)
    {
      String[0] = 0x00;
      printf("\n\n\n");

      return;
    }


    /* User pressed a menu option, execute it. */
    Menu = atoi(String);

    switch(Menu)
    {
       case (1):
        /* Set MQTT broker IP address. */
        printf("\n\n");
        log_printf(__LINE__, __func__, "Setting MQTT broker IP address.\n");
        log_printf(__LINE__, __func__, "===============================\n");
        log_printf(__LINE__, __func__, "Current MQTT broker IP address: %s\n", ip4addr_ntoa(&StructMQTT.BrokerAddress));
        log_printf(__LINE__, __func__, "Enter new IP address for MQTT broker or <Enter> to keep current IP address: ");
        input_string(String, sizeof(String), 0ll);
        if ((String[0] != 0x0D) && (String[0] != 0x1B))
        {
          if (!ip4addr_aton(String, &TestAddress))
          {
            log_printf(__LINE__, __func__, "Invalid IP address entered... IP address has not been changed.\n");
          }
          else
          {
            ip4addr_aton(String, &StructMQTT.BrokerAddress);
            log_printf(__LINE__, __func__, "MQTT server IP address has been set to: <%s>\n", ip4addr_ntoa(&StructMQTT.BrokerAddress));
          }
          printf("\n\n");
        }
        else
        {
          log_printf(__LINE__, __func__, "No change to MQTT broker IP address <%s>.\n", ip4addr_ntoa(&StructMQTT.BrokerAddress));
        }
        printf("\n\n");
      break;

      case (2):
        /* Set MQTT client  parameters. */
        printf("\n\n");
        log_printf(__LINE__, __func__, "Set MQTT client parameters.\n");
        log_printf(__LINE__, __func__, "===========================\n");
        log_printf(__LINE__, __func__, "To be completed\n");
        printf("\n\n");
      break;

      case (3):
        /* Display MQTT client information. */
        printf("\n\n");
        log_printf(__LINE__, __func__, "Display current MQTT client information.\n");
        mqtt_display_client();
        printf("\n\n");
      break;

      case (4):
        /* MQTT Connect. */
        printf("\n\n");
        log_printf(__LINE__, __func__, "Sending <Connect> command to MQTT broker.\n");
        log_printf(__LINE__, __func__, "=========================================\n");
        log_printf(__LINE__, __func__, "Press <G> to proceed: ");
        input_string(String, 1, 0ll);
        if ((String[0] == 'G') || (String[0] == 'g'))
        {
          log_printf(__LINE__, __func__, "Connecting client to MQTT broker.\n");
          ReturnCode = mqtt_client_connect(StructMQTT.MqttClientInstance, &StructMQTT.BrokerAddress, PORT, mqtt_connection_cb, &StructMQTT, &StructMQTT.MqttClientInfo);
          if (ReturnCode != ERR_OK)
            log_printf(__LINE__, __func__, "Error 0x%X while trying to connect to MQTT broker.\n", ReturnCode);
          else
            log_printf(__LINE__, __func__, "Connection request sent to MQTT broker without error (return code: %d).\n", ReturnCode);

          sleep_ms(800);
        }
        else
        {
          log_printf(__LINE__, __func__, "User didn't press <G>, connect command has NOT been sent to MQTT broket.\n");
        }
        printf("\n\n");
      break;

      case (5):
        /* Subscribe to a MQTT topic. */
        printf("\n\n");
        log_printf(__LINE__, __func__, "Subscribe to a MQTT topic.\n");
        log_printf(__LINE__, __func__, "==========================\n");
        log_printf(__LINE__, __func__, "Enter topic to subscribe to: ");
        input_string(Topic, sizeof(Topic), 0);
        if (isalnum(Topic[0]) == 0)
        {
          log_printf(__LINE__, __func__, "No significant data entered for topic... operation cancelled.\n");
          break;
        }
        log_printf(__LINE__, __func__, "Subscribing to topic: <%s>\n", Topic);
        strcpy(StructMQTT.Topic, Topic);
        /// strcpy(StructMQTT.Payload, "1");  // to indicate <subscribe>.
        /// mqtt_sub_unsub(MqttClient.MqttClientInstance, Topic, 1, mqtt_sub_request_cb, MqttClient, 1);  // QoS is 3rd parameter.
        StructMQTT.FlagSubscribe = FLAG_ON;
        ReturnCode = mqtt_subscribe(StructMQTT.MqttClientInstance, Topic, 1, mqtt_sub_request_cb, &StructMQTT);
        /// if ((ReturnCode = mqtt_subscribe(StructMQTT.MqttClientInstance, Topic, 1, mqtt_sub_request_cb, StructMQTT)) != 0)
        if (ReturnCode)
        {
          log_printf(__LINE__, __func__, "Error 0x%X while trying to subscribe to topic <%s>.\n", ReturnCode, Topic);
        }
        else
        {
          log_printf(__LINE__, __func__, "Successfully sent subscribe request.\n");
        }
        sleep_ms(100);
        printf("\n\n");
      break;

      case (6):
        /* Publish on a specific MQTT topic. */
        printf("\n\n");
        log_printf(__LINE__, __func__, "Publish on a specific MQTT topic.\n");
        log_printf(__LINE__, __func__, "=================================\n");
        log_printf(__LINE__, __func__, "Enter Topic to publish on: ");
        input_string(Topic, sizeof(Topic), 0);
        if (isalnum(Topic[0]) == 0)
        {
          log_printf(__LINE__, __func__, "No significant data entered for topic... operation cancelled.\n");
          break;
        }
        log_printf(__LINE__, __func__, "Enter Payload to publish (on Topic <%s>): ", Topic);
        input_string(Payload, sizeof(Payload), 0ll);
        if (isalnum(Payload[0]) == 0)
        {
          log_printf(__LINE__, __func__, "No significant data entered for payload... operation cancelled.\n");
          break;
        }
        strcpy(StructMQTT.Topic, Topic);
        strcpy(StructMQTT.Payload, Payload);
        log_printf(__LINE__, __func__, "Publishing on Topic: <%s>   Payload: <%s>\n", Topic, Payload);
        ReturnCode = mqtt_publish(StructMQTT.MqttClientInstance, Topic, Payload, strlen(Payload), 0, 0, mqtt_pub_request_cb, &StructMQTT);
        if (ReturnCode)
        {
          log_printf(__LINE__, __func__, "Error 0x%X while trying to publish on Topic <%s>   Payload: <%s>.\n", ReturnCode, Topic, Payload);
        }
        sleep_ms(100);
        printf("\n\n");
      break;

      case (7):
        /* Unsubscribe from a MQTT topic. */
        printf("\n\n");
        log_printf(__LINE__, __func__, "Unsubscribe from a MQTT topic.\n");
        log_printf(__LINE__, __func__, "==============================\n");
        log_printf(__LINE__, __func__, "Enter topic to unsubscribe from: ");
        input_string(Topic, sizeof(Topic), 0ll);
        if (isalnum(Topic[0]) == 0)
        {
          log_printf(__LINE__, __func__, "No significant data entered for topic... operation cancelled.\n");
          break;
        }
        log_printf(__LINE__, __func__, "Unsubscribing from topic: <%s>\n", Topic);
        strcpy(StructMQTT.Topic, Topic);
        /// strcpy(StructMQTT.Payload, "0");  // to indicate <unsubscribe>.
         /// mqtt_sub_unsub(MqttClient.MqttClientInstance, Topic, 1, NULL, arg, 0);
        StructMQTT.FlagSubscribe = FLAG_OFF;
        if ((ReturnCode = mqtt_unsubscribe(StructMQTT.MqttClientInstance, Topic, mqtt_sub_request_cb, &StructMQTT)) != 0)
        {
          log_printf(__LINE__, __func__, "Error %d while trying to unsubscribe from topic: <%s>\n", ReturnCode, Topic);
          break;
        }
        sleep_ms(100);
        printf("\n\n");
      break;

      case (8):
        /* Disconnect client from MQTT broker. */
        printf("\n\n");
        log_printf(__LINE__, __func__, "Disconnect client from MQTT broker.\n");
        log_printf(__LINE__, __func__, "===================================\n");
        log_printf(__LINE__, __func__, "Press <G> to proceed: ");
        input_string(String, 1, 0ll);
        if ((String[0] == 'G') || (String[0] == 'g'))
        {
          log_printf(__LINE__, __func__, "Disconnecting client from MQTT broker.\n");
          mqtt_disconnect(StructMQTT.MqttClientInstance);
        }
        else
        {
          log_printf(__LINE__, __func__, "User didn't press <G>, disconnect command has not been sent...\n");
        }
        printf("\n\n");
      break;

      case (9):
        /* Find memory pattern for a given number. */
        printf("\n\n");
        log_printf(__LINE__, __func__, "Find the memory pattern for a given number.\n");
        log_printf(__LINE__, __func__, "===========================================\n");
        log_printf(__LINE__, __func__, "Enter the number to modelize: ");
        input_string(String, sizeof(String), 0ll);
        Dum1UInt16 = atoi(String);
        Dum1UInt64 = atoll(String);

        Dum1UCharPtr = (UCHAR*)&Dum1UInt16;
        printf("\n");
        log_printf(__LINE__, __func__, "UINT16 (address: %p):\n", Dum1UCharPtr);
        for (Loop1UInt16 = 0; Loop1UInt16 < sizeof(UINT16); ++Loop1UInt16)
        {
          log_printf(__LINE__, __func__, "address: %p   0x%2.2X  (decimal: %u)\n", &Dum1UCharPtr[Loop1UInt16], Dum1UCharPtr[Loop1UInt16], Dum1UCharPtr[Loop1UInt16]);
        }
        printf("\n\n");

        Dum1UCharPtr = (UCHAR *)&Dum1UInt64;
        log_printf(__LINE__, __func__, "UINT64 (address: %p):\n", Dum1UCharPtr);
        for (Loop1UInt16 = 0; Loop1UInt16 < sizeof(UINT64); ++Loop1UInt16)
        {
          log_printf(__LINE__, __func__, "Offset: %u   %3u (0x%2.2X)\n", Loop1UInt16, Dum1UCharPtr[Loop1UInt16], Dum1UCharPtr[Loop1UInt16]);
        }
        printf("\n\n");
      break;

      case (88):
        /* Restart the Firmware. */
        printf("\n\n");
        log_printf(__LINE__, __func__, "Restart the Firmware.\n");
        log_printf(__LINE__, __func__, "=====================\n");
        log_printf(__LINE__, __func__, "Press <G> to proceed: ");
        input_string(String, 1, 0ll);
        if ((String[0] == 'G') || (String[0] == 'g'))
        {
          cyw43_arch_deinit();
          log_printf(__LINE__, __func__, "Restarting the Firmware...\n");
          sleep_ms(2000);
          printf("\n\n\n");  // make sure last lines are blanked on terminal.
          log_printf(__LINE__, __func__, "CLS");   // clean log screen before restarting.
          log_printf(__LINE__, __func__, "HOME");  // home the sursor before restarting.
          sleep_ms(500);
          watchdog_enable(1, 1);
          sleep_ms(1000);  // prevent beginning of menu redisplay.
        }
        else
        {
          log_printf(__LINE__, __func__, "User didn't press <G>, Firmware hasn't been restarted...\n");
        }
      break;

      case (99):
        /* Switch the Pico in upload mode. */
        printf("\n\n");
        log_printf(__LINE__, __func__, "Switch Pico in upload mode.\n");
        log_printf(__LINE__, __func__, "===========================\n");
        log_printf(__LINE__, __func__, "Press <G> to proceed: ");
        input_string(String, 1, 0ll);
        if ((String[0] == 'G') || (String[0] == 'g'))
        {
          cyw43_arch_deinit();
          log_printf(__LINE__, __func__, "Toggling Pico in upload mode...\n");
          sleep_ms(2000);
          printf("\n\n\n");  // make sure last lines are blanked on terminal.
          log_printf(__LINE__, __func__, "CLS");   // clean log screen before restarting.
          log_printf(__LINE__, __func__, "HOME");  // home the sursor before restarting.
          sleep_ms(500);
          reset_usb_boot(1, 0);
          sleep_ms(500);
        }
        else
        {
          log_printf(__LINE__, __func__, "User didn't press <G>, Pico hasn't been toggled in upload mode...\n");
        }
        printf("\n\n");
      break;

      default:
        printf("\n\n");
        log_printf(__LINE__, __func__, "Invalid choice... please re-enter [%s]  [%u]\n\n\n\n\n", String, Menu);
        printf("\n\n");
      break;
    }
  }

  return;
}
