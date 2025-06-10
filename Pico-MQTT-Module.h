/* ============================================================================================================================================================= *\
   Pico-MQTT-Module.h
   St-Louys Andre - May 2025
   astlouys@gmail.com
   Revision 21-MAY-2025
   Langage: C
\* ============================================================================================================================================================= */

#ifndef __PICO_MQTT_MODULE_H
#define __PICO_MQTT_MODULE_H



/* $PAGE */
/* $TITLE=Include files. */
/* ============================================================================================================================================================= *\
                                                                      Include files.
\* ============================================================================================================================================================= */
#include "lwip/apps/mqtt.h"



/* $PAGE */
/* $TITLE=Definitions. */
/* ============================================================================================================================================================= *\
                                                                        Definitions.
\* ============================================================================================================================================================= */
/* MQTT credentials. */
#define MAX_SUB_TOPIC      10  // maximum number of sub-topics.
#define MAX_TOPIC_LENGTH   40  // maximum length of a sub-topic.
#define MAX_SUB_PAYLOAD    10  // maximum number of sub-payloads.
#define MAX_PAYLOAD_LENGTH 40  // maximum length of a sub-payload.
#define PARSE_TOPIC         1
#define PARSE_PAYLOAD       2
#define PORT             1883  // port used for MQTT.



/* $PAGE */
/* $TITLE=Variable definitions. */
/* ============================================================================================================================================================= *\
                                                                      Variable definitions.
\* ============================================================================================================================================================= */
struct struct_mqtt
{
  UINT8          FlagHealth;
  UINT32         TotalErrors;
  UCHAR          PicoUniqueId[40];    // Pico Unique ID ("serial number") used for MQTT client ID.
  UCHAR          PicoIdentifier[40];  // "human string" to describe / identify the PicoW client device from its Unique Number.
  UCHAR          Password[40];        // MQTT password.
  ip_addr_t      BrokerAddress;       // IP address of MQTT broker.
  ip_addr_t      PicoIPAddress;       // IP address of PicoW.
  UINT8          Topic[100];
  UINT8          Payload[MQTT_OUTPUT_RINGBUF_SIZE];
  UCHAR          SubTopic[MAX_SUB_TOPIC][MAX_TOPIC_LENGTH];
  UCHAR          SubPayload[MAX_SUB_PAYLOAD][MAX_PAYLOAD_LENGTH];
  UINT32         PayloadLength;
  mqtt_client_t *MqttClientInstance;
  struct mqtt_connect_client_info_t MqttClientInfo;
};

typedef struct mqtt_client_s mqtt_client_t;



/* $PAGE */
/* $TITLE=Function prototypes. */
/* ============================================================================================================================================================= *\
                                                                     Function prototypes.
\* ============================================================================================================================================================= */
/* Display MQTT client information. */
void mqtt_display_client(struct struct_mqtt *StructMQTT);

/* Display MQTT topic and subtopics. */
void mqtt_display_topic(struct struct_mqtt *StructMQTT);

/* Initialize MQTT session. */
INT16 mqtt_init(struct struct_mqtt *StructMQTT);

/* Parse topic or data into its components (separator must be a slash </>). */
void mqtt_parse_topic(struct struct_mqtt *StructMQTT, UINT8 ParseUnit);

/* Send a string to external monitor through Pico UART (or USB CDC). */
extern void log_info(UINT LineNumber, const UCHAR *FunctionName, UCHAR *Format, ...);

#endif  // __PICO_MQTT_MODULE_H
