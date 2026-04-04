/* ============================================================================================================================================================= *\
   Pico-MQTT-Module.h
   St-Louys Andre - May 2025
   astlouys@gmail.com
   Revision 02-APR-2026
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

#define MQTT_CONNECTION_OK        1001  // connect with MQTT broker without error.
#define MQTT_CONNECTION_ERROR     1002  // error while trying to connect with MQTT broker.
#define MQTT_PUBLISH_OK           1003  // publish on a specific topic without error.
#define MQTT_PUBLISH_ERROR        1004  // error while trying to publish on a specific topic.
#define MQTT_RECEIVE_TOPIC        1005  // received a topic on which we did subscribe.
#define MQTT_SUB_UNSUB_OK         1006  // subscribe or unsubscribe from a specific topic without error.
#define MQTT_SUB_UNSUB_ERROR      1007  // error while trying to subscribe or unsubscribe from a specific topic.
#define MQTT_SUBSCRIBE_OK         1008  // subscribe to a specific topic without error.
#define MQTT_SUBSCRIBE_ERROR      1009  // error while trying to subscribe to a specific topic.
#define MQTT_UNSUBSCRIBE_OK       1010  // unsubscribe from a specific topic without error.
#define MQTT_UNSUBSCRIBE_ERROR    1011  // error while trying to unsubscribe from a specific topic.


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
  void           (*mqtt_status)(UINT16 Status);
  /// void           (*mqtt_connection_status)(UINT8 Status);
  /// void           (*mqtt_publish_status)(UINT8 Status);
  /// void           (*mqtt_subscribe_status)(UINT8 Status);
  /// void           (*mqtt_topic_receive)(UINT8 Argument);
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
void mqtt_breakdown_end(void);

/* Enter time of beginning of MQTT breakdown. */
void mqtt_breakdown_start(void);

/* Check MQTT connection health. */
INT16 mqtt_check_connection(UINT8 FlagWiFiHealth);

/* Callback to receive the result for a MQTT connection request. */
void mqtt_connection_cb(mqtt_client_t *LocalClient, void *ExtraArgument, mqtt_connection_status_t Status);

/* Display MQTT client information. */
void mqtt_display_client(void);

/* Display all current MQTT sub-payloads. */
void mqtt_display_payload(void);

/* Display all current MQTT sub-topics. */
void mqtt_display_topic(void);

/* Callback to receive the response of a publish request. */
void mqtt_incoming_publish_cb(void *ExtraArgument, const char *Topic, UINT32 PayloadLength);

/* Initialize MQTT session. */
INT16 mqtt_init(void);

/* Parse topic or payload into its components: sub-topics and sub-payloads (separator must be a slash </> in both cases). */
void mqtt_parse_item(UINT8 ParseUnit);

/* Callback to receive the response of a publish request. */
void mqtt_pub_request_cb(void *ExtraArgument, err_t Result);

/* Callback to receive the response to a subscribe request. */
void mqtt_sub_request_cb(void *ExtraArgument, err_t Result);

/* Wipe MQTT packet in preparation for next reception. */
void mqtt_wipe_packet(void);

/* Send a string to external monitor through Pico UART (or USB CDC). */
extern void log_printf(UINT LineNumber, const UCHAR *FunctionName, UCHAR *Format, ...);

#endif  // __PICO_MQTT_MODULE_H
