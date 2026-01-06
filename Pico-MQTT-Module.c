/* ============================================================================================================================================================= *\
   Pico-MQTT-Module.c
   St-Louys Andre - May 2025
   astlouys@gmail.com
   https://github.com/astlouys/Pico-MQTT-Module
   Revision 07-DEC-2025
   Langage: C
   Version 2.04

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
                    - Adapted for the new updates done to Pico-MQTT-Module.
\* ============================================================================================================================================================= */



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
Pin 20 - GPIO 15  - Button 0 (on Waveshare's 2-inch LCD display)                                           Pimoroni Explorer Base: ButtonY.
Pin 21 - GPIO 16  - Reserved for infrared sensor / receiver (VS1838)                                       Pimoroni Explorer Base: SPI MISO.
Pin 22 - GPIO 17  - Button 1 (on Waveshare's 2-inch LCD display)                                           Pimoroni Explorer Base: LCD Chip Select.
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
#define RELEASE_VERSION  ///
#define FRENCH



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
                                                               Enter time of end of MQTT breakdown.
\* ============================================================================================================================================================= */
void mqtt_breakdown_end(datetime_t EndTime)
{
#ifdef RELEASE_VERSION
  UINT8 FlagLocalDebug = FLAG_OFF;  // must remain OFF all time.
#else   // RELEASE_VERSION
  UINT8 FlagLocalDebug = FLAG_OFF;  // may be turned ON for debug purposes.
#endif  // RELEASE_VERSION

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
               EndTime.day,
               EndTime.month,
               EndTime.year,
               EndTime.hour,
               EndTime.min,
               EndTime.sec);
  }

  return;
}





/* $PAGE */
/* $TITLE=mqtt_breakdown_start() */
/* ============================================================================================================================================================= *\
                                                         Keep track of the time of beginning of MQTT breakdown.
\* ============================================================================================================================================================= */
void mqtt_breakdown_start(datetime_t StartTime)
{
#ifdef RELEASE_VERSION
  UINT8 FlagLocalDebug = FLAG_OFF;  // must remain OFF all time.
#else   // RELEASE_VERSION
  UINT8 FlagLocalDebug = FLAG_OFF;  // may be turned ON for debug purposes.
#endif  // RELEASE_VERSION

  UINT16 Loop1UInt16;
  UINT16 Loop2UInt16;

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
               StartTime.day,
               StartTime.month,
               StartTime.year,
               StartTime.hour,
               StartTime.min,
               StartTime.sec);

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
  UINT8 FlagLocalDebug = FLAG_OFF;  // must remain Off at all time.
#else   // RELEASE_VERSION
  UINT8 FlagLocalDebug = FLAG_OFF;  // may be turned On for debugging purposes.
#endif  // RELEASE_VERSION

  static UINT16 RetryCycles = 4;
  static UINT32 MQTTCycles15Sec;  // cumulative number of MQTT down 15-seconds cycles

  datetime_t CurrentTime;


  if (FlagLocalDebug)
  {
    /* Optionally display debug information on entry. */
    log_printf(__LINE__, __func__, "Entering mqtt_check_connection()   Wi-Fi health: 0x%2.2X   StructMQTT.MqttClientInstance: 0x%p\n", FlagWiFiHealth, StructMQTT.MqttClientInstance);
    log_printf(__LINE__, __func__, "StructMQTT.FlagHealth: %u   FlagStartupOver: %u   MQTTCycles15Sec: %u   RetryCycles: %u\n", StructMQTT.FlagHealth, StructMQTT.FlagStartupOver, MQTTCycles15Sec, RetryCycles);
    if (StructMQTT.MqttClientInstance)
    {
      if (mqtt_client_is_connected(StructMQTT.MqttClientInstance))
        log_printf(__LINE__, __func__, "MQTT client is connected to MQTT broker: %d\n", mqtt_client_is_connected(StructMQTT.MqttClientInstance));
      else
        log_printf(__LINE__, __func__, "MQTT client is NOT connected to MQTT broker: %d\n", mqtt_client_is_connected(StructMQTT.MqttClientInstance));
    }
  }


  if ((StructMQTT.MqttClientInstance) && (mqtt_client_is_connected(StructMQTT.MqttClientInstance)))
  {
    /* MQTT connection is OK. */
    if (StructMQTT.FlagHealth == FLAG_OFF)
    {
      /* MQTT connection was down during previous cycle, so it has just been restored. */
      StructMQTT.FlagHealth = FLAG_ON;
      MQTTCycles15Sec       = 0l;  // reset cumulative number of wait cycles.
      if (FlagLocalDebug) log_printf(__LINE__, __func__, "************************  MQTT client connection has just been restored.\n");
      log_printf(__LINE__, __func__, "*********************** Returning error code 1.\n");
      return 1;  // indicate to caller that MQTT connection has just been established / restored.
    }
    else
    {
      if (FlagLocalDebug)
      {
        log_printf(__LINE__, __func__, "MQTT client connection is OK.\n");
        log_printf(__LINE__, __func__, "Returning error code 0.\n");
      }
      return 0;
    }
  }
  else
  {
    /* MQTT client is disconnected from broker. */
    log_printf(__LINE__, __func__, "MQTT client is disconnected from broker.\n");
    if (StructMQTT.FlagHealth == FLAG_ON)
    {
      /* Connection with broker was still good during previous cycle... make sure current MQTT parameters are cleaned. */
      if (FlagLocalDebug) log_printf(__LINE__, __func__, "MQTT connection was good during previous cycle, time stamp MQTT breakdown start.\n");
      StructMQTT.FlagHealth = FLAG_OFF;

      if (StructMQTT.MqttClientInstance)
      {
        mqtt_disconnect(StructMQTT.MqttClientInstance);
        mqtt_client_free(StructMQTT.MqttClientInstance);
        StructMQTT.MqttClientInstance = NULL;
      }

      /* Since MQTT connection was still good during previous cycle, this is the beginning of a new MQTT breakdown period. */
      ++StructMQTT.TotalErrors;

      /* Keep track of time at beginning of breakdown. */
      rtc_get_datetime(&CurrentTime);
      mqtt_breakdown_start(CurrentTime);
    }


    if (FlagWiFiHealth == FLAG_ON)
    {
      /* Wi-Fi connection is OK, problem is only with MQTT connection. */
      ++MQTTCycles15Sec;  // one more 15-seconds cycle with a bad MQTT connection, will rollover to 0 when reaching end of UINT32 range if ever the case.
      if (FlagLocalDebug) log_printf(__LINE__, __func__, "Wi-Fi health is OK, increment MQTT cycles before next retry (%lu).\n", MQTTCycles15Sec);

      /* Try to reconnect with MQTT broker every once in a while. */
      if ((!(MQTTCycles15Sec % RetryCycles)) || (StructMQTT.FlagStartupOver == FLAG_OFF))
      {
        if (FlagLocalDebug) log_printf(__LINE__, __func__, "FlagStartupOver: %u or MQTTCycles15Sec divisible by 4: %u ---> retry MQTT connection.\n", StructMQTT.FlagStartupOver, MQTTCycles15Sec);
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
            if (StructMQTT.FlagStartupOver == FLAG_ON)
            {
              if (StructMQTT.BreakdownStart[0].day == 0)
              {
                log_printf(__LINE__, __func__, "There is currently no MQTT breakdown start time logged, it must be the first MQTT connect sequence during startup.\n");
              }
              else
              {
                log_printf(__LINE__, __func__, "There is a MQTT breakdown start time logged, so log the breakdown end time.\n");
                rtc_get_datetime(&CurrentTime);
                mqtt_breakdown_end(CurrentTime);
                if (FlagLocalDebug) log_printf(__LINE__, __func__, "MQTT client connection has just been restored.\n");
              }
            }
            else
            {
              StructMQTT.FlagStartupOver = FLAG_ON; 
            }
            StructMQTT.FlagHealth = FLAG_ON;
            MQTTCycles15Sec = 0l;
            log_printf(__LINE__, __func__, "Returning error code 1.\n");
            return 1;  // indicate that connection with broker has just been restored.
          }
          else
          {
            StructMQTT.FlagHealth = FLAG_OFF;
          }
        }
      }
    }
  }
  log_printf(__LINE__, __func__, "Returning error code -1.\n");
  return -1;
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
  if (StructMQTT.MqttClientInstance == NULL)
  {
    log_printf(__LINE__, __func__, "MQTT client is disconnected from MQTT broker\n");
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

#if 0
    log_printf(__LINE__, __func__, "MqttClientInstance.cyclic_tick <%u>\n", StructMQTT.MqttClientInstance->cyclic_tick);
    printf("[%5u] - cyclic_tick: %u\n",     __LINE__, StructMQTT.MqttClientInstance->cyclic_tick);
    printf("[%5u] - keep_alive: %u\n",      __LINE__, MqttClientInstance->keep_alive);
    printf("[%5u] - server_watchdog: %u\n", __LINE__, MqttClientInstance->server_watchdog);
    printf("[%5u] - \n");

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
  }

  log_printf(__LINE__, __func__, "Total unique MQTT error count: %lu\n", StructMQTT.TotalErrors);
  log_printf(__LINE__, __func__, "MQTT broker IP address:        <%s>\n", ip4addr_ntoa(&StructMQTT.BrokerAddress));
  log_printf(__LINE__, __func__, "Pico IP address:               <%s>\n", ip4addr_ntoa(&StructMQTT.PicoIPAddress));
  log_printf(__LINE__, __func__, "Pico Unique ID:                <%s>\n", StructMQTT.PicoUniqueId);
  log_printf(__LINE__, __func__, "Device Identifier:             <%s>\n", StructMQTT.PicoIdentifier);
  log_printf(__LINE__, __func__, "Last Topic:                    <%s>\n", StructMQTT.Topic);
  log_printf(__LINE__, __func__, "Last Payload:                  <%s>\n", StructMQTT.Payload);
  log_printf(__LINE__, __func__, "Last PayloadLength:            <%u>\n", StructMQTT.PayloadLength);

  log_printf(__LINE__, __func__, "========================================================================================================================\n");
  log_printf(__LINE__, __func__, "Last Topic details:\n");
  if (StructMQTT.SubTopic[0][0] == '\0') log_printf(__LINE__, __func__, " <None> received yet.\n");
  for (Loop1UInt16 = 0; Loop1UInt16 < MAX_SUB_TOPICS; ++Loop1UInt16)
  {
    if (StructMQTT.SubTopic[Loop1UInt16][0]) log_printf(__LINE__, __func__, "[%2u] <%s>\n", Loop1UInt16, StructMQTT.SubTopic[Loop1UInt16]);
  }

  log_printf(__LINE__, __func__, "========================================================================================================================\n");
  log_printf(__LINE__, __func__, "Last Payload details:\n");
  if (StructMQTT.SubPayload[0][0] == '\0') log_printf(__LINE__, __func__, " <None> received yet.\n");
  for (Loop1UInt16 = 0; Loop1UInt16 < MAX_SUB_PAYLOADS; ++Loop1UInt16)
  {
        if (StructMQTT.SubPayload[Loop1UInt16][0]) log_printf(__LINE__, __func__, "[%2u] <%s>\n", Loop1UInt16, StructMQTT.SubPayload[Loop1UInt16]);
  }
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
  UINT16 Loop1UInt16;

  log_printf(__LINE__, __func__, "==================================================\n");
  log_printf(__LINE__, __func__, "                   SubPayloads:\n");
  log_printf(__LINE__, __func__, "==================================================\n");
  for (Loop1UInt16 = 0; Loop1UInt16 < MAX_SUB_PAYLOADS; ++Loop1UInt16)
  {
    if ((StructMQTT.SubPayload[Loop1UInt16][0] != '\0') && (StructMQTT.SubPayload[Loop1UInt16][0] != 0x0D))
      log_printf(__LINE__, __func__, "%2u) <%s>\n", Loop1UInt16 + 1, StructMQTT.SubPayload[Loop1UInt16]);
    else
      break;
  }
  log_printf(__LINE__, __func__, "==================================================\n");

  return;
}





/* $PAGE */
/* $TITLE=mqtt_display_topic() */
/* ============================================================================================================================================================= *\
                                                                  Display all current MQTT sub-topics.
\* ============================================================================================================================================================= */
void mqtt_display_topic(void)
{
  UCHAR String[50];

  UINT8 SourceTopic;  // number of the last sub-topic representing the source of MQTT packet as per ASTL ecosystem convention.

  UINT16 Loop1UInt16;


  /* Retrieve source of MQTT packet. */
  /* NOTE: This is an "ASTL Smart Home ecosystem" standard and not a MQTT standard. */
  /* You may want to comment out the blocks below if you use the code for another usage. */
  /* Identify source of the MQTT packet, as per ASTL ecosystem convention. */
  SourceTopic = MAX_SUB_TOPICS;  // initialize with invalid value on entry.
  for (Loop1UInt16 = 0; Loop1UInt16 < sizeof(String); ++Loop1UInt16) String[Loop1UInt16] = '\0';
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
    /* Still contains starting invalid value ? Source of packet has not been found. */
    sprintf(String, "Source of MQTT packet unidentified.");
  }
  else
  {
    sprintf(String, "Source: %s", StructMQTT.SubTopic[SourceTopic]);
  }

  log_printf(__LINE__, __func__, "==================================================\n");
  
  // log_printf(__LINE__, __func__, "sizeof(String): %u\n", sizeof(String));
  // log_printf(__LINE__, __func__, "strlen(String): %u\n", strlen(String));
  // log_printf(__LINE__, __func__, "sizeof(String) - strlen(String): %u\n", sizeof(String) - strlen(String));
  // log_printf(__LINE__, __func__, "((sizeof(String) - strlen(String)) / 2): %u\n", ((sizeof(String) - strlen(String)) / 2));
  log_printf(__LINE__, __func__, "");
  for (Loop1UInt16 = 0; Loop1UInt16 < ((sizeof(String) - strlen(String)) / 2); ++Loop1UInt16) printf(" ");
  printf("%s\n", String);
  
  log_printf(__LINE__, __func__, "                   SubTopics:\n");
  log_printf(__LINE__, __func__, "==================================================\n");
  for (Loop1UInt16 = 0; Loop1UInt16 < MAX_SUB_TOPICS; ++Loop1UInt16)
  {
    if (StructMQTT.SubTopic[Loop1UInt16])
      log_printf(__LINE__, __func__, "%2u) <%s>\n", Loop1UInt16 + 1, StructMQTT.SubTopic[Loop1UInt16]);
    else
      break;
  }
  /// log_printf(__LINE__, __func__, "==================================================\n");

  return;
}





/* $PAGE */
/* $TITLE=mqtt_init() */
/* ============================================================================================================================================================= *\
                                                                     Initialize MQTT session.
\* ============================================================================================================================================================= */
INT16 mqtt_init(void)
{
#ifdef RELEASE_VERSION
  UINT8 FlagLocalDebug = FLAG_OFF;  // must remain OFF at all time.
#else   // RELEASE_VERSION
  UINT8 FlagLocalDebug = FLAG_OFF;  // may be modified for debug purposes.
#endif  // RELEASE_VERSION

  /* Copy Unique ID and Device ID to structure MQTT. */
  strcpy(StructMQTT.PicoUniqueId, PicoUniqueId);
  strcpy(StructMQTT.PicoIdentifier, PicoIdentifier);

  /* Allocate memory for a new structure MqttClientInstance. */
  StructMQTT.MqttClientInstance = mqtt_client_new();
  if (!StructMQTT.MqttClientInstance)
  {
    log_printf(__LINE__, __func__, "Error while trying to create an MQTT client instance.\n");
    log_printf(__LINE__, __func__, "Aborting Firmware...\n");
    return -1;
  }

  if (FlagLocalDebug) log_printf(__LINE__, __func__, "Successful creation of a new MQTT client instance (0x%p).\n", StructMQTT.MqttClientInstance);

  return 0;
}





/* $PAGE */
/* $TITLE=mqtt_parse_topic() */
/* ============================================================================================================================================================= *\
                                                        Parse topic or payload into its components.
\* ============================================================================================================================================================= */
void mqtt_parse_topic(UINT8 ParseUnit)
{
#ifdef RELEASE_VERSION
  UINT8 FlagLocalDebug = FLAG_OFF;  // must remain OFF all time.
#else   // RELEASE_VERSION
  UINT8 FlagLocalDebug = FLAG_OFF;  // may be modified for debug purposes.
#endif  // RELEASE_VERSION

  UCHAR *DataSpace;
  UCHAR  UnitName[10];
  UCHAR  SubUnitName[15];
  UCHAR  ParseCharacter;
  UCHAR **SubItem;

  UINT8 FlagFirst;
  UINT8 Loop2UInt8;

  UINT16 ItemNumber;
  UINT16 Loop1UInt16;
  UINT16 MaxCount;
  UINT16 MaxLength;


  /* Initializations. */
  if (ParseUnit == PARSE_TOPIC)
  {
    MaxCount  = MAX_SUB_TOPICS;
    MaxLength = MAX_TOPIC_LENGTH;
    DataSpace = (UCHAR *)&StructMQTT.Topic;
    SubItem   = (UCHAR **)&StructMQTT.SubTopic;
    strcpy(UnitName, "topic");
    strcpy(SubUnitName, "sub-topic");
  }
  else
  {
    MaxCount  = MAX_SUB_PAYLOADS;
    MaxLength = MAX_PAYLOAD_LENGTH;
    DataSpace = (UCHAR *)&StructMQTT.Payload;
    SubItem   = (UCHAR **)&StructMQTT.SubPayload;
    strcpy(UnitName, "payload");
    strcpy(SubUnitName, "sub-payload");
  }

  ParseCharacter = '/';      // compliant to MQTT naming convention.
  ItemNumber     = 0;        // we will process the first sub-item number (number 0) on entry.
  FlagFirst      = FLAG_ON;  // first valid character that we read will be the pointer to the first sub-topic or sub-payload.


  if (FlagLocalDebug)
  {
    log_printf(__LINE__, __func__, "====================================================================\n");
    log_printf(__LINE__, __func__, "Entering mqtt_parse_topic() to decode %s\n", SubUnitName);
    log_printf(__LINE__, __func__, "====================================================================\n");
    log_printf(__LINE__, __func__, "(UCHAR *)&StructMQTT.Topic:      0x%p\n", (UCHAR *)&StructMQTT.Topic);
    log_printf(__LINE__, __func__, "(UCHAR *)&StructMQTT.SubTopic:   0x%p\n", (UCHAR *)&StructMQTT.SubTopic);
    log_printf(__LINE__, __func__, "(UCHAR *)&StructMQTT.Payload:    0x%p\n", (UCHAR *)&StructMQTT.Payload);
    log_printf(__LINE__, __func__, "(UCHAR *)&StructMQTT.SubPayload: 0x%p\n", (UCHAR *)&StructMQTT.SubPayload);
    log_printf(__LINE__, __func__, "\n");

    /* Optionally display main display topic or payload on entry. */
    if (ParseUnit == PARSE_TOPIC)
      log_printf(__LINE__, __func__, "Topic string: <%s>\n", StructMQTT.Topic);
    else
      log_printf(__LINE__, __func__, "Payload string: <%s>\n", StructMQTT.Payload);

    log_printf(__LINE__, __func__, "====================================================================\n");

    /* Optionally display all sub-topics on entry. */
    for (Loop2UInt8 = 0; Loop2UInt8 < MAX_SUB_TOPICS; ++Loop2UInt8)
      log_printf(__LINE__, __func__, "StructMQTT.SubTopic[%2.2u]:         0x%p     0x%p   <%s>\n", Loop2UInt8, &StructMQTT.SubTopic[Loop2UInt8], StructMQTT.SubTopic[Loop2UInt8], StructMQTT.SubTopic[Loop2UInt8]);
    log_printf(__LINE__, __func__, "\n");
  
    log_printf(__LINE__, __func__, "====================================================================\n");

    /* Optionally display all sub-payloads on entry. */
    /* NOTE: <for-loop> below may print garbage is payload is made of binary data. */
    for (Loop2UInt8 = 0; Loop2UInt8 < MAX_SUB_PAYLOADS; ++Loop2UInt8)
      log_printf(__LINE__, __func__, "StructMQTT.SubPayload[%2.2u]:       0x%p     0x%p   <%s>\n", Loop2UInt8, &StructMQTT.SubPayload[Loop2UInt8], StructMQTT.SubPayload[Loop2UInt8], StructMQTT.SubPayload[Loop2UInt8]);

    log_printf(__LINE__, __func__, "====================================================================\n");
  }


  /* Extract all sub-topics or sub-payloads from main topic or payload data space. */
  for (Loop1UInt16 = 0; Loop1UInt16 < MaxLength; ++Loop1UInt16)
  {
    if (FlagLocalDebug)
    {
      if (isprint(DataSpace[Loop1UInt16]))
        log_printf(__LINE__, __func__, "Processing character at address 0x%p - %3u <%c>\n",       &DataSpace[Loop1UInt16], Loop1UInt16, DataSpace[Loop1UInt16]);
      else
        log_printf(__LINE__, __func__, "Processing character at address 0x%p - %3u <0x%2.2X>\n",  &DataSpace[Loop1UInt16], Loop1UInt16, DataSpace[Loop1UInt16]);
    }


    /* Processing any valid character (other than a ParseCharacter) from topic or payload data space. */
    if (DataSpace[Loop1UInt16] != ParseCharacter)
    {
      /* Topic must be an ASCII string compliant to MQTT naming convention. When reaching end-of-string, get out of <for> loop. Note: Payload could be binary data. */
      if ((DataSpace[Loop1UInt16] == '\0') && (ParseUnit == PARSE_TOPIC))
      {
        if (FlagLocalDebug)
        {
          log_printf(__LINE__, __func__, "End-of-string found. End processing of %s number %u\n", SubUnitName, ItemNumber);
          log_printf(__LINE__, __func__, "----->  %s number %2.2u (0x%p) has been set to 0x%p <%s>\n", SubUnitName, ItemNumber, SubItem, *SubItem, *SubItem);
        }
        break;
      }

      /* This block allows processing of payload as an ASCII string. When reaching end-of-string, get out of <for> loop. */
      /* NOTE: If payload is made of binary data, remove the 5 lines below. */
      if ((DataSpace[Loop1UInt16] == '\0') && (ParseUnit == PARSE_PAYLOAD))
      {
        if (FlagLocalDebug)
        {
          log_printf(__LINE__, __func__, "End-of-string found. End processing of %s number %u\n", SubUnitName, ItemNumber);
          log_printf(__LINE__, __func__, "----->  %s number %2.2u (0x%p) has been set to 0x%p <%s>\n", SubUnitName, ItemNumber, SubItem, *SubItem, *SubItem);
        }
        break;
      }

      /* Processing first valid character after a ParseCharacter. */
      if (FlagFirst == FLAG_ON)
      {
        FlagFirst = FLAG_OFF;
        *SubItem = &DataSpace[Loop1UInt16];
        if (FlagLocalDebug) log_printf(__LINE__, __func__, "Assigning %s %2.2u at address 0x%p to pointer: 0x%p in main %s data space.\n", SubUnitName, ItemNumber, SubItem, &DataSpace[Loop1UInt16], UnitName);
        ++SubItem;
      }
      continue;
    }



    /* Processing a ParseCharacter (slash). */
    if (Loop1UInt16 == 0)
    {
      /* If a slash is encountered as the very first character of the topic or payload data space. */
      if (FlagLocalDebug) log_printf(__LINE__, __func__, "Skipping first leading slash without incrementing %s number.\n", SubUnitName);
      continue;  // skip first leading slash.
    }
    else
    {
      /* The slash is not the first character of the topic or payload data space, proceed with decoding of next sub-item. */
      ++ItemNumber;
      FlagFirst = FLAG_ON;  // next valid character will be the pointer to next sub-item.
      DataSpace[Loop1UInt16] = '\0';  // replace ParseCharacter (slash) by an end-of-string character in the whole topic or payload main data space.
      if (ItemNumber >= MaxCount)
      {
        /* Check if more data is available in the topic or payload data space. */
        if (DataSpace[Loop1UInt16 + 1])
        {
          /* There is still data available in the topic or payload data space, but we reached the maximum number of sub-topics or sub-payloads items. */
          if (FlagLocalDebug) log_printf(__LINE__, __func__, "Too many %ss have been sent in this packet. Reached %2u %ss while maximum is %u\n", SubUnitName, ItemNumber, SubUnitName, MaxCount);
          break;
        }
      }
      else
      {
        if (FlagLocalDebug)
        {
          log_printf(__LINE__, __func__, "End processing of item number %u proceed with decoding of next item (item number: %2u)\n", ItemNumber - 1, ItemNumber);
          log_printf(__LINE__, __func__, "----->  %s number %2.2u (0x%p) has been set to 0x%p <%s>\n", SubUnitName, ItemNumber - 1, SubItem - 1, *(SubItem - 1), *(SubItem - 1));
        }

        // if (ItemNumber > 0)
        // {
        //   if (ParseUnit == PARSE_TOPIC)
        //   {
        //     mqtt_display_topic();
        //     // for (Loop2UInt8 = 0; Loop2UInt8 < MAX_SUB_TOPICS; ++Loop2UInt8)
        //     //   log_printf(__LINE__, __func__, "Sub-Topic number %2.2u (0x%p) is 0x%p <%s>\n", Loop2UInt8, &StructMQTT.SubTopic[Loop2UInt8], StructMQTT.SubTopic[Loop2UInt8], StructMQTT.SubTopic[Loop2UInt8]);
        //   }
        //   else
        //   {
        //     mqtt_display_payload();
        //     // for (Loop2UInt8 = 0; Loop2UInt8 < MAX_SUB_PAYLOADS; ++Loop2UInt8)
        //     //   log_printf(__LINE__, __func__, "Sub-Payload number %2.2u (0x%p) is 0x%p <%s>\n", Loop2UInt8, &StructMQTT.SubPayload[Loop2UInt8], StructMQTT.SubPayload[Loop2UInt8], StructMQTT.SubPayload[Loop2UInt8]);
        //   }
      }
      continue;
    }
  }

  // if (FlagLocalDebug)
  // {
  //   log_printf(__LINE__, __func__, "End of processing for %s.\n\n\n\n\n", SubUnitName);
  //   mqtt_display_topic();
  //   mqtt_display_payload();
  // }

  return;
}





/* $PAGE */
/* $TITLE=mqtt_wipe_packet() */
/* ============================================================================================================================================================= *\
                                                       Wipe MQTT packet in preparation for next reception.
\* ============================================================================================================================================================= */
void mqtt_wipe_packet(void)
{
  UCHAR *Dum1Ptr;

  UINT16 Loop1UInt16;


  /* Wipe Topic data space. */
  Dum1Ptr = (UCHAR *)&StructMQTT.Topic;
  for (Loop1UInt16 = 0; Loop1UInt16 < (MAX_TOPIC_LENGTH); ++Loop1UInt16)
    Dum1Ptr[Loop1UInt16] = '\0';

    /* Wipe all subtopics. */
  for (Loop1UInt16 = 0; Loop1UInt16 < (MAX_SUB_TOPICS); ++Loop1UInt16)
    StructMQTT.SubTopic[Loop1UInt16] = NULL;

  /* Wipe Payload data space. */
  Dum1Ptr = (UCHAR *)&StructMQTT.Payload;
  for (Loop1UInt16 = 0; Loop1UInt16 < (MAX_PAYLOAD_LENGTH); ++Loop1UInt16)
    Dum1Ptr[Loop1UInt16] = '\0';

  /* Wipe all subpayload. */
  for (Loop1UInt16 = 0; Loop1UInt16 < (MAX_SUB_PAYLOADS); ++Loop1UInt16)
    StructMQTT.SubPayload[Loop1UInt16] = NULL;

  return;
}

