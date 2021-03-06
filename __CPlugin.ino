//********************************************************************************
// Initialize all Controller CPlugins that where defined earlier
// and initialize the function call pointer into the CCPlugin array
//********************************************************************************
void CPluginInit(void)
{
  byte x;

  // Clear pointer table for all plugins
  for (x = 0; x < CPLUGIN_MAX; x++)
  {
    CPlugin_ptr[x] = 0;
    CPlugin_id[x] = 0;
  }

  x = 0;

#ifdef CPLUGIN_001
  CPlugin_id[x] = 1; CPlugin_ptr[x++] = &CPlugin_001;
#endif

#ifdef CPLUGIN_002
  CPlugin_id[x] = 2; CPlugin_ptr[x++] = &CPlugin_002;
#endif

#ifdef CPLUGIN_003
  CPlugin_id[x] = 3; CPlugin_ptr[x++] = &CPlugin_003;
#endif

#ifdef CPLUGIN_004
  CPlugin_id[x] = 4; CPlugin_ptr[x++] = &CPlugin_004;
#endif

#ifdef CPLUGIN_005
  CPlugin_id[x] = 5; CPlugin_ptr[x++] = &CPlugin_005;
#endif

#ifdef CPLUGIN_006
  CPlugin_id[x] = 6; CPlugin_ptr[x++] = &CPlugin_006;
#endif

#ifdef CPLUGIN_007
  CPlugin_id[x] = 7; CPlugin_ptr[x++] = &CPlugin_007;
#endif

#ifdef CPLUGIN_008
  CPlugin_id[x] = 8; CPlugin_ptr[x++] = &CPlugin_008;
#endif

#ifdef CPLUGIN_009
  CPlugin_id[x] = 9; CPlugin_ptr[x++] = &CPlugin_009;
#endif

#ifdef CPLUGIN_010
  CPlugin_id[x] = 10; CPlugin_ptr[x++] = &CPlugin_010;
#endif

#ifdef CPLUGIN_011
  CPlugin_id[x] = 11; CPlugin_ptr[x++] = &CPlugin_011;
#endif

#ifdef CPLUGIN_012
  CPlugin_id[x] = 12; CPlugin_ptr[x++] = &CPlugin_012;
#endif

#ifdef CPLUGIN_013
  CPlugin_id[x] = 13; CPlugin_ptr[x++] = &CPlugin_013;
#endif

#ifdef CPLUGIN_014
  CPlugin_id[x] = 14; CPlugin_ptr[x++] = &CPlugin_014;
#endif

#ifdef CPLUGIN_015
  CPlugin_id[x] = 15; CPlugin_ptr[x++] = &CPlugin_015;
#endif

#ifdef CPLUGIN_016
  CPlugin_id[x] = 16; CPlugin_ptr[x++] = &CPlugin_016;
#endif

#ifdef CPLUGIN_017
  CPlugin_id[x] = 17; CPlugin_ptr[x++] = &CPlugin_017;
#endif

#ifdef CPLUGIN_018
  CPlugin_id[x] = 18; CPlugin_ptr[x++] = &CPlugin_018;
#endif

#ifdef CPLUGIN_019
  CPlugin_id[x] = 19; CPlugin_ptr[x++] = &CPlugin_019;
#endif

#ifdef CPLUGIN_020
  CPlugin_id[x] = 20; CPlugin_ptr[x++] = &CPlugin_020;
#endif

#ifdef CPLUGIN_021
  CPlugin_id[x] = 21; CPlugin_ptr[x++] = &CPlugin_021;
#endif

#ifdef CPLUGIN_022
  CPlugin_id[x] = 22; CPlugin_ptr[x++] = &CPlugin_022;
#endif

#ifdef CPLUGIN_023
  CPlugin_id[x] = 23; CPlugin_ptr[x++] = &CPlugin_023;
#endif

#ifdef CPLUGIN_024
  CPlugin_id[x] = 24; CPlugin_ptr[x++] = &CPlugin_024;
#endif

#ifdef CPLUGIN_025
  CPlugin_id[x] = 25; CPlugin_ptr[x++] = &CPlugin_025;
#endif

  CPluginCall(CPLUGIN_PROTOCOL_ADD, 0);
}

byte CPluginCall(byte Function, struct EventStruct *event)
{
  int x;
  struct EventStruct TempEvent;

  if (event == 0)
    event=&TempEvent;

  switch (Function)
  {
    // Unconditional calls to all plugins
    case CPLUGIN_PROTOCOL_ADD:
      for (x = 0; x < CPLUGIN_MAX; x++)
      {
        // ignore if individual protocol failed to initialize,
        // there is nothing we can do about it, anyway.
        (void) CPlugin_ProtocolAdd(x,
                                   event,
                                   dummyString);
	    }
      return true;
      break;
  }

  return false;
}

/*
 * Dispatch call to the proper handler for the plugin, make sure plugin and handler exist.
 */
bool CPlugin_CallPluginFunction(unsigned int PluginIndex,
                                byte Function,
                                struct EventStruct *Event,
                                String& Text)
{
  const bool successful = true;
  const bool failure = false;
  bool status = successful;

  // check plugin exists and known function is called. Log failures.
  if ( (PluginIndex > CPLUGIN_MAX) ||
       ( (Function < CPLUGIN_PROTOCOL_ADD) || (Function > CPLUGIN_WEBFORM_LOAD) ) )
  {
    String log = F("CallPluginFunction: PluginIndex or Function out of bounds! Plugin(");
    log += PluginIndex;
    log += F("), Function(");
    log += Function;
    log += F(")");
    addLog(LOG_LEVEL_INFO, log);

    status = failure;  // we weren't successful
  }

  // check handler for the requested function exists in this plugin. Log failures.
  if ( status ) {
    CPluginHandler PluginHandler = CPlugin_ptr[PluginIndex];
    if ( NULL == PluginHandler )
    {
      String log = F("CallPluginFunction: PluginHandler is NULL! Plugin(");
      log += PluginIndex;
      log += F(")");
      addLog(LOG_LEVEL_INFO, log);

      status = failure;
    }

    // Call handler if all is well
    status = status && PluginHandler(Function, Event, Text);
  }

  return status;
}

/*  Wrapper function for calling the plugin function and logging the result. */
bool CPlugin_LoggedPluginFunction(unsigned int PluginIndex,
                                  byte Function,
                                  struct EventStruct *Event,
                                  String& Text,
                                  const String& DebugPrefix)
{
  bool result = CPlugin_CallPluginFunction(PluginIndex,
                                           Function,
                                           Event,
                                           Text);
  CPlugin_LogHandlerCall(DebugPrefix,
                         PluginIndex,
                         result);

  return result;
}
/* Wrapper to add a protocol handler for the plugin */
bool CPlugin_ProtocolAdd(unsigned int PluginIndex,
                         struct EventStruct *Event,
                         String& Text)
{
  return CPlugin_LoggedPluginFunction(PluginIndex,
                                      CPLUGIN_PROTOCOL_ADD,
                                      Event,
                                      Text,
                                      PSTR("CPlugin_ProtocolAdd: Result "));
}
/* Wrapper function for logging the result of calling a handler */
static void CPlugin_LogHandlerCall(const String& prefix,
                                   unsigned int PluginIndex,
                                   bool result)
{
  String log = prefix;

  log += result;
  log += F(" Plugin(");
  log += PluginIndex;
  log += F(")");

  addLog(LOG_LEVEL_INFO, log);
}


bool CPlugin_ProtocolTemplate(unsigned int PluginIndex,
                              struct EventStruct *Event,
                              String& Text)
{
  return CPlugin_LoggedPluginFunction(PluginIndex,
                                      CPLUGIN_PROTOCOL_TEMPLATE,
                                      Event,
                                      Text,
                                      PSTR("CPlugin_ProtocolTemplate: Result"));
}

bool CPlugin_ProtocolSend(unsigned int PluginIndex,
                          struct EventStruct *Event,
                          String& Text)
{
  return CPlugin_LoggedPluginFunction(PluginIndex,
                                      CPLUGIN_PROTOCOL_SEND,
                                      Event,
                                      Text,
                                      PSTR("CPlugin_ProtocolSend: Result"));
}

bool CPlugin_ProtocolRecv(unsigned int PluginIndex,
                          struct EventStruct *Event,
                          String& Text)
{
  return CPlugin_LoggedPluginFunction(PluginIndex,
                                      CPLUGIN_PROTOCOL_RECV,
                                      Event,
                                      Text,
                                      PSTR("CPlugin_ProtocolRecv: Result"));
}

bool CPlugin_GetDevicename(unsigned int PluginIndex,
                           struct EventStruct *Event,
                           String& Text)
{
  return CPlugin_LoggedPluginFunction(PluginIndex,
                                      CPLUGIN_GET_DEVICENAME,
                                      Event,
                                      Text,
                                      PSTR("CPlugin_GetDevicename: Result"));
}

bool CPlugin_WebformSave(unsigned int PluginIndex,
                         struct EventStruct *Event,
                         String& Text)
{
  return CPlugin_LoggedPluginFunction(PluginIndex,
                                      CPLUGIN_WEBFORM_SAVE,
                                      Event,
                                      Text,
                                      PSTR("CPlugin_WebformSave: Result"));
}

bool CPlugin_WebformLoad(unsigned int PluginIndex,
                         struct EventStruct *Event,
                         String& Text)
{
  return CPlugin_LoggedPluginFunction(PluginIndex,
                                      CPLUGIN_WEBFORM_LOAD,
                                      Event,
                                      Text,
                                      PSTR("CPlugin_WebformLoad: Result"));
}

