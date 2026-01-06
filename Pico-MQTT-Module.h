/* ============================================================================================================================================================= *\
   Pico-MQTT-Module.h
   St-Louys Andre - May 2025
   astlouys@gmail.com
   Revision 07-DEC-2025
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
#define MAX_TOPIC_LENGTH           512  // maximum length of the topic data space (note that MQTT protocol allows up to 65535).
#define MAX_SUB_TOPICS              25  // maximum number of sub-topics.
#define MAX_PAYLOAD_LENGTH         512  // maximum length of the payload data space.
#define MAX_SUB_PAYLOADS            25  // maximum number of sub-payloads.
#define PARSE_TOPIC                  1  // determine which item is to be parsed (topic or payload).
#define PARSE_PAYLOAD                2  // determine which item is to be parsed (topic or payload).
#define PORT                      1883  // port used for MQTT.
#define MAX_MQTT_BREAKDOWN_HISTORY  10  // number of breakdown history items to keep in memory.



/* $PAGE */
/* $TITLE=Variable definitions. */
/* ============================================================================================================================================================= *\
                                                                      Variable definitions.
\* ============================================================================================================================================================= */
struct struct_mqtt
{
  UINT8          FlagHealth;
  UINT8          FlagSubscribe;       // if FLAG_ON, means that we want to subscribe, FLAG_OFF means that we want to unsubscribe.
  UINT8          FlagStartupOver;     // indicate that MQTT connection has already been established with MQTT broker during startup sequence.
  UINT32         TotalErrors;
  UCHAR          PicoUniqueId[40];    // Pico Unique ID ("serial number") used for MQTT client ID.
  UCHAR          PicoIdentifier[40];  // "human string" to describe / identify the PicoW client device from its Unique Number.
  UCHAR          Password[40];        // MQTT password.
  ip_addr_t      BrokerAddress;       // IP address of MQTT broker.
  ip_addr_t      PicoIPAddress;       // IP address of PicoW.
  UCHAR          Topic[MAX_TOPIC_LENGTH];
  UCHAR          Payload[MAX_PAYLOAD_LENGTH];
  UCHAR          *SubTopic[MAX_SUB_TOPICS];
  UCHAR          *SubPayload[MAX_SUB_PAYLOADS];
  UINT32         PayloadLength;
  mqtt_client_t *MqttClientInstance;
  struct mqtt_connect_client_info_t MqttClientInfo;
  datetime_t BreakdownStart[MAX_MQTT_BREAKDOWN_HISTORY];  // time stamp of the last MQTT connection breakdowns start time.
  datetime_t BreakdownEnd[MAX_MQTT_BREAKDOWN_HISTORY];    // time stamp of the last MQTT connection breakdowns ened time.
};

typedef struct mqtt_client_s mqtt_client_t;



/* $PAGE */
/* $TITLE=Function prototypes. */
/* ============================================================================================================================================================= *\
                                                                     Function prototypes.
\* ============================================================================================================================================================= */
/* Enter time of end of MQTT breakdown. */
void mqtt_breakdown_end(datetime_t EndTime);

/* Enter time of beginning of MQTT breakdown. */
void mqtt_breakdown_start(datetime_t StartTime);

/* Check MQTT connection health. */
INT16 mqtt_check_connection(UINT8 FlagWiFiHealth);

/* Display MQTT client information. */
void mqtt_display_client(void);

/* Display all current MQTT sub-payloads. */
void mqtt_display_payload(void);

/* Display all current MQTT sub-topics. */
void mqtt_display_topic(void);

/* Initialize MQTT session. */
INT16 mqtt_init(void);

/* Parse topic or payload into its components: sub-topics and sub-payloads (separator must be a slash </> in both cases). */
void mqtt_parse_topic(UINT8 ParseUnit);

/* Wipe MQTT packet in preparation for next reception. */
void mqtt_wipe_packet(void);

/* Send a string to external monitor through Pico UART (or USB CDC). */
extern void log_printf(UINT LineNumber, const UCHAR *FunctionName, UCHAR *Format, ...);

#endif  // __PICO_MQTT_MODULE_H
