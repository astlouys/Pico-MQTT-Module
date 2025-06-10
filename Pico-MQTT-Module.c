/* ============================================================================================================================================================= *\
   Pico-MQTT-Module.c
   St-Louys Andre - May 2025
   astlouys@gmail.com
   Revision 21-MAY-2025
   Langage: C
   Version 1.00

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
    21-MAY-2025 1.00 - First version release.
\* ============================================================================================================================================================= */



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
*/



/* $PAGE */
/* $TITLE=Include files. */
/* ============================================================================================================================================================= *\
                                                                          Include files
\* ============================================================================================================================================================= */
#include "baseline.h"
#include "hardware/gpio.h"
#include "hardware/irq.h"
#include "Pico-MQTT-Module.h"
#include "pico/stdlib.h"
#include "stdarg.h"
#include <stdio.h>
#include "string.h"


 
/* $PAGE */
/* $TITLE=Definitions and macros. */
/* ============================================================================================================================================================= *\
                                                                       Definitions and macros.
\* ============================================================================================================================================================= */
#define RELEASE_VERSION  ///


/* $PAGE */
/* $TITLE=Global variables declaration / definition. */
/* ============================================================================================================================================================= *\
                                                            Global variables declaration / definition.
\* ============================================================================================================================================================= */



/* $PAGE */
/* $TITLE=mqtt_display_client() */
/* ============================================================================================================================================================= *\
                                                                 Display MQTT client information.
\* ============================================================================================================================================================= */
void mqtt_display_client(struct struct_mqtt *StructMQTT)
{
  UCHAR String[25];

  UINT16 Loop1UInt16;

  mqtt_client_t *MqttClientInstance;

  printf("\r");
  log_info(__LINE__, __func__, "================================================================\r");
  log_info(__LINE__, __func__, "                        MQTT information\r");
  log_info(__LINE__, __func__, "================================================================\r");
  /* Display MQTT health. */
  if (mqtt_client_is_connected(StructMQTT->MqttClientInstance))
  {
    log_info(__LINE__, __func__, "   MQTT client is connected to MQTT broker.\r");
    log_info(__LINE__, __func__, "   MQTT health:              Good\r");
  }
  else
  {
    log_info(__LINE__, __func__, "   MQTT client is disconnected from MQTT broker\r");
  }

  log_info(__LINE__, __func__, "   Total unique error count: %lu\r", StructMQTT->TotalErrors);
  log_info(__LINE__, __func__, "   MQTT broker IP address:  <%s>\r", ip4addr_ntoa(&StructMQTT->BrokerAddress));
  log_info(__LINE__, __func__, "   Pico IP address:         <%s>\r", ip4addr_ntoa(&StructMQTT->PicoIPAddress));
  log_info(__LINE__, __func__, "   Pico Unique ID:          <%s>\r", StructMQTT->PicoUniqueId);
  log_info(__LINE__, __func__, "   Device Identifier:       <%s>\r", StructMQTT->PicoIdentifier);
  log_info(__LINE__, __func__, "   Last Topic:              <%s>\r", StructMQTT->Topic);
  log_info(__LINE__, __func__, "   Last Payload:            <%s>\r", StructMQTT->Payload);
  log_info(__LINE__, __func__, "   Last PayloadLength:      <%u>\r", StructMQTT->PayloadLength);

  log_info(__LINE__, __func__, "================================================================\r");
  log_info(__LINE__, __func__, "   Last Topic details:\r", __LINE__);
  for (Loop1UInt16 = 0; Loop1UInt16 < MAX_SUB_TOPIC; ++Loop1UInt16)
  {
    log_info(__LINE__, __func__, "   [%2u] <%s>\r", Loop1UInt16, StructMQTT->SubTopic[Loop1UInt16]);
  }

  log_info(__LINE__, __func__, "================================================================\r");
  log_info(__LINE__, __func__, "   Last Payload details:\r");
  for (Loop1UInt16 = 0; Loop1UInt16 < MAX_SUB_PAYLOAD; ++Loop1UInt16)
  {
    log_info(__LINE__, __func__, "   [%2u] <%s>\r", Loop1UInt16, StructMQTT->SubPayload[Loop1UInt16]);
  }

  /// log_info(__LINE__, __func__, "================================================================\r");


#if 0
  printf("[%5u] - cyclic_tick: %u\r",     __LINE__, StructMQTT->MqttClientInstance->cyclic_tick);
  printf("[%5u] - keep_alive: %u\r",      __LINE__, MqttClientInstance->keep_alive);
  printf("[%5u] - server_watchdog: %u\r", __LINE__, MqttClientInstance->server_watchdog);
  printf("[%5u] - \r");

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

  log_info(__LINE__, __func__, "================================================================\r");
  log_info(__LINE__, __func__, "   MQTT client data:\r");
  log_info(__LINE__, __func__, "   =====================\r");

  MqttClientInstance = StructMQTT->MqttClientInstance;
  if (mqtt_client_is_connected(StructMQTT->MqttClientInstance))
    log_info(__LINE__, __func__, "   MQTT client is connected to MQTT broker.\r");
  else
    log_info(__LINE__, __func__, "   MQTT client is disconnected from MQTT broker.\r");
  log_info(__LINE__, __func__, "   MqttClientInfo.client_id:      <%s>\r", StructMQTT->MqttClientInfo.client_id);
  log_info(__LINE__, __func__, "   MqttClientInfo.client_user:    <%s>\r", StructMQTT->MqttClientInfo.client_user);
  log_info(__LINE__, __func__, "   MqttClientInfo.client_pass:    <%s>\r", StructMQTT->MqttClientInfo.client_pass);
  log_info(__LINE__, __func__, "   MqttClientInfo.keep_alive:     <%u>\r", StructMQTT->MqttClientInfo.keep_alive);
  log_info(__LINE__, __func__, "   MqttClientInfo.will_topic:     <%s>\r", StructMQTT->MqttClientInfo.will_topic);
  log_info(__LINE__, __func__, "   MqttClientInfo.will_msg:       <%s>\r", StructMQTT->MqttClientInfo.will_msg);
  log_info(__LINE__, __func__, "   MqttClientInfo.will_qos:       <%u>\r", StructMQTT->MqttClientInfo.will_qos);
  log_info(__LINE__, __func__, "   MqttClientInfo.will_retain:    <%u>\r", StructMQTT->MqttClientInfo.will_retain);
  log_info(__LINE__, __func__, "================================================================\r\r\r");

  return;
}





/* $PAGE */
/* $TITLE=mqtt_display_topic() */
/* ============================================================================================================================================================= *\
                                                                 Display MQTT topic and subtopics.
\* ============================================================================================================================================================= */
void mqtt_display_topic(struct struct_mqtt *StructMQTT)
{
  UINT16 Loop1UInt16;


  log_info(__LINE__, __func__, "Display Topic and SubTopics:\r");
  log_info(__LINE__, __func__, "============================\r");
  for (Loop1UInt16 = 0; Loop1UInt16 < MAX_SUB_TOPIC; ++Loop1UInt16)
    log_info(__LINE__, __func__, "SubTopic %2u: <%s>\r", Loop1UInt16, StructMQTT->SubTopic[Loop1UInt16]);

  printf("\r");
  log_info(__LINE__, __func__, "Display Payload and SubPayloads:\r");
  log_info(__LINE__, __func__, "================================\r");
  for (Loop1UInt16 = 0; Loop1UInt16 < MAX_SUB_PAYLOAD; ++Loop1UInt16)
    log_info(__LINE__, __func__, "SubPayload %2u: <%s>\r", __LINE__, Loop1UInt16, StructMQTT->SubPayload[Loop1UInt16]);

  return;
}





/* $PAGE */
/* $TITLE=mqtt_init() */
/* ============================================================================================================================================================= *\
                                                                Initialize MQTT session.
\* ============================================================================================================================================================= */
INT16 mqtt_init(struct struct_mqtt *StructMQTT)
{
  UCHAR *Dum1Ptr;

  UINT16 StructSize;
  UINT16 Loop1UInt16;
  UINT16 Loop2UInt16;


  /* --------------------------------------------------------------------------------------------------------------------------- *\
                                                     Wipe MQTT structure on entry.
  \* --------------------------------------------------------------------------------------------------------------------------- */
  /* Wipe MQTT structure on entry. */
  Dum1Ptr = (UCHAR *)StructMQTT;
  for (Loop1UInt16 = 0; Loop1UInt16 < sizeof(struct struct_mqtt); ++Loop1UInt16)
    Dum1Ptr[Loop1UInt16] = '\0';


  /* Allocate memory for a new structure MqttClientInstance. */
  StructMQTT->MqttClientInstance = mqtt_client_new();
  if (!StructMQTT->MqttClientInstance)
  {
    log_info(__LINE__, __func__, "Error while trying to create an MQTT client instance.\r");
    log_info(__LINE__, __func__, "Aborting Firmware...\r");
    return 1;
  }

  log_info(__LINE__, __func__, "Successful creation of a new MQTT client instance (0x%p).\r", StructMQTT->MqttClientInstance);

  return 0;
}





/* $PAGE */
/* $TITLE=mqtt_parse_topic() */
/* ============================================================================================================================================================= *\
                                                        Parse topic or payload into its components.
\* ============================================================================================================================================================= */
void mqtt_parse_topic(struct struct_mqtt *StructMQTT, UINT8 ParseUnit)
{
  UCHAR *Dum1Ptr;
  UCHAR  ParseCharacter;

  UINT16 CharCount;
  UINT16 ItemNumber;
  UINT16 Loop1UInt16;
  UINT16 MaxCount;
  UINT16 MaxLength;


  /* Initializations. */
  ParseCharacter = '/';

  switch (ParseUnit)
  {
    case (PARSE_TOPIC):
      log_info(__LINE__, __func__, "Topic string: <%s>\r", StructMQTT->Topic);

      /* Wipe all subtopics on entry. */
      Dum1Ptr = (UCHAR *)&StructMQTT->SubTopic;
      for (Loop1UInt16 = 0; Loop1UInt16 < (MAX_SUB_TOPIC * MAX_TOPIC_LENGTH); ++Loop1UInt16)
        Dum1Ptr[Loop1UInt16] = 0x00;

      MaxCount  = MAX_SUB_TOPIC;
      MaxLength = MAX_TOPIC_LENGTH;
      Dum1Ptr = (UCHAR *)&StructMQTT->Topic;
    break;

    case (PARSE_PAYLOAD):
      log_info(__LINE__, __func__, "Payload string: <%s>\r", StructMQTT->Payload);

      /* Wipe all subpayload on entry. */
      Dum1Ptr = (UCHAR *)&StructMQTT->SubPayload;
      for (Loop1UInt16 = 0; Loop1UInt16 < (MAX_SUB_PAYLOAD * MAX_PAYLOAD_LENGTH); ++Loop1UInt16)
        Dum1Ptr[Loop1UInt16] = 0x00;

      MaxCount  = MAX_SUB_PAYLOAD;
      MaxLength = MAX_PAYLOAD_LENGTH;
      Dum1Ptr = (UCHAR *)&StructMQTT->Payload;
    break;

    default:
      log_info(__LINE__, __func__, "Invalid parameter passed as ParseUnit: %u\r", ParseUnit);
      return;
    break;
  }


  CharCount  = 0;
  ItemNumber = 0;

  for (Loop1UInt16 = 0; Dum1Ptr[Loop1UInt16]; ++Loop1UInt16)
  {
    /// log_info(__LINE__, __func__, "Processing char %2u <%c>\r", Loop1UInt16, Dum1Ptr[Loop1UInt16]);
    if (Dum1Ptr[Loop1UInt16] == ParseCharacter)
    {
      if (Loop1UInt16 == 0)
      {
        /// log_info(__LINE__, __func__, "Skipping very first leading slash.\r");
        continue;  // very first leading slash.
      }
      else
      {
        /* Skip to next subitem. */
        /// log_info(__LINE__, __func__, "Skip to item number %2u\r", ItemNumber + 1);
        ++ItemNumber;
        CharCount = 0;
        if (ItemNumber >= MaxCount) break;
        continue;
      }
    }
    
    switch (ParseUnit)
    {
      case (PARSE_TOPIC):
        StructMQTT->SubTopic[ItemNumber][CharCount] = Dum1Ptr[Loop1UInt16];
        /// log_info(__LINE__, __func__, "Current SubTopic: <%s>\r", StructMQTT->SubTopic[ItemNumber]);
        ++CharCount;
        /* If this subtopic is too long, troncate it. */
        if ((CharCount >= (MaxLength - 1)) && (Dum1Ptr[Loop1UInt16 + 1] != ParseCharacter))
        {
          while ((Dum1Ptr[Loop1UInt16 + 1] != 0) && (Dum1Ptr[Loop1UInt16 + 1] != ParseCharacter)) ++Loop1UInt16;
        }
      break;

      case (PARSE_PAYLOAD):
        StructMQTT->SubPayload[ItemNumber][CharCount] = Dum1Ptr[Loop1UInt16];
        /// log_info(__LINE__, __func__, "Current SubPayload: <%s>\r", StructMQTT->SubPayload[ItemNumber]);
        ++CharCount;
        /* If this subpayload is too long, troncate it. */
        if ((CharCount >= (MaxLength - 1)) && (Dum1Ptr[Loop1UInt16 + 1] != ParseCharacter))
        {
          while ((Dum1Ptr[Loop1UInt16 + 1] != 0) && (Dum1Ptr[Loop1UInt16 + 1] != ParseCharacter)) ++Loop1UInt16;
        }
      break;
    }
  }

#if 0
  log_info(__LINE__, __func__, "Topic components:\r");
  for (Loop1UInt16 = 0; Loop1UInt16 < MAX_SUB_TOPIC; ++Loop1UInt16)
    log_info(__LINE__, __func__, "SubTopic %2u: <%s>\r", Loop1UInt16, StructMQTT->SubTopic[Loop1UInt16]);

  log_info(__LINE__, __func__, "Payload components:\r");
  for (Loop1UInt16 = 0; Loop1UInt16 < MAX_SUB_PAYLOAD; ++Loop1UInt16)
    log_info(__LINE__, __func__, "SubPayload %2u: <%s>\r", Loop1UInt16, StructMQTT->SubPayload[Loop1UInt16]);
#endif  // 0

  return;
}

