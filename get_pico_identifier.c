/* $PAGE */
/* $TITLE=get_pico_identifier() */
/* ============================================================================================================================================================= *\
                                                 Retrieve specific Pico identifier string from its Unique ID.
                     This function attrubutes a "device name" (or "device ID") to each physical Pico, based on its "Unique ID" (serial number).
    This way, we can use this "name" as an MQTT client ID. A "UFO-Pico" means an "Unidentified Flying picO", or a Pico for which no name has been assigned yet.
\* ============================================================================================================================================================= */
void get_pico_identifier(UCHAR *PicoUniqueId, UCHAR *PicoIdentifier)
{
  UINT Loop1UInt;

  pico_unique_board_id_t board_id;


  /* Retrieve Pico Unique ID from its flash memory IC. */
  pico_get_unique_board_id(&board_id);

  /* Build the Unique ID string in hex. */
  PicoUniqueId[0] = '\0';  // initialize as null string on entry.
  for (Loop1UInt = 0; Loop1UInt < PICO_UNIQUE_BOARD_ID_SIZE_BYTES; ++Loop1UInt)
  {
    // log_info(__LINE__, __func__, "%2u - 0x%2.2X\n", Loop1UInt, board_id.id[Loop1UInt]);
    sprintf(&PicoUniqueId[strlen(PicoUniqueId)], "%2.2X", board_id.id[Loop1UInt]);
    if ((Loop1UInt % 2) && (Loop1UInt != 7)) sprintf(&PicoUniqueId[strlen(PicoUniqueId)], "-");
  }


  /* Assign Pico's unique ID as default identifier in case this specific Pico Unique ID has not been tagged yet. */
  sprintf(PicoIdentifier, "%s", PicoUniqueId);

  if (strncmp(PicoUniqueId, "E661-4103-E72C-2423", strlen(PicoUniqueId)) == 0)
    sprintf(PicoIdentifier, "Control");

  if (strncmp(PicoUniqueId, "E661-4103-E74E-9221", strlen(PicoUniqueId)) == 0)
    sprintf(PicoIdentifier, "CallerId");

  if (strncmp(PicoUniqueId, "E661-4103-E756-8321", strlen(PicoUniqueId)) == 0)
    sprintf(PicoIdentifier, "Roland");

  if (strncmp(PicoUniqueId, "E661-4103-E75E-7F21", strlen(PicoUniqueId)) == 0)
    sprintf(PicoIdentifier, "MultiSensor");

  if (strncmp(PicoUniqueId, "E661-4103-E770-1B25", strlen(PicoUniqueId)) == 0)
    sprintf(PicoIdentifier, "Kitchen");

  if (strncmp(PicoUniqueId, "E661-4103-E79D-5723", strlen(PicoUniqueId)) == 0)
    sprintf(PicoIdentifier, "SoundServer");

  if (strncmp(PicoUniqueId, "E661-6408-4323-8B29", strlen(PicoUniqueId)) == 0)
    sprintf(PicoIdentifier, "BureauM");

  if (strncmp(PicoUniqueId, "E661-6408-4329-202D", strlen(PicoUniqueId)) == 0)
    sprintf(PicoIdentifier, "Lounge");

  if (strncmp(PicoUniqueId, "E661-6408-4329-6029", strlen(PicoUniqueId)) == 0)
    sprintf(PicoIdentifier, "Atelier");

  if (strncmp(PicoUniqueId, "E661-6408-4337-9121", strlen(PicoUniqueId)) == 0)
    sprintf(PicoIdentifier, "ChambreA");

  if (strncmp(PicoUniqueId, "E661-6408-433D-3127", strlen(PicoUniqueId)) == 0)
    sprintf(PicoIdentifier, "Raymonde");  // to be verified

  if (strncmp(PicoUniqueId, "E661-6408-434A-0826", strlen(PicoUniqueId)) == 0)
    sprintf(PicoIdentifier, "OfficeLong");

  if (strncmp(PicoUniqueId, "E661-6408-434B-7A22", strlen(PicoUniqueId)) == 0)
    sprintf(PicoIdentifier, "Dining");

  if (strncmp(PicoUniqueId, "E661-6408-434D-8521", strlen(PicoUniqueId)) == 0)
    sprintf(PicoIdentifier, "CallerId1");

  if (strncmp(PicoUniqueId, "E661-6408-436E-6924", strlen(PicoUniqueId)) == 0)
    sprintf(PicoIdentifier, "OfficeTest");

  if (strncmp(PicoUniqueId, "E661-6408-437C-B024", strlen(PicoUniqueId)) == 0)
    sprintf(PicoIdentifier, "DualExpander");

  // if (strncmp(PicoUniqueId, "E661-6408-4382-772E", strlen(PicoUniqueId)) == 0)
  //   sprintf(PicoIdentifier, "Control");

  return;
}
