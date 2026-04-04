/* ============================================================================================================================================================= *\
   Pico-MQTT-Module.c
   St-Louys Andre - May 2025
   astlouys@gmail.com
   https://github.com/astlouys/Pico-MQTT-Module
   Revision 02-APR-2026
   Langage: C
   Version 3.00

   =========================================================================
   Pico-MQTT-Module is compatible with the ASTL Smart Home ecosystem family.
   =========================================================================

   Raspberry Pi Pico C-Language module for support MQTT protocol in a project / program.

   NOTE:
   THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
   WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
   TIME. AS A RESULT, THE AUTHOR SHALL NOT BE HELD LIABLE FOR ANY DIRECT, 
   INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM
   THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING
   INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCT.


   NOTES:
   Pico-MQTT_Example.c gives an example on how to use this module.

   REVISION HISTORY:
   =================
   21-MAY-2025 1.00 - Initial release.
   14-JUL-2025 2.03 - Rename function log_info() to log_printf()
                    - Use common functions: get_pico_identifier(), input_string(), log_printf().
                    - Convert all <\r> to <\n>.
                    - Adapted as a <module> to comply with other Pico-ASTL-xxx ecosystem members.
                    - Change algorithm to parse Topics into sub-topics and Payload into sub-payloads: add end-of-strings in the original main Topic and main
                      Payload string instead of duplicating them into other similar sub-strings. NOTE: if the payload we are using is binary data, Firmware
                      logic will require to be changed / adapted.
                    - Add MQTT breakdown history and related functions (mqtt_breakdown_start(), mqtt_breakdown_end()) and print MQTT breakdown history at the
                      end of MQTT information display (mqtt_display_client()).
   06-JAN-2026 2.04 - Many improvements and cosmetics changes.
                    - Adapted for the new updates done to Pico-WiFi-Module and Pico-MQTT-Module.
   29-MAR-2026 3.00 - Adapted to the last modifications to comply with ASTL Smart Home ecosystem standards.
\* ============================================================================================================================================================= */



/* $PAGE */
/* $TITLE=Generic list of GPIO usage. */
/* ============================================================================================================================================================= *\
                                            Generic list of GPIOs usage and pin-out for most ASTL Smart Home ecosystem devices.
                                               (To be used as a guideline only. May sometimes be different for some devices)
\* ============================================================================================================================================================= */
/* ------------------------------------------------------------------------------------------------------------------------------------------------------------- *\
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
Pin 11 - GPIO  8  - LCD_DATA_COMMAND - data / command control pin  (on Waveshare's 2-inch LCD display)     Pimoroni Explorer Base: Motor1-.
Pin 12 - GPIO  9  - LCD_CHIP_SELECT  - chip select pin, active low (on Waveshare's 2-inch LCD display)     Pimoroni Explorer Base: Motor1+.
Pin 13 - GND      - Ground
Pin 14 - GPIO 10  - LCD_CLOCK        - SPI clock pin               (on Waveshare's 2-inch LCD display)     Pimoroni Explorer Base: Motor2-.
Pin 15 - GPIO 11  - LCD_DATA         - SPI  data pin               (on Waveshare's 2-inch LCD display)     Pimoroni Explorer Base: Motor2+.
Pin 16 - GPIO 12  - LCD_RESET        - reset pin, active low       (on Waveshare's 2-inch LCD display)     Pimoroni Explorer Base: ButtonA.
Pin 17 - GPIO 13  - LCD_BRIGHTNESS   - backlight pwm pin           (on Waveshare's 2-inch LCD display)     Pimoroni Explorer Base: ButtonB.
Pin 18 - GND      - Ground
Pin 19 - GPIO 14  - Reserved for Pico-ASTL-SoundServer status pin                                          Pimoroni Explorer Base: ButtonX.
Pin 20 - GPIO 15  - Button 0                                       (on Waveshare's 2-inch LCD display)     Pimoroni Explorer Base: ButtonY.
Pin 21 - GPIO 16  - Reserved for infrared sensor (VS1838)                                                  Pimoroni Explorer Base: SPI MISO.
Pin 22 - GPIO 17  - Button 1                                       (on Waveshare's 2-inch LCD display)     Pimoroni Explorer Base: LCD Chip Select.
Pin 23 - GND      - Ground
Pin 24 - GPIO 18  - Reserved for indicator LED                                                             Pimoroni Explorer Base: SPI SCK.
Pin 25 - GPIO 19  - Reserved for tri-color LED (RED)                                                       Pimoroni Explorer Base: SPI MOSI.
Pin 26 - GPIO 20  - Reserved for tri-color LED (GREEN)                                                     Pimoroni Explorer Base: I2C SDA (data).
Pin 27 - GPIO 21  - Reserved for tri-color LED (BLUE)                                                      Pimoroni Explorer Base: I2C SCL (clock).
Pin 28 - GND      - Ground
Pin 29 - GPIO 22  - Reserved for Active or Passive buzzer                                                  Pimoroni Explorer Base: I2C INT (interrupt).
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

Test points under Pico PC board:
Test point 1 - Ground.
Test point 2 - USB DM  - USB data negative pin.
Test point 3 - USB DP  - USB data positive pin.
Test point 4 - GPIO 23 - Switched Mode Power Supply (SMPS) Pin. Controls the on-board SMPS power save pin.
Test point 5 - GPIO 25 - Pico's LED.
Test point 6 - Bootsel (upload mode).
\* ------------------------------------------------------------------------------------------------------------------------------------------------------------- */



/* $PAGE */
/* $TITLE=MQTT functions definition. */
/* ============================================================================================================================================================= *\
                                                      MQTT functions definition (only for reference purpose).
\* ============================================================================================================================================================= */
/*
err_t mqtt_client_connect(mqtt_client_t *client, const ip_addr_t *ip_addr, u16_t port, mqtt_connection_cb_t cb, void *arg, const struct mqtt_connect_client_info_t *client_info)
void mqtt_client_free(mqtt_client_t *client);
u8_t mqtt_client_is_connected(mqtt_client_t *client);
mqtt_client_t *mqtt_client_new(void);
void mqtt_disconnect(mqtt_client_t *client);
err_t mqtt_publish(mqtt_client_t *client, const char *topic, const void *payload, u16_t payload_length, u8_t qos, u8_t retain, mqtt_request_cb_t cb, void *arg);
void  mqtt_set_inpub_callback(mqtt_client_t *client, mqtt_incoming_publish_cb_t pub_cb, mqtt_incoming_data_cb_t data_cb, void *arg);
err_t mqtt_sub_unsub(mqtt_client_t *client, const char *topic, u8_t qos, mqtt_request_cb_t cb, void *arg, u8_t sub);

callback prototypes:
void (*mqtt_connection_cb_t)(mqtt_client_t *client, void *arg, mqtt_connection_status_t status);

NOTE: QoS (Quality of Service) for MQTT message delivery goes as follow:
      QoS = 0 -> Message received at most once.
      QoS = 1 -> Message received at least once.
      QoS = 2 -> Message received exactly once.
*/



/* $PAGE */
/* $TITLE=Include files. */
/* ============================================================================================================================================================= *\
                                                                          Include files
\* ============================================================================================================================================================= */
#include "baseline.h"
#include "hardware/gpio.h"
#include "hardware/irq.h"
#include "hardware/rtc.h"
#include "pico/stdlib.h"
#include "stdarg.h"
#include <stdio.h>
#include "string.h"

#include "Pico-MQTT-Module.h"


 
/* $PAGE */
/* $TITLE=Definitions and macros. */
/* ============================================================================================================================================================= *\
                                                                     Definitions and macros.
\* ============================================================================================================================================================= */
#define RELEASE_VERSION
// #define FRENCH   // not used for now.
// #define ENGLISH  // not used for now.


/* $PAGE */
/* $TITLE=Global variables declaration / definition. */
/* ============================================================================================================================================================= *\
                                                               Global variables declaration / definition.
\* ============================================================================================================================================================= */
extern struct struct_mqtt StructMQTT;
extern UCHAR DayName[7][13];
extern UCHAR ShortMonth[13][4];
extern UCHAR PicoIdentifier[40];
extern UCHAR PicoUniqueId[25];





/* $PAGE */
/* $TITLE=mqtt_breakdown_end() */
/* ============================================================================================================================================================= *\
                                                               Log time at the end of MQTT breakdown.
\* ============================================================================================================================================================= */
void mqtt_breakdown_end(void)
{
#ifdef RELEASE_VERSION
  UINT8 FlagLocalDebug = FLAG_OFF;  // must be turned OFF at all time.
#else   // RELEASE_VERSION
  UINT8 FlagLocalDebug = FLAG_OFF;  // may be turned ON for debug purposes.
#endif  // RELEASE_VERSION

  datetime_t EndTime;


  if (StructMQTT.BreakdownStart[0].day == 0)
  {
    log_printf(__LINE__, __func__, "There is currently no MQTT breakdown start time logged, it must be the first MQTT connect request during startup sequence.\n");
    return;
  }


  log_printf(__LINE__, __func__, "There is a MQTT breakdown start time logged, so log the breakdown end time.\n");
  rtc_get_datetime(&EndTime);
 
  /* Write new entry on top of history (history has already been slided down while writing beginning of breakdown). */
  StructMQTT.BreakdownEnd[0].dotw  = EndTime.dotw;
  StructMQTT.BreakdownEnd[0].day   = EndTime.day;
  StructMQTT.BreakdownEnd[0].month = EndTime.month;
  StructMQTT.BreakdownEnd[0].year  = EndTime.year;
  StructMQTT.BreakdownEnd[0].hour  = EndTime.hour;
  StructMQTT.BreakdownEnd[0].min   = EndTime.min;
  StructMQTT.BreakdownEnd[0].sec   = EndTime.sec;

  if (FlagLocalDebug)
  {
    log_printf(__LINE__, __func__, "End of MQTT downtime: Day: %2.2u   Month: %2.2u   Year: %4.4u   at   %2.2u:%2.2u:%2.2u\n", 
               EndTime.day,  EndTime.month, EndTime.year, EndTime.hour, EndTime.min,   EndTime.sec);
  }

  return;
}





/* $PAGE */
/* $TITLE=mqtt_breakdown_start() */
/* ============================================================================================================================================================= *\
                                                         Log the time at the beginning of MQTT breakdown.
\* ============================================================================================================================================================= */
void mqtt_breakdown_start(void)
{
#ifdef RELEASE_VERSION
  UINT8 FlagLocalDebug = FLAG_OFF;  // must be turned OFF at all time.
#else   // RELEASE_VERSION
  UINT8 FlagLocalDebug = FLAG_OFF;  // may be turned ON for debug purposes.
#endif  // RELEASE_VERSION

  UINT16 Loop1UInt16;
  UINT16 Loop2UInt16;

  datetime_t StartTime;


  rtc_get_datetime(&StartTime);

  /* Slide current breakdown history one line down to make room for the new entry on top. */
  for (Loop1UInt16 = MAX_MQTT_BREAKDOWN_HISTORY; Loop1UInt16 > 1; --Loop1UInt16)
  {
    memcpy(&StructMQTT.BreakdownStart[Loop1UInt16 - 1], &StructMQTT.BreakdownStart[Loop1UInt16 - 2], sizeof(datetime_t));
    memcpy(&StructMQTT.BreakdownEnd[Loop1UInt16 - 1],   &StructMQTT.BreakdownEnd[Loop1UInt16   - 2], sizeof(datetime_t));
    memset(&StructMQTT.BreakdownStart[Loop1UInt16 - 2], 0x00, sizeof(datetime_t));  // wipe the entry that we just moved down.
    memset(&StructMQTT.BreakdownEnd[Loop1UInt16 - 2],   0x00, sizeof(datetime_t));  // wipe the entry that we just moved down.

#if 0
    log_printf(__LINE__, __func__, "MQTT breakdown history after sliding down line: %2u (for a maximum of %u entries)\n", Loop1UInt16, MAX_MQTT_BREAKDOWN_HISTORY);
    log_printf(__LINE__, __func__, "              Breakdown start time                Breakdown end time\n");
    log_printf(__LINE__, __func__, "Line   DoW    Date           Time             DoW    Date           Time\n");
    for (Loop2UInt16 = 0; Loop2UInt16 < MAX_MQTT_BREAKDOWN_HISTORY; ++Loop2UInt16)
    {
      /* Display all entries for debugging. */
      log_printf(__LINE__, __func__, "%2u)     %u  %2.2u-%2.2u-%4.4u  at  %2.2u:%2.2u:%2.2u            %u  %2.2u-%2.2u-%4.4u  at  %2.2u:%2.2u:%2.2u\n",
                 Loop2UInt16 + 1,
                 StructMQTT.BreakdownStart[Loop2UInt16].dotw,
                 StructMQTT.BreakdownStart[Loop2UInt16].day,
                 StructMQTT.BreakdownStart[Loop2UInt16].month,
                 StructMQTT.BreakdownStart[Loop2UInt16].year,
                 StructMQTT.BreakdownStart[Loop2UInt16].hour,
                 StructMQTT.BreakdownStart[Loop2UInt16].min,
                 StructMQTT.BreakdownStart[Loop2UInt16].sec,
                 StructMQTT.BreakdownEnd[Loop2UInt16].dotw,
                 StructMQTT.BreakdownEnd[Loop2UInt16].day,
                 StructMQTT.BreakdownEnd[Loop2UInt16].month,
                 StructMQTT.BreakdownEnd[Loop2UInt16].year,
                 StructMQTT.BreakdownEnd[Loop2UInt16].hour,
                 StructMQTT.BreakdownEnd[Loop2UInt16].min,
                 StructMQTT.BreakdownEnd[Loop2UInt16].sec);
    }
    printf("\n");
#endif  // 0
  }

  /* Add new entry on top of history. */
  StructMQTT.BreakdownStart[0].dotw  = StartTime.dotw;
  StructMQTT.BreakdownStart[0].day   = StartTime.day;
  StructMQTT.BreakdownStart[0].month = StartTime.month;
  StructMQTT.BreakdownStart[0].year  = StartTime.year;
  StructMQTT.BreakdownStart[0].hour  = StartTime.hour;
  StructMQTT.BreakdownStart[0].min   = StartTime.min;
  StructMQTT.BreakdownStart[0].sec   = StartTime.sec;

  if (FlagLocalDebug)
  {
    log_printf(__LINE__, __func__, "MQTT start of downtime: Day: %2.2u   Month: %2.2u   Year: %4.4u   at   %2.2u:%2.2u:%2.2u\n", 
               StartTime.day, StartTime.month, StartTime.year, StartTime.hour, StartTime.min, StartTime.sec);
    log_printf(__LINE__, __func__, "       MQTT breakdown start time              MQTT breakdown end time\n");
    log_printf(__LINE__, __func__, "Line   DoW    Date           Time             DoW    Date           Time\n");
    for (Loop2UInt16 = 0; Loop2UInt16 < MAX_MQTT_BREAKDOWN_HISTORY; ++Loop2UInt16)
    {
      /* Display all entries for debugging. */
      if (StructMQTT.BreakdownStart[Loop2UInt16].day)
      {
        log_printf(__LINE__, __func__, "%2u)     %u  %2.2u-%2.2u-%4.4u  at  %2.2u:%2.2u:%2.2u            %u  %2.2u-%2.2u-%4.4u  at  %2.2u:%2.2u:%2.2u\n",
                   Loop2UInt16 + 1,
                   StructMQTT.BreakdownStart[Loop2UInt16].dotw,
                   StructMQTT.BreakdownStart[Loop2UInt16].day,
                   StructMQTT.BreakdownStart[Loop2UInt16].month,
                   StructMQTT.BreakdownStart[Loop2UInt16].year,
                   StructMQTT.BreakdownStart[Loop2UInt16].hour,
                   StructMQTT.BreakdownStart[Loop2UInt16].min,
                   StructMQTT.BreakdownStart[Loop2UInt16].sec,
                   StructMQTT.BreakdownEnd[Loop2UInt16].dotw,
                   StructMQTT.BreakdownEnd[Loop2UInt16].day,
                   StructMQTT.BreakdownEnd[Loop2UInt16].month,
                   StructMQTT.BreakdownEnd[Loop2UInt16].year,
                   StructMQTT.BreakdownEnd[Loop2UInt16].hour,
                   StructMQTT.BreakdownEnd[Loop2UInt16].min,
                   StructMQTT.BreakdownEnd[Loop2UInt16].sec);
      }
    }
    printf("\n");
  }

  return;
}





/* $PAGE */
/* $TITLE=mqtt_check_connection() */
/* ============================================================================================================================================================= *\
                                                                   Check MQTT connection health.
\* ============================================================================================================================================================= */
INT16 mqtt_check_connection(UINT8 FlagWiFiHealth)
{
#ifdef RELEASE_VERSION
  UINT8 FlagLocalDebug = FLAG_OFF;  // must be turned OFF at all time.
#else   // RELEASE_VERSION
  UINT8 FlagLocalDebug = FLAG_OFF;  // may be turned ON for debug purposes.
#endif  // RELEASE_VERSION

  static UINT16 RetryCycles = 4;

  static UINT32 MQTTCycles15Sec;  // cumulative number of "15-second cycles" with MQTT down 


  if (FlagLocalDebug)
  {
    /* Optionally display debug information on entry. */
    log_printf(__LINE__, __func__, "Entering mqtt_check_connection()   Wi-Fi health: 0x%2.2X   StructMQTT.MqttClientInstance: 0x%p\n", FlagWiFiHealth, StructMQTT.MqttClientInstance);
    log_printf(__LINE__, __func__, "StructMQTT.FlagHealth: %u   FlagStartupOver: %u   MQTTCycles15Sec: %u   RetryCycles: %u\n", StructMQTT.FlagHealth, StructMQTT.FlagStartupOver, MQTTCycles15Sec, RetryCycles);
  }


  if (StructMQTT.MqttClientInstance)
  {
    if (mqtt_client_is_connected(StructMQTT.MqttClientInstance))
    {
      /* MQTT connection is OK. */
      if (FlagLocalDebug) log_printf(__LINE__, __func__, "MQTT client is connected to MQTT broker: %d\n", mqtt_client_is_connected(StructMQTT.MqttClientInstance));
      return 0;
    }
    else
    {
      log_printf(__LINE__, __func__, "MQTT client is NOT connected to MQTT broker: %d (cycle number %u)\n", mqtt_client_is_connected(StructMQTT.MqttClientInstance), RetryCycles);
    }
  }


  if ((!StructMQTT.MqttClientInstance) || (!mqtt_client_is_connected(StructMQTT.MqttClientInstance)))
  {
    if (StructMQTT.FlagStartupOver == FLAG_OFF)
      log_printf(__LINE__, __func__, "MQTT client is not connected to MQTT broker yet.\n");
    else
      log_printf(__LINE__, __func__, "MQTT client has been disconnected from MQTT broker.\n");

    /* MQTT client is disconnected from broker. */
    if (StructMQTT.FlagHealth == FLAG_ON)
    {
      /* Connection with broker was still good during previous cycle... make sure current MQTT parameters are cleaned. */
      if (FlagLocalDebug) log_printf(__LINE__, __func__, "MQTT connection was good during previous cycle, time stamp MQTT breakdown start.\n");
      StructMQTT.FlagHealth = FLAG_OFF;

      /* Since MQTT connection was still good during previous cycle, this is the beginning of a new MQTT breakdown period. */
      ++StructMQTT.TotalErrors;

      /* Keep track of time at beginning of breakdown. */
      mqtt_breakdown_start();
    }


    if (FlagWiFiHealth == FLAG_ON)
    {
      /* Wi-Fi connection is OK, problem is only with MQTT connection. */
      ++MQTTCycles15Sec;  // one more 15-seconds cycle with a bad MQTT connection, will rollover to 0 when reaching end of UINT32 range if ever the case.
      if (FlagLocalDebug) log_printf(__LINE__, __func__, "Wi-Fi health is OK, increment MQTT cycles before next retry (%lu).\n", MQTTCycles15Sec);

      /* Try to reconnect with MQTT broker every once in a while. */
      if ((!(MQTTCycles15Sec % RetryCycles)) || (StructMQTT.FlagStartupOver == FLAG_OFF))
      {
        if (FlagLocalDebug) log_printf(__LINE__, __func__, "FlagStartupOver -> %u or MQTTCycles15Sec divisible by 4 -> %u     Trying to connect to MQTT broker.\n", StructMQTT.FlagStartupOver, MQTTCycles15Sec);
        /* Validate MQTT broker IP address. */
        if (!ip4addr_aton(MQTT_BROKER_IP, &StructMQTT.BrokerAddress))
        {
          log_printf(__LINE__, __func__, "Invalid MQTT broker IP address.\n");
        }
        else
        {
          if (FlagLocalDebug) log_printf(__LINE__, __func__, "MQTT broker IP address seems valid: %s, proceed with mqtt_init()\n", ip4addr_ntoa(&StructMQTT.BrokerAddress));
          if (mqtt_init() == 0)
          {
            log_printf(__LINE__, __func__, "MQTT client instance created (0x%p).\n", StructMQTT.MqttClientInstance);
            MQTTCycles15Sec = 0l;
            return 1;  // indicate that MQTT Client instance has been created.
          }
          else
          {
            StructMQTT.FlagHealth = FLAG_OFF;
            log_printf(__LINE__, __func__, "MQTT Client instance could not be created.\n");
          }
        }
      }
    }
  }
  log_printf(__LINE__, __func__, "Returning error code -1 (MQTT connection problem).\n");

  return -1;
}





/* $PAGE */
/* $TITLE=mqtt_connection_cb() */
/* ============================================================================================================================================================= *\
                                                         Receiving the response for a MQTT connection request.
\* ============================================================================================================================================================= */
void mqtt_connection_cb(mqtt_client_t *, void *ExtraArgument, mqtt_connection_status_t Status)
{
#ifdef RELEASE_VERSION
  UINT8 FlagLocalDebug = FLAG_OFF;  // must be turned OFF at all time.
#else   // RELEASE_VERSION
  UINT8 FlagLocalDebug = FLAG_OFF;  // may be turned ON for debug purposes.
#endif  // RELEASE_VERSION

  UINT16 ConnectionStatus;


  if (FlagLocalDebug) log_printf(__LINE__, __func__, "Entering mqtt_connection_cb(0x%p)\n", ExtraArgument);

  ConnectionStatus = MQTT_CONNECTION_ERROR;  // assign default value.

  switch(Status)
  {
    case(MQTT_CONNECT_ACCEPTED):  // 0
      /* Connection accepted by MQTT broker. */
      log_printf(__LINE__, __func__, "Connection accepted by MQTT broker (Status: %d)\n", Status);
      ConnectionStatus = MQTT_CONNECTION_OK;
      StructMQTT.FlagHealth = FLAG_ON;
      StructMQTT.FlagStartupOver = FLAG_ON; 
      mqtt_breakdown_end();
    break;

    case(MQTT_CONNECT_REFUSED_PROTOCOL_VERSION):
      /* Connection refused: bad protocol version. */
      log_printf(__LINE__, __func__, "Connection FAILED to MQTT broker: bad protocol version (Status: %d)\n\n", Status);
    break;

    case(MQTT_CONNECT_REFUSED_IDENTIFIER):
      /* Connection refused: refused identifier. */
      log_printf(__LINE__, __func__, "Connection FAILED to MQTT broker: bad identifier (Status: %d)\n\n", Status);
    break;

    case(MQTT_CONNECT_REFUSED_SERVER):
      /* Connection refused: refused server. */
      log_printf(__LINE__, __func__, "Connection FAILED to MQTT broker: bad server (Status: %d)\n\n", Status);
    break;

    case(MQTT_CONNECT_REFUSED_USERNAME_PASS):
      /* Connection refused: refused user credentials. */
      log_printf(__LINE__, __func__, "Connection FAILED to MQTT broker: bad user credentials (Status: %d)\n\n", Status);
    break;

    case(MQTT_CONNECT_REFUSED_NOT_AUTHORIZED_):
      /* Connection refused: refused not authorized. */
      log_printf(__LINE__, __func__, "Connection FAILED to MQTT broker: not authorized (Status: %d)\n\n", Status);
    break;

    case(MQTT_CONNECT_DISCONNECTED):
      /* Connection disconnected. */
      log_printf(__LINE__, __func__, "MQTT client has been disconnected (Status: %d)\n\n", Status);
    break;

    case(MQTT_CONNECT_TIMEOUT):
      /* Connection timed out. */
      log_printf(__LINE__, __func__, "MQTT connection timed out (Status: %d)\n\n", Status);
    break;

    default:
      /* Undefined return code. */
      log_printf(__LINE__, __func__, "MQTT connection problem: undefined return code (%d)\n\n", Status);
    break;
  }

  if (StructMQTT.mqtt_status) StructMQTT.mqtt_status(ConnectionStatus);

  // log_printf(__LINE__, __func__, "Exiting mqtt_connection_cb().\n\n");

  return;  
}





/* $PAGE */
/* $TITLE=mqtt_display_client() */
/* ============================================================================================================================================================= *\
                                                                   Display MQTT client information.
\* ============================================================================================================================================================= */
void mqtt_display_client(void)
{
  UINT8 TotalCount;

  UINT16 Loop1UInt16;

  log_printf(__LINE__, __func__, "========================================================================================================================\n");
  log_printf(__LINE__, __func__, "                                                    MQTT information\n");
  log_printf(__LINE__, __func__, "========================================================================================================================\n");

  /* Display MQTT health and information directly related to MqttClientInfo if its pointer is valid. */
  if (!StructMQTT.MqttClientInstance)
  {
    log_printf(__LINE__, __func__, "MQTT client instance has been disallocated.\n");
  }
  else
  {
    if (mqtt_client_is_connected(StructMQTT.MqttClientInstance))
    {
      log_printf(__LINE__, __func__, "MQTT client is connected to MQTT broker.\n");
      log_printf(__LINE__, __func__, "MQTT health:                   Good\n");
      log_printf(__LINE__, __func__, "MqttClientInfo.client_id:      <%s>\n", StructMQTT.MqttClientInfo.client_id);
      log_printf(__LINE__, __func__, "MqttClientInfo.client_user:    <%s>\n", StructMQTT.MqttClientInfo.client_user);
      log_printf(__LINE__, __func__, "MqttClientInfo.client_pass:    <%s>\n", StructMQTT.MqttClientInfo.client_pass);
      log_printf(__LINE__, __func__, "MqttClientInfo.keep_alive:     <%u>\n", StructMQTT.MqttClientInfo.keep_alive);
      log_printf(__LINE__, __func__, "MqttClientInfo.will_topic:     <%s>\n", StructMQTT.MqttClientInfo.will_topic);
      log_printf(__LINE__, __func__, "MqttClientInfo.will_msg:       <%s>\n", StructMQTT.MqttClientInfo.will_msg);
      log_printf(__LINE__, __func__, "MqttClientInfo.will_qos:       <%u>\n", StructMQTT.MqttClientInfo.will_qos);
      log_printf(__LINE__, __func__, "MqttClientInfo.will_retain:    <%u>\n", StructMQTT.MqttClientInfo.will_retain);

#if 0  // Below for reference purposes.
  
    u16_t cyclic_tick;
    u16_t keep_alive;
    u16_t server_watchdog;
    /** Packet identifier generator*/
    u16_t pkt_id_seq;
    /** Packet identifier of pending incoming publish */
    u16_t inpub_pkt_id;
    /** Connection state */
    u8_t conn_state;
    struct altcp_pcb *conn;
    /** Connection callback */
    void *connect_arg;
    mqtt_connection_cb_t connect_cb;
    /** Pending requests to server */
    struct mqtt_request_t *pend_req_queue;
    struct mqtt_request_t req_list[MQTT_REQ_MAX_IN_FLIGHT];
    void *inpub_arg;
    /** Incoming data callback */
    mqtt_incoming_data_cb_t data_cb;
    mqtt_incoming_publish_cb_t pub_cb;
    /** Input */
    u32_t msg_idx;
    u8_t rx_buffer[MQTT_VAR_HEADER_BUFFER_LEN];
    /** Output ring-buffer */
    struct mqtt_ringbuf_t output;
#endif  // 0
    }
    else
    {
      log_printf(__LINE__, __func__, "MQTT client is disconnected from MQTT broker\n");
    }
  }

  log_printf(__LINE__, __func__, "Total unique MQTT error count: %lu\n", StructMQTT.TotalErrors);
  log_printf(__LINE__, __func__, "MQTT broker IP address:        <%s>\n", ip4addr_ntoa(&StructMQTT.BrokerAddress));
  log_printf(__LINE__, __func__, "Pico IP address:               <%s>\n", ip4addr_ntoa(&StructMQTT.PicoIPAddress));
  log_printf(__LINE__, __func__, "Pico Unique ID:                <%s>\n", StructMQTT.PicoUniqueId);
  log_printf(__LINE__, __func__, "Device Identifier:             <%s>\n", StructMQTT.PicoIdentifier);
  log_printf(__LINE__, __func__, "========================================================================================================================\n");
  log_printf(__LINE__, __func__, "<120>Last Topic details:\n");
  mqtt_display_topic();
  log_printf(__LINE__, __func__, "========================================================================================================================\n");
  log_printf(__LINE__, __func__, "<120>Last Payload details:\n");
  mqtt_display_payload();
  log_printf(__LINE__, __func__, "========================================================================================================================\n");
  log_printf(__LINE__, __func__, "<120>MQTT breakdown history (for a maximum of last %u entries)\n", MAX_MQTT_BREAKDOWN_HISTORY);
  log_printf(__LINE__, __func__, "              MQTT breakdown start time                  MQTT breakdown end time\n");
  TotalCount = 0;
  for (Loop1UInt16 = 0; Loop1UInt16 < MAX_MQTT_BREAKDOWN_HISTORY; ++Loop1UInt16)
  {
    /* Display only valid entries. */
    if (StructMQTT.BreakdownStart[Loop1UInt16].day == 0) continue;
    ++TotalCount;
    log_printf(__LINE__, __func__, "%2u)   %10s %2u-%3s-%4u  at  %2.2u:%2.2u:%2.2u",
               Loop1UInt16 + 1,
               DayName[StructMQTT.BreakdownStart[Loop1UInt16].dotw],
               StructMQTT.BreakdownStart[Loop1UInt16].day,
               ShortMonth[StructMQTT.BreakdownStart[Loop1UInt16].month],
               StructMQTT.BreakdownStart[Loop1UInt16].year,
               StructMQTT.BreakdownStart[Loop1UInt16].hour,
               StructMQTT.BreakdownStart[Loop1UInt16].min,
               StructMQTT.BreakdownStart[Loop1UInt16].sec);

    if (StructMQTT.BreakdownEnd[Loop1UInt16].day == 0)
    {
      printf("                      - - - - -\n");
      continue;
    }
    else
    {
      printf("     %10s %2u-%3s-%4u  at  %2.2u:%2.2u:%2.2u\n",
             DayName[StructMQTT.BreakdownEnd[Loop1UInt16].dotw],
             StructMQTT.BreakdownEnd[Loop1UInt16].day,
             ShortMonth[StructMQTT.BreakdownEnd[Loop1UInt16].month],
             StructMQTT.BreakdownEnd[Loop1UInt16].year,
             StructMQTT.BreakdownEnd[Loop1UInt16].hour,
             StructMQTT.BreakdownEnd[Loop1UInt16].min,
             StructMQTT.BreakdownEnd[Loop1UInt16].sec);
    }
  }
  if (TotalCount == 0) log_printf(__LINE__, __func__, "   ---> No MQTT breakdown recorded so far...\n");
  log_printf(__LINE__, __func__, "========================================================================================================================\n");

  return;
}





/* $PAGE */
/* $TITLE=mqtt_display_payload() */
/* ============================================================================================================================================================= *\
                                                                 Display all current MQTT sub-payloads.
                    NOTE: This function assumes that payload is an ASCII string. If payload is binary, it should be displayed as hex values.
\* ============================================================================================================================================================= */
void mqtt_display_payload(void)
{
#ifdef RELEASE_VERSION
  UINT8 FlagLocalDebug = FLAG_OFF;  // must be turned OFF at all time.
#else   // RELEASE_VERSION
  UINT8 FlagLocalDebug = FLAG_OFF;  // may be turned ON for debug purposes.
#endif  // RELEASE_VERSION

  UINT8  DisplayLength;

  UINT16 Loop1UInt16;

  DisplayLength = 50;  // limit to first 50 characters.
  log_printf(__LINE__, __func__, "========================================================================================================================\n");
  log_printf(__LINE__, __func__, "<120> Payload\n");
  log_printf(__LINE__, __func__, "========================================================================================================================\n");

  /* Parse main payload string into its sub-payload components. */
  mqtt_parse_item(PARSE_PAYLOAD);


  if (FlagLocalDebug)
  {
    /* Display Payload string count in decimal. */
    for (Loop1UInt16 = 0; Loop1UInt16 < DisplayLength; ++Loop1UInt16) printf("%3u  ",    Loop1UInt16);
    printf("\n");

    /* Display Payload hex values. */
    for (Loop1UInt16 = 0; Loop1UInt16 < DisplayLength; ++Loop1UInt16) printf("0x%2.2X ", StructMQTT.Payload[Loop1UInt16]);
    printf("\n");

    /* Display Payload characters. */
    for (Loop1UInt16 = 0; Loop1UInt16 < DisplayLength; ++Loop1UInt16)
    {
      if (isprint(StructMQTT.Payload[Loop1UInt16]))
        printf("  %c  ",   StructMQTT.Payload[Loop1UInt16]);
      else
        printf("  -  ");
    }
    printf("\n");

    log_printf(__LINE__, __func__, "========================================================================================================================\n");

    /* Display content of all sub-payload null string or not. */
    for (Loop1UInt16 = 0; Loop1UInt16 < MAX_SUB_PAYLOADS; ++Loop1UInt16)
    {
      log_printf(__LINE__, __func__, "SubPayload[%2u] (0x%p) -> <0x%p>   <%s>\n", Loop1UInt16, &StructMQTT.SubPayload[Loop1UInt16], StructMQTT.SubPayload[Loop1UInt16], StructMQTT.SubPayload[Loop1UInt16]);
    }
    log_printf(__LINE__, __func__, "========================================================================================================================\n");
  }


  /* Display non-null sub-payloads. */
  for (Loop1UInt16 = 0; Loop1UInt16 < MAX_SUB_PAYLOADS; ++Loop1UInt16)
  {
    if ((StructMQTT.SubPayload[Loop1UInt16][0]) && (StructMQTT.SubPayload[Loop1UInt16][0] != 0x0D))
      log_printf(__LINE__, __func__, "%2u) <%s>\n", Loop1UInt16 + 1, StructMQTT.SubPayload[Loop1UInt16]);
    else
      break;
  }

  return;
}





/* $PAGE */
/* $TITLE=mqtt_display_topic() */
/* ============================================================================================================================================================= *\
                                                                  Display all current MQTT sub-topics.
\* ============================================================================================================================================================= */
void mqtt_display_topic(void)
{
#ifdef RELEASE_VERSION
  UINT8 FlagLocalDebug = FLAG_OFF;  // must be turned OFF at all time.
#else   // RELEASE_VERSION
  UINT8 FlagLocalDebug = FLAG_OFF;  // may be turned ON for debug purposes.
#endif  // RELEASE_VERSION

  UCHAR String[50];

  UINT8 DisplayLength;
  UINT8 SourceTopic;  // number of the last sub-topic representing the source of MQTT packet as per ASTL ecosystem convention.

  UINT16 Loop1UInt16;

  DisplayLength = 50;  // limit to first 50 characters.

  log_printf(__LINE__, __func__, "========================================================================================================================\n");
  log_printf(__LINE__, __func__, "<120> Topic\n");
  log_printf(__LINE__, __func__, "========================================================================================================================\n");

  /* Parse main topic string into its sub-topic components. */
  mqtt_parse_item(PARSE_TOPIC);


  /* Retrieve source of MQTT packet. */
  /* NOTE: This is an "ASTL Smart Home ecosystem" standard and not a MQTT standard. */
  /* You may want to comment out the block below if you use the code for another device. */
  /* Identify source of the MQTT packet, as per ASTL ecosystem convention. */
  SourceTopic = MAX_SUB_TOPICS;  // initialize with invalid value on entry.
  /// for (Loop1UInt16 = 0; Loop1UInt16 < sizeof(String); ++Loop1UInt16) String[Loop1UInt16] = '\0';
  memset(String, 0x00, sizeof(String));
  for (Loop1UInt16 = MAX_SUB_TOPICS; Loop1UInt16 > 0; --Loop1UInt16)
  {
    if (StructMQTT.SubTopic[Loop1UInt16 - 1] != NULL)
    {
      SourceTopic = Loop1UInt16 - 1;
      break;
    }
  }


  /* Check if a sub-topic has been found. */
  if (SourceTopic == MAX_SUB_TOPICS)
  {
    /* Still contains starting invalid value -> Source of MQTT packet has not been found. */
    sprintf(String, "Source of MQTT packet unidentified.");
  }
  else
  {
    sprintf(String, "Source: %s", StructMQTT.SubTopic[SourceTopic]);
  }


  if (FlagLocalDebug)
  {
    /* Display Topic string count in decimal. */
    for (Loop1UInt16 = 0; Loop1UInt16 < DisplayLength; ++Loop1UInt16) printf("%3u  ",    Loop1UInt16);
    printf("\n");

    /* Display Topic hex values. */
    for (Loop1UInt16 = 0; Loop1UInt16 < DisplayLength; ++Loop1UInt16) printf("0x%2.2X ", StructMQTT.Topic[Loop1UInt16]);
    printf("\n");

    /* Display Topic characters. */
    for (Loop1UInt16 = 0; Loop1UInt16 < DisplayLength; ++Loop1UInt16)
    {
      if (isprint(StructMQTT.Topic[Loop1UInt16]))
        printf("  %c  ",   StructMQTT.Topic[Loop1UInt16]);
      else
        printf("  -  ");
    }
    printf("\n");

    log_printf(__LINE__, __func__, "========================================================================================================================\n");

    /* Display content of all sub-topics null string or not. */
    for (Loop1UInt16 = 0; Loop1UInt16 < MAX_SUB_TOPICS; ++Loop1UInt16)
    {
      log_printf(__LINE__, __func__, "SubTopic[%2u] (0x%p) -> <0x%p>   <%s>\n", Loop1UInt16, &StructMQTT.SubTopic[Loop1UInt16], StructMQTT.SubTopic[Loop1UInt16], StructMQTT.SubTopic[Loop1UInt16]);
    }
  }


  /* Display non-null sub-topics. */
  for (Loop1UInt16 = 0; Loop1UInt16 < MAX_SUB_TOPICS; ++Loop1UInt16)
  {
    if ((StructMQTT.SubTopic[Loop1UInt16][0]) && (StructMQTT.SubTopic[Loop1UInt16][0] != 0x0D))
      log_printf(__LINE__, __func__, "%2u) <%s>\n", Loop1UInt16 + 1, StructMQTT.SubTopic[Loop1UInt16]);
    else
      break;
  }

  log_printf(__LINE__, __func__, "========================================================================================================================\n");

  return;
}





/* $PAGE */
/* $TITLE=mqtt_incoming_publish_cb() */
/* ============================================================================================================================================================= *\
                                                         Callback to receive a validation of a MQTT publish.
                          NOTE: This callback receives the topic only. <mqtt_incoming_data_cb()> is in charge of receiving the payload.
\* ============================================================================================================================================================= */
void mqtt_incoming_publish_cb(void *ExtraArgument, const char *Topic, UINT32 PayloadLength)
{
#ifdef RELEASE_VERSION
  UINT8 FlagLocalDebug = FLAG_OFF;  // must be turned OFF at all times
#else  // RELEASE_VERSION
  UINT8 FlagLocalDebug = FLAG_OFF;  // may be turned ON for debug purposes.
#endif  // RELEASE_VERSION

  if (FlagLocalDebug)
  {
    log_printf(__LINE__, __func__, "Entering mqtt_incoming_publish_cb(0x%p).\n", ExtraArgument);
    log_printf(__LINE__, __func__, "Receiving a MQTT message on topic: <%s>.\n", Topic);
    log_printf(__LINE__, __func__, "StructMQTT: %p   ExtraArgument: %p   *Topic: %p   Topic: <%s>   Payload length: %lu\n", &StructMQTT, ExtraArgument, Topic, Topic, PayloadLength);
    log_printf(__LINE__, __func__, "========================================================================================================================\n");
    log_printf(__LINE__, __func__, "Topic: <%s>.\n", Topic);
  }

  /* Wipe MQTT packet currently containing the data of the previous MQTT packet received and keep track of the new topic data space. */
  mqtt_wipe_packet();
  strcpy(StructMQTT.Topic, Topic);
  if (StructMQTT.mqtt_status) StructMQTT.mqtt_status(MQTT_RECEIVE_TOPIC);

  if (FlagLocalDebug) log_printf(__LINE__, __func__, "Exiting mqtt_incoming_publish_cb().\n");

  return;
}





/* $PAGE */
/* $TITLE=mqtt_init() */
/* ============================================================================================================================================================= *\
                                                                     Initialize MQTT session.
\* ============================================================================================================================================================= */
INT16 mqtt_init(void)
{
  /* Copy Unique ID and Device ID to structure MQTT. */
  strcpy(StructMQTT.PicoUniqueId,   PicoUniqueId);
  strcpy(StructMQTT.PicoIdentifier, PicoIdentifier);

  /* Allocate memory for a new structure MqttClientInstance if this has not been done previously. */
  if (!StructMQTT.MqttClientInstance)
  {
    StructMQTT.MqttClientInstance = mqtt_client_new();
    if (!StructMQTT.MqttClientInstance)
    {
      log_printf(__LINE__, __func__, "Error while trying to create an MQTT client instance.\n");
      return -1;
    }
    else
    {
      log_printf(__LINE__, __func__, "Successful creation of a new MQTT client instance (0x%p).\n", StructMQTT.MqttClientInstance);
    }
  }
  else
  {
    log_printf(__LINE__, __func__, "MQTT client instance already exists (0x%p).\n", StructMQTT.MqttClientInstance);
  }

  return 0;
}





/* $PAGE */
/* $TITLE=mqtt_parse_item() */
/* ============================================================================================================================================================= *\
                                                           Parse topic or payload into its sub-components.
\* ============================================================================================================================================================= */
void mqtt_parse_item(UINT8 ParseUnit)
{
#ifdef RELEASE_VERSION
  UINT8 FlagLocalDebug = FLAG_OFF;  // must be turned OFF at all time.
#else   // RELEASE_VERSION
  UINT8 FlagLocalDebug = FLAG_OFF;  // may be turned ON for debug purposes.
#endif  // RELEASE_VERSION

  UCHAR  *DataSpace;        // pointer to main topic string or payload string.
  UCHAR   UnitName[10];     // will be "topic" or "payload".
  UCHAR   SubUnitName[15];  // will be "sub-topic" or "sub-payload".
  UCHAR   ParseCharacter;   // "slash" character, as per MQTT convention.
  UCHAR **SubItem;          // pointer to a pointer of the sub-item being decoded.

  UINT8 FlagFirst;          // indicate that we are decoding the very first sub-item of the main string.

  UINT16 ItemNumber;        // correspond to the sub-item number we are currently decoding.
  UINT16 Loop1UInt16;
  UINT16 Loop2UInt16;
  UINT16 MaxCount;          // maximum number of sub-topics or sub-payloads possible.
  UINT16 MaxLength;         // maximum length of the main string (either topic or payload).


  /* Initializations. */
  ParseCharacter = '/';      // compliant to MQTT naming convention.
  ItemNumber     = 0;        // we will process the first sub-item number (number 0) on entry.
  FlagFirst      = FLAG_ON;  // first valid character that we read will be the pointer to the first sub-topic or sub-payload.

  if (ParseUnit == PARSE_TOPIC)
  {
    /* Assign parameters for topics if we are decoding the main "topic" string. */
    MaxCount  = MAX_SUB_TOPICS;
    MaxLength = MAX_TOPIC_LENGTH;
    DataSpace = (UCHAR *)&StructMQTT.Topic;
    SubItem   = (UCHAR **)&StructMQTT.SubTopic;
    strcpy(UnitName, "topic");
    strcpy(SubUnitName, "sub-topic");
  }
  else
  {
    /* Assign parameters for payloads if we are decoding the main "payload" string. */
    MaxCount  = MAX_SUB_PAYLOADS;
    MaxLength = MAX_PAYLOAD_LENGTH;
    DataSpace = (UCHAR *)&StructMQTT.Payload;
    SubItem   = (UCHAR **)&StructMQTT.SubPayload;
    strcpy(UnitName, "payload");
    strcpy(SubUnitName, "sub-payload");
  }


  if (FlagLocalDebug)
  {
    log_printf(__LINE__, __func__, "========================================================================================================================\n");
    log_printf(__LINE__, __func__, "<120>Entering function: mqtt_parse_item() to decode %s\n", SubUnitName);
    log_printf(__LINE__, __func__, "========================================================================================================================\n");
    log_printf(__LINE__, __func__, "Address of StructMQTT.Topic:      0x%p\n", (UCHAR *)&StructMQTT.Topic);
    log_printf(__LINE__, __func__, "Address of StructMQTT.SubTopic:   0x%p\n", (UCHAR *)&StructMQTT.SubTopic);
    log_printf(__LINE__, __func__, "Address of StructMQTT.Payload:    0x%p\n", (UCHAR *)&StructMQTT.Payload);
    log_printf(__LINE__, __func__, "Address of StructMQTT.SubPayload: 0x%p\n", (UCHAR *)&StructMQTT.SubPayload);
    log_printf(__LINE__, __func__, "========================================================================================================================\n");

    /* Optionally display main topic string or payload string on entry. */
    if (ParseUnit == PARSE_TOPIC)
      log_printf(__LINE__, __func__, "Main Topic string: <%s>\n", StructMQTT.Topic);
    else
      log_printf(__LINE__, __func__, "Main Payload string: <%s>\n", StructMQTT.Payload);
    log_printf(__LINE__, __func__, "========================================================================================================================\n");
  }


  /* Extract all sub-topics or sub-payloads by replacing every slash character by an end-of-string character and keeping a pointer to the sub-string. */
  for (Loop1UInt16 = 0; Loop1UInt16 < MaxLength; ++Loop1UInt16)
  {
    if (FlagLocalDebug)
    {
      if (isprint(DataSpace[Loop1UInt16]))
        log_printf(__LINE__, __func__, "ItemNumber: %2u - Processing character at address 0x%p - %3u <%c>\n",       ItemNumber, &DataSpace[Loop1UInt16], Loop1UInt16, DataSpace[Loop1UInt16]);
      else
        log_printf(__LINE__, __func__, "ItemNumber: %2u - Processing character at address 0x%p - %3u <0x%2.2X>\n",  ItemNumber, &DataSpace[Loop1UInt16], Loop1UInt16, DataSpace[Loop1UInt16]);
    }


    /* Processing a ParseCharacter from topic or payload main string. */
    if (DataSpace[Loop1UInt16] == ParseCharacter)
    {
      /* Processing a ParseCharacter (slash). */
      FlagFirst = FLAG_ON;  // next valid character will be the first character (pointer) to next sub-item string.

      if (Loop1UInt16 == 0)
      {
        /* If a slash is encountered as the very first character of the topic or payload main string. */
        if (FlagLocalDebug) log_printf(__LINE__, __func__, "Skipping first leading slash without incrementing %s number.\n", SubUnitName);
        continue;  // skip first leading slash.
      }
      else
      {
        /* The slash is not the first character of the topic or payload data space, proceed with decoding of next sub-item. */
        DataSpace[Loop1UInt16] = '\0';  // replace ParseCharacter (slash) by an end-of-string character in the topic or payload main string.
        ++ItemNumber;
        if (ItemNumber >= MaxCount)
        {
          /* Check if more data is available in the topic or payload data space. */
          if (DataSpace[Loop1UInt16 + 1])
          {
            /* There is still data available in the topic or payload data space, but we reached the maximum number of sub-topic or sub-payload items. */
            if (FlagLocalDebug) log_printf(__LINE__, __func__, "Too many %ss have been sent in this packet. Reached %2u %ss while maximum is %u\n", SubUnitName, ItemNumber, SubUnitName, MaxCount);
            break;
          }
        }
        else
        {
          if (FlagLocalDebug)
          {
            log_printf(__LINE__, __func__, "End processing of item number %u proceed with decoding of next item (item number: %2u)\n", ItemNumber - 1, ItemNumber);
            log_printf(__LINE__, __func__, "----->  %s number %2.2u (0x%p) has been set to 0x%p <%s>\n\n", SubUnitName, ItemNumber - 1, SubItem - 1, *(SubItem - 1), *(SubItem - 1));
          }
        }
        continue;
      }
    }
    else
    {
      /* Topic must be an ASCII string compliant to MQTT naming convention. When reaching end-of-string, get out of <for> loop.
         Note: As per MQTT standard, Payload could be binary data, but for our own deployment and for now, consider payload is also an ASCII string. */
      if ((DataSpace[Loop1UInt16] == '\0') && (ParseUnit == PARSE_TOPIC))
      {
        if (FlagLocalDebug)
        {
          log_printf(__LINE__, __func__, "End-of-string found. End processing of %s number %u\n", SubUnitName, ItemNumber);
          log_printf(__LINE__, __func__, "----->  %s number %2.2u (0x%p) has been set to 0x%p <%s>\n\n", SubUnitName, ItemNumber, SubItem - 1, *(SubItem - 1), *(SubItem - 1));
        }
        break;
      }

      /* This block allows processing of payload as an ASCII string. When reaching end-of-string, get out of <for> loop. */
      /* NOTE: If payload is made of binary data, section below must be adapted accordingly. */
      if ((DataSpace[Loop1UInt16] == '\0') && (ParseUnit == PARSE_PAYLOAD))
      {
        if (FlagLocalDebug)
        {
          log_printf(__LINE__, __func__, "End-of-string found. End processing of %s number %u\n", SubUnitName, ItemNumber);
          log_printf(__LINE__, __func__, "----->  %s number %2.2u (0x%p) has been set to 0x%p <%s>\n\n", SubUnitName, ItemNumber, SubItem - 1, *(SubItem - 1), *(SubItem - 1));
        }
        break;
      }

      /* Processing first valid character after a ParseCharacter. */
      if (FlagFirst == FLAG_ON)
      {
        FlagFirst = FLAG_OFF;
        *SubItem = &DataSpace[Loop1UInt16];
        if (FlagLocalDebug) log_printf(__LINE__, __func__, "Assigning %s %2.2u (address 0x%p) to pointer: 0x%p in %s main string.\n", SubUnitName, ItemNumber, SubItem, &DataSpace[Loop1UInt16], UnitName);
        ++SubItem;
      }
      continue;
    }
  }

  if (FlagLocalDebug)
  {
    if (ParseUnit == PARSE_TOPIC)
    {
      /* Optionally display all sub-topics when done. */
      log_printf(__LINE__, __func__, "========================================================================================================================\n");
      for (Loop2UInt16 = 0; Loop2UInt16 < MAX_SUB_TOPICS; ++Loop2UInt16)
        log_printf(__LINE__, __func__, "SubTopic[%2.2u]:         0x%p     0x%p   <%s>\n", Loop2UInt16, &StructMQTT.SubTopic[Loop2UInt16], StructMQTT.SubTopic[Loop2UInt16], StructMQTT.SubTopic[Loop2UInt16]);
      log_printf(__LINE__, __func__, " \n");
      log_printf(__LINE__, __func__, "========================================================================================================================\n");
    }
    else
    {
      /* Optionally display all sub-payloads when done. */
      /* NOTE: <for-loop> below may print garbage is payload is made of binary data. */
      log_printf(__LINE__, __func__, "========================================================================================================================\n");
      for (Loop2UInt16 = 0; Loop2UInt16 < MAX_SUB_PAYLOADS; ++Loop2UInt16)
        log_printf(__LINE__, __func__, "SubPayload[%2.2u]:       0x%p     0x%p   <%s>\n", Loop2UInt16, &StructMQTT.SubPayload[Loop2UInt16], StructMQTT.SubPayload[Loop2UInt16], StructMQTT.SubPayload[Loop2UInt16]);
      log_printf(__LINE__, __func__, "========================================================================================================================\n");
    }
  }

  return;
}





/* $PAGE */
/* $TITLE=mqtt_pub_request_cb() */
/* ============================================================================================================================================================= *\
                                                         Callback to receive the response of a publish request.
\* ============================================================================================================================================================= */
void mqtt_pub_request_cb(void *ExtraArgument, err_t Result)
{
#ifdef RELEASE_VERSION
  UINT8 FlagLocalDebug = FLAG_OFF;  // must be turned OFF at all time.
#else   // RELEASE_VERSION
  UINT8 FlagLocalDebug = FLAG_OFF;  // may be turned ON for debug purposes.
#endif  // RELEASE_VERSION

  UINT16 PublishResult;


  if (FlagLocalDebug) log_printf(__LINE__, __func__, "Entering mqtt_pub_request_cb(0x%p).\n", ExtraArgument);

  if (Result)
  {
    PublishResult = MQTT_PUBLISH_ERROR;
    log_printf(__LINE__, __func__, "Error while trying to publish to Topic: <%s>   Payload: <%s>   (ReturnCode: %d)\n", StructMQTT.Topic, StructMQTT.Payload, Result);
    log_printf(__LINE__, __func__, "========================================================================================================================\n");
  }
  else
  {
    PublishResult = MQTT_PUBLISH_OK;
    if (FlagLocalDebug)
    {
      log_printf(__LINE__, __func__, "Successfully published to Topic: <%s>   Payload: <%s>   (ReturnCode: %d)\n", StructMQTT.Topic, StructMQTT.Payload, Result);
      log_printf(__LINE__, __func__, "========================================================================================================================\n");
    }
  }

  /* Check if we shall return the outcome of the MQTT publish to caller. */
  if (StructMQTT.mqtt_status) StructMQTT.mqtt_status(PublishResult);

  if (FlagLocalDebug) log_printf(__LINE__, __func__, "Exiting mqtt_pub_request_cb().\n\n");

  return;
}





/* $PAGE */
/* $TITLE=mqtt_sub_request_cb() */
/* ============================================================================================================================================================= *\
                                                       Callback to receive the response to a subscribe request.
\* ============================================================================================================================================================= */
void mqtt_sub_request_cb(void *ExtraArgument, err_t Result)
{
#ifdef RELEASE_VERSION
  UINT8 FlagLocalDebug = FLAG_OFF;  // must be turned OFF at all time.
#else   // RELEASE_VERSION
  UINT8 FlagLocalDebug = FLAG_OFF;  // may be turned ON for debug purposes.
#endif  // RELEASE_VERSION

  UINT16 SubscribeResult;


  if (FlagLocalDebug) log_printf(__LINE__, __func__, "Entering mqtt_sub_request_cb(0x%p).\n", ExtraArgument);

  switch (StructMQTT.FlagSubscribe)
  {
    case (FLAG_ON):
      if (Result == 0)
      {
        SubscribeResult = MQTT_SUBSCRIBE_OK;
        if (FlagLocalDebug)
        {
          log_printf(__LINE__, __func__, "Successfully subscribed to topic <%s>   ExtraArgument: 0x%p   (ReturnCode: %d)\n", StructMQTT.Topic, ExtraArgument, Result);
          log_printf(__LINE__, __func__, "========================================================================================================================\n");
        }
      }
      else
      {
        SubscribeResult = MQTT_SUBSCRIBE_ERROR;
        log_printf(__LINE__, __func__, "Error while trying to subscribe to topic <%s>   ExtraArgument: 0x%p   (ReturnCode: 0x%X)\n", StructMQTT.Topic, ExtraArgument, Result);
        log_printf(__LINE__, __func__, "========================================================================================================================\n");
      }
    break;

    case (FLAG_OFF):
      if (Result == 0)
      {
        SubscribeResult = MQTT_UNSUBSCRIBE_OK;
        if (FlagLocalDebug)
        {
          log_printf(__LINE__, __func__, "Successfully unsubscribed from topic <%s>   ExtraArgument: 0x%p   (ReturnCode: %d)\n\n", StructMQTT.Topic, ExtraArgument, Result);
          log_printf(__LINE__, __func__, "========================================================================================================================\n");
        }
      }
      else
      {
        SubscribeResult = MQTT_UNSUBSCRIBE_ERROR;
        log_printf(__LINE__, __func__, "Error while trying to unsubscribe from topic <%s>   ExtraArgument: 0x%p   (ReturnCode: 0x%X)\n\n", StructMQTT.Topic, ExtraArgument, Result);
        log_printf(__LINE__, __func__, "========================================================================================================================\n");
      }
    break;

    default:
      if (Result == 0)
      {
        SubscribeResult = MQTT_SUB_UNSUB_OK;
        if (FlagLocalDebug)
        {
          log_printf(__LINE__, __func__, "Successfully subscribed or unsubscribed to topic <%s>   (ReturnCode: %d)\n", StructMQTT.Topic, Result);
          log_printf(__LINE__, __func__, "You should turn <StructMQTT.FlagSubscribe> On or Off before your request.\n\n");
          log_printf(__LINE__, __func__, "========================================================================================================================\n");
        }
      }
      else
      {
        SubscribeResult = MQTT_SUB_UNSUB_ERROR;
        log_printf(__LINE__, __func__, "Error while trying to subscribe or unsubscribe to topic <%s>   (ReturnCode: 0x%X)\n", StructMQTT.Topic, Result);
        log_printf(__LINE__, __func__, "You should turn <StructMQTT.FlagSubscribe> On or Off before your request.\n\n");
        log_printf(__LINE__, __func__, "========================================================================================================================\n");
      }
    break;
  }

  if (StructMQTT.mqtt_status) StructMQTT.mqtt_status(SubscribeResult);

  if (FlagLocalDebug) log_printf(__LINE__, __func__, "Exiting mqtt_sub_request_cb().\n\n");

  return;
}





/* $PAGE */
/* $TITLE=mqtt_wipe_packet() */
/* ============================================================================================================================================================= *\
                                                       Wipe MQTT packet in preparation for next reception.
\* ============================================================================================================================================================= */
void mqtt_wipe_packet(void)
{
  /* Wipe Topic data space. */
  memset(&StructMQTT.Topic, 0x00, MAX_TOPIC_LENGTH);

  /* Wipe all subtopics. */
  memset(&StructMQTT.SubTopic, 0x00, sizeof(UCHAR *) * MAX_SUB_TOPICS);

  /* Wipe Payload data space. */
  memset(&StructMQTT.Payload, 0x00, MAX_PAYLOAD_LENGTH);

  /* Wipe all subpayload. */
  memset(&StructMQTT.SubPayload, 0x00, sizeof(UCHAR *) * MAX_SUB_PAYLOADS);

  return;
}

