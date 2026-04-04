#include "hardware/adc.h"
#define ADC_VCC                 29            // internal GPIO to determine if we are running on a Pico or PicoW.
/* $PAGE */
/* $TITLE=get_pico_identifier() */
/* ============================================================================================================================================================= *\
                                                 Retrieve specific Pico identifier string from its Unique ID.
         This function attributes a "device name" (or "device ID") to each physical Pico, based on its "Unique ID" (serial number in Pico's flash memory).
    This way, we can use this "Device Name" as an MQTT client ID. If no Device Id is found corresponding to the Pico's Unique ID, the Unique ID itself
    will be returned as the Device Name. If this happens, you should edit the <get_pico_identifier()> function and add the Pico's Unique ID and A "UFO-Pico" means an "Unidentified Flying picO", or a Pico for which no name has been assigned yet.
\* ============================================================================================================================================================= */
void get_pico_identifier(UCHAR *PicoUniqueId, UCHAR *PicoIdentifier, UINT8 *PicoType)
{
  UINT Loop1UInt;

  UINT16 AdcValue;

  float Volts;

  pico_unique_board_id_t board_id;


  /* ----------------------------------------------------------------------------------------------------------------------------------------------------------- *\
                                                                 Determine Pico type used (Pico or PicoW)
  \* ----------------------------------------------------------------------------------------------------------------------------------------------------------- */
  /* Initialize analog-to-digital converter to read ambient light relative value and power supply info. */
  adc_init();
  adc_gpio_init(ADC_VCC);           // power supply voltage.

  /* Select power supply input. */
  adc_select_input(3);

  /* The important power supply value to consider is when GPIO25 is Low. */
  gpio_put(GPIO_PICO_LED, 0);

  /* Read ADC converter raw value. */
  AdcValue = adc_read();

  /* Convert raw value to voltage value. */
  Volts = AdcValue * (3.3 / (1 << 12));

  log_printf(__LINE__, __func__, "AdcValue: %4u   Volts: %4.2f\n", AdcValue, Volts);
  /* Reading example for a Pico  =         1995            1.61             2010            1.62   */
  /* Reading example for a PicoW =         2395            1.93              120            0.10   */

  /* Determine the microcontroller type based on Volts2 value. */
  if (Volts > 1.0)
    *PicoType = TYPE_PICO;
  else
    *PicoType = TYPE_PICOW;


  
  /* ----------------------------------------------------------------------------------------------------------------------------------------------------------- *\
                                                           Retrieve Pico Unique ID from its flash memory IC.
  \* ----------------------------------------------------------------------------------------------------------------------------------------------------------- */
  pico_get_unique_board_id(&board_id);

  /* Build the Unique ID string in hex. */
  PicoUniqueId[0] = '\0';  // initialize as null string on entry.
  for (Loop1UInt = 0; Loop1UInt < PICO_UNIQUE_BOARD_ID_SIZE_BYTES; ++Loop1UInt)
  {
    // log_info(__LINE__, __func__, "%2u - 0x%2.2X\n", Loop1UInt, board_id.id[Loop1UInt]);
    sprintf(&PicoUniqueId[strlen(PicoUniqueId)], "%2.2X", board_id.id[Loop1UInt]);
    if ((Loop1UInt % 2) && (Loop1UInt != 7)) sprintf(&PicoUniqueId[strlen(PicoUniqueId)], "-");
  }



  /* ----------------------------------------------------------------------------------------------------------------------------------------------------------- *\
                                                     Retrieve Pico Device ID corresponding to this Pico's Unique ID.
  \* ----------------------------------------------------------------------------------------------------------------------------------------------------------- */
  /* Assign Pico's unique ID as default identifier in case this specific Pico Unique ID has not been tagged yet. */
  sprintf(PicoIdentifier, "%s", PicoUniqueId);

  if (strncmp(PicoUniqueId, "E661-4103-E756-8321", strlen(PicoUniqueId)) == 0)
    sprintf(PicoIdentifier, "Alain");

  if (strncmp(PicoUniqueId, "E661-6408-4329-6029", strlen(PicoUniqueId)) == 0)
    sprintf(PicoIdentifier, "Atelier");

  if (strncmp(PicoUniqueId, "E661-6408-4323-8B29", strlen(PicoUniqueId)) == 0)
    sprintf(PicoIdentifier, "BureauM");

  if (strncmp(PicoUniqueId, "E661-4103-E74E-9221", strlen(PicoUniqueId)) == 0)
    sprintf(PicoIdentifier, "CallerId");

  if (strncmp(PicoUniqueId, "E661-6408-434D-8521", strlen(PicoUniqueId)) == 0)
    sprintf(PicoIdentifier, "CallerId1");

  if (strncmp(PicoUniqueId, "E661-6408-4337-9121", strlen(PicoUniqueId)) == 0)
    sprintf(PicoIdentifier, "ChambreA");

  if (strncmp(PicoUniqueId, "E661-4103-E72C-2423", strlen(PicoUniqueId)) == 0)
    sprintf(PicoIdentifier, "Control");

  if (strncmp(PicoUniqueId, "E661-6408-434B-7A22", strlen(PicoUniqueId)) == 0)
    sprintf(PicoIdentifier, "Dining");

  if (strncmp(PicoUniqueId, "E661-6408-437C-B024", strlen(PicoUniqueId)) == 0)
    sprintf(PicoIdentifier, "DualExpander");

  if (strncmp(PicoUniqueId, "E661-4103-E770-1B25", strlen(PicoUniqueId)) == 0)
    sprintf(PicoIdentifier, "Kitchen");

  if (strncmp(PicoUniqueId, "E661-6408-4329-202D", strlen(PicoUniqueId)) == 0)
    sprintf(PicoIdentifier, "Lounge");

  if (strncmp(PicoUniqueId, "E661-6408-4316-352D", strlen(PicoUniqueId)) == 0)
    sprintf(PicoIdentifier, "MultiSensor");

  if (strncmp(PicoUniqueId, "E661-6408-434A-0826", strlen(PicoUniqueId)) == 0)
    sprintf(PicoIdentifier, "OfficeLong");

  if (strncmp(PicoUniqueId, "E661-6408-436E-6924", strlen(PicoUniqueId)) == 0)
    sprintf(PicoIdentifier, "OfficeTest");

  if (strncmp(PicoUniqueId, "E661-6408-436C-4D2B", strlen(PicoUniqueId)) == 0)
    sprintf(PicoIdentifier, "PrintServer");

  if (strncmp(PicoUniqueId, "E661-6408-433D-3127", strlen(PicoUniqueId)) == 0)
    sprintf(PicoIdentifier, "Raymonde");  // to be verified

  if (strncmp(PicoUniqueId, "E661-6408-4354-9829", strlen(PicoUniqueId)) == 0)
    sprintf(PicoIdentifier, "SoundServer1");

  if (strncmp(PicoUniqueId, "E661-6408-437F-732A", strlen(PicoUniqueId)) == 0)
    sprintf(PicoIdentifier, "SoundServer2");

  return;
}
