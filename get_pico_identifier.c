/* $PAGE */
/* $TITLE=get_pico_identifier() */
/* ============================================================================================================================================================= *\
                                                 Retrieve specific Pico identifier string from its Unique ID.
                     This function attrubutes a "device name" (or "device ID") to each physical Pico, based on its "Unique ID" (serial number).
    This way, we can use this "name" as an MQTT client ID. A "UFO-Pico" means an "Unidentified Flying picO", or a Pico for which no name has been assigned yet.
\* ============================================================================================================================================================= */
void get_pico_identifier(UCHAR *PicoUniqueId, UCHAR *PicoIdentifier)
{
  /* Assign a default identifier in case this specific Pico Unique ID has not been tagged yet. */
  sprintf(PicoIdentifier, "UFO-Pico");

  if (strncmp(PicoUniqueId, "E661-6408-4329-6029", strlen(PicoUniqueId)) == 0)
    sprintf(PicoIdentifier, "Atelier");

  if (strncmp(PicoUniqueId, "E661-6408-4323-8B29", strlen(PicoUniqueId)) == 0)
    sprintf(PicoIdentifier, "BureauM");

  if (strncmp(PicoUniqueId, "E661-6408-4337-9121", strlen(PicoUniqueId)) == 0)
    sprintf(PicoIdentifier, "ChambreA");

  if (strncmp(PicoUniqueId, "E661-4103-E770-1B25", strlen(PicoUniqueId)) == 0)
    sprintf(PicoIdentifier, "Cuisine");

  if (strncmp(PicoUniqueId, "E661-6408-434B-7A22", strlen(PicoUniqueId)) == 0)
    sprintf(PicoIdentifier, "Dining");

  if (strncmp(PicoUniqueId, "E661-6408-4329-202D", strlen(PicoUniqueId)) == 0)
    sprintf(PicoIdentifier, "Lounge");

  if (strncmp(PicoUniqueId, "E661-4103-E74E-9221", strlen(PicoUniqueId)) == 0)
    sprintf(PicoIdentifier, "MQTTControl");

  if (strncmp(PicoUniqueId, "E661-4103-E72C-2423", strlen(PicoUniqueId)) == 0)
    sprintf(PicoIdentifier, "MQTTSensor1");

  if (strncmp(PicoUniqueId, "E661-6408-434A-0826", strlen(PicoUniqueId)) == 0)
    sprintf(PicoIdentifier, "OfficeLong");

  if (strncmp(PicoUniqueId, "E661-6408-437C-B024", strlen(PicoUniqueId)) == 0)
    sprintf(PicoIdentifier, "OfficeMain");

  if (strncmp(PicoUniqueId, "E661-6408-433D-3127", strlen(PicoUniqueId)) == 0)
    sprintf(PicoIdentifier, "Raymonde");  // to be verified

  /// if (strncmp(PicoUniqueId, "E661-6408-4382-772E", strlen(PicoUniqueId)) == 0)
  ///   sprintf(PicoIdentifier, "OfficePlain");

  return;
}
