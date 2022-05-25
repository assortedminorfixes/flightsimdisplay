#include "Arduino.h"
#include "state.hh"
#include "display.hh"
#include "lights.hh"
#include "messaging.hh"

//// ------   Spad Coms Section ------ ///////
CmdMessenger messenger(Serial);

bool isReady = false;
bool isConfig = false;
bool isPowerOn = false;
bool isDisplay = false;
unsigned long subscribeTime = 0;
uint8_t subscribeIndex = 0;

#define DEBUG

template <class T>
void sendDebugMsg(int command, T arg)
{
#ifdef DEBUG
    disp.lastCommand(command, static_cast<int32_t>(arg));
    // messenger.sendCmd(kDebug, arg);
#endif
}

// ------------------  C A L L B A C K S -----------------------

// Called when a received command has no attached function
void onUnknownCommand()
{
    uint8_t cmd = messenger.commandID();

#ifdef DEBUG
    String msg = F("Unknown command: ");
    disp.printDebug(msg + cmd);
#endif

    messenger.sendCmd(kDebug, F("UNKNOWN COMMAND")); // if a command comes in that is not reckognized from sketch write to the spad log
}

// Callback function to respond to indentify request. This is part of the
// Auto connection handshake.
void onIdentifyRequest()
{
    char *szRequest = messenger.readStringArg();

    if (strcmp(szRequest, "INIT") == 0)
    { // Initial Configuration declaration

        isReady = false;
        isPowerOn = false;
        isConfig = false;

        uint8_t apiVersion = messenger.readInt32Arg();
        String spadVersion = messenger.readStringArg();
        String spadAuthToken = messenger.readStringArg();

        messenger.sendCmdStart(kRequest);
        messenger.sendCmdArg(F("SPAD"));
        messenger.sendCmdArg(F("{7eb4b953-64c6-4c94-a958-1fac034a0370}")); // GUID
        messenger.sendCmdArg(F("SimDisplay"));                             // DEVICE NAME
        messenger.sendCmdArg(2);
        messenger.sendCmdArg(F("0.2"));                  // VERSION NUMBER
        messenger.sendCmdArg(F("AUTHOR=1683e5ce90820838a39d0d3990f4c266"));
        messenger.sendCmdArg(F("ALLOWLOCAL=2"));
        // messenger.sendCmdArg("VID=LarsLL"); // AUTHOR ID
        // messenger.sendCmdArg("PID=SimDisplay"); // AUTHOR ID
        messenger.sendCmdEnd();

        return;
    }

    if (strcmp(szRequest, "PING") == 0)
    { // This is a watchdog timer response
        messenger.sendCmdStart(kRequest);
        messenger.sendCmdArg(F("PONG"));
        messenger.sendCmdArg(messenger.readInt32Arg());
        messenger.sendCmdEnd();
        return;
    }
    if (strcmp(szRequest, "CONFIG") == 0)
    {
        disp.printSplash(F("Configuring"));
        isReady = false;
        isConfig = false;
        isPowerOn = false;
        isDisplay = false;
        subscribeIndex = 0;

        messenger.sendCmdStart(kRequest);
        messenger.sendCmdArg("OPTION");
        messenger.sendCmdArg("ISGENERIC=" + String(1));
        messenger.sendCmdArg("PAGESUPPORT=" + String(0));
        messenger.sendCmdArg("OUT_COOLDOWN=" + String(100));
        messenger.sendCmdArg("NO_DISPLAY_CLEAR=" + String(1));
        messenger.sendCmdArg("VPSUPPORT=" + String(1));
        messenger.sendCmdEnd();

        messenger.sendCmdStart(kRequest);
        messenger.sendCmdArg(F("INPUT"));         // Subcommand..ADD - SUBSCRIBE - UNSUBSCRIBE - EMULATE
        messenger.sendCmdArg(F("0"));         // Subcommand..ADD - SUBSCRIBE - UNSUBSCRIBE - EMULATE
        messenger.sendCmdArg(F("CONFIGURE_PANEL_STATUS"));         
        messenger.sendCmdArg(F("SYSTEM"));        
        messenger.sendCmdArg(F("SPAD_VIRTUAL_POWER"));        
        messenger.sendCmdArg(F("UI_TYPE=3"));  
        messenger.sendCmdArg(F("CUSTOM_TYPE=POWER"));  
        messenger.sendCmdEnd();

        // Expose Course selector ... Mode..
        messenger.sendCmdStart(kRequest);       // This is a "1" or Command:1 from Spad list
        messenger.sendCmdArg(F("INPUT"));         // Subcommand..ADD - SUBSCRIBE - UNSUBSCRIBE - EMULATE
        messenger.sendCmdArg(iSelCRS);            // CMDID value defined at the top as "19" this will be the DATA channel
        messenger.sendCmdArg(F("CRS_SELECT"));    // will become "SERIAL:<guid>/APm/CRSs"
        messenger.sendCmdArg(F("ROTARY"));          // Value Type .. (Signed/Unsigned) one of S8,S16,S32,S64,U8,U16,U32,U64,FLT32,FLT64,ASCIIZ
        messenger.sendCmdArg(F("SPAD_ENCODER_NOACC"));          // Value Type .. (Signed/Unsigned) one of S8,S16,S32,S64,U8,U16,U32,U64,FLT32,FLT64,ASCIIZ        
        messenger.sendCmdArg(F("POS_NAMES=CRS1#CRS2"));            // Access Type RO - ReadOnly, RW - ReadWrite
        messenger.sendCmdArg(F("POS_VALUES=0#1"));            // Access Type RO - ReadOnly, RW - ReadWrite
        messenger.sendCmdEnd();

        // Expose Radio selector ... Mode..
        messenger.sendCmdStart(kRequest);         // This is a "1" or Command:1 from Spad list
        messenger.sendCmdArg(F("INPUT"));           // Subcommand..ADD - SUBSCRIBE - UNSUBSCRIBE - EMULATE
        messenger.sendCmdArg(iSelRadio);            // CMDID value defined at the top as "28" this will be the DATA channel
        messenger.sendCmdArg(F("RADIO_SELECT"));   // will become "SERIAL:<guid>/AP/APRmode"
        messenger.sendCmdArg(F("ROTARY"));            // Value Type .. (Signed/Unsigned) one of S8,S16,S32,S64,U8,U16,U32,U64,FLT32,FLT64,ASCIIZ
        messenger.sendCmdArg(F("SPAD_ENCODER_NOACC"));          // Value Type .. (Signed/Unsigned) one of S8,S16,S32,S64,U8,U16,U32,U64,FLT32,FLT64,ASCIIZ        
        messenger.sendCmdArg(F("POS_NAMES=NAV1#NAV2#COM1#COM2#ADF"));            // Access Type RO - ReadOnly, RW - ReadWrite
        messenger.sendCmdArg(F("POS_VALUES=0#1#2#3#4"));            // Access Type RO - ReadOnly, RW - ReadWrite
        messenger.sendCmdEnd();

        // tell SPAD.neXT we are done with config
        messenger.sendCmd(kRequest, F("CONFIG"));

        isConfig = true;

        return;
    }

    if (strcmp(szRequest, "SCANSTATE") == 0)
    {
        char *str = messenger.readStringArg();

        messenger.sendCmd(kRequest, F("STATESCAN,1"));  
     
        // Provides currently selected Radio
        messenger.sendCmdStart(kInput);
        messenger.sendCmdArg(iSelRadio);
        messenger.sendCmdArg(state.radio);
        messenger.sendCmdEnd();

        // Provides currently selected CRS
        messenger.sendCmdStart(kInput);
        messenger.sendCmdArg(iSelCRS);
        messenger.sendCmdArg(state.crs);
        messenger.sendCmdEnd();

        messenger.sendCmd(kRequest, F("STATESCAN,2"));        
        return;
    }   

}

void onEvent()
{
    char *szRequest = messenger.readStringArg();

    if (strcmp(szRequest, "VIRTUALPOWER") == 0)
    {
        uint8_t flag = messenger.readInt16Arg();


    }
    else if (strcmp(szRequest, "PROFILECHANGED") == 0)
    {
        char *str = messenger.readStringArg();
        return;
    } 
    else if (strcmp(szRequest, "PAGE") == 0)
    {
        char *uid = messenger.readStringArg();
        uint8_t pagenum = messenger.readInt16Arg();
        char *pagename = messenger.readStringArg();
        return;
    }
    else if (strcmp(szRequest, "START") == 0)
    {
        disp.printSplash(F("Starting..."));
        isDisplay = false;
        subscribeTime = millis() + MESSAGING_START_DELAY;
        disp.setActiveRadio(0);

        if (isConfig)
        {
            isPowerOn = true;
        }

    }
}

void onData()
{
    uint8_t dataIdx = messenger.readInt16Arg();
    bool modeSwitch = false;
    int32_t intVal = 0;
    float_t floatVal = 0.0;

    if (dataIdx == dModeAP) {
        modeSwitch = (bool) messenger.readInt16Arg();
        lights.setAutopilot(modeSwitch);
    } 
    else if (dataIdx == dModeFD) {
        modeSwitch = (bool) messenger.readInt16Arg();
        messenger.sendCmd(kDebug, F("FD Mode not enabled")); // Writing the Spad Log that we turned the FD Annunciator ON...
    }
    else if (dataIdx == dModeHDG) {
        modeSwitch = (bool) messenger.readInt16Arg();
        lights.setHeading(modeSwitch);
    }
    else if (dataIdx == dModeNAV) {
        modeSwitch = (bool) messenger.readInt16Arg();
        lights.setNavigation(modeSwitch);
    }
    else if (dataIdx == dModeALT) {
        modeSwitch = (bool) messenger.readInt16Arg();
        lights.setAltitude(modeSwitch);
    }
    else if (dataIdx == dModeIAS) {
        modeSwitch = (bool) messenger.readInt16Arg();
        messenger.sendCmd(kDebug, F("IAS Mode not enabled")); // Writing the Spad Log that we turned the FD Annunciator ON...
    }    
    else if (dataIdx == dModeVS) {
        modeSwitch = (bool) messenger.readInt16Arg();
        lights.setVerticalSpeed(modeSwitch);
        if (!modeSwitch)
        disp.setVerticalSpeed(0);

    }
    else if (dataIdx == dModeAPR) {
        modeSwitch = (bool) messenger.readInt16Arg();
        lights.setApproach(modeSwitch);
    }
    else if (dataIdx == dModeIAS) {
        modeSwitch = (bool) messenger.readInt16Arg();
        messenger.sendCmd(kDebug, F("IAS Mode not enabled")); // Writing the Spad Log that we turned the FD Annunciator ON...
    }
    else if (dataIdx == dModeREV) {
        modeSwitch = (bool) messenger.readInt16Arg();
        messenger.sendCmd(kDebug, F("REV Mode not enabled")); // Writing the Spad Log that we turned the FD Annunciator ON...
    }
    else if (dataIdx == dValALT) {
        intVal = messenger.readInt32Arg();
        disp.setAltitude(intVal);
        sendDebugMsg(messenger.commandID(), intVal);
    }
    else if (dataIdx == dValVS) {
        intVal = messenger.readInt32Arg();
        disp.setVerticalSpeed(intVal);
        sendDebugMsg(messenger.commandID(), intVal);
    }    
    else if (dataIdx == dValIAS) {
        intVal = messenger.readInt32Arg();
        messenger.sendCmd(kDebug, F("IAS Mode not enabled")); // Writing the Spad Log that we turned the FD Annunciator ON...
    }    
    else if (dataIdx == dValHDG) {
        intVal = messenger.readInt32Arg();
        disp.setHeading(intVal);
        sendDebugMsg(messenger.commandID(), intVal);
    }    
    else if (dataIdx == dValCRS) {
        intVal = messenger.readInt32Arg();
        disp.setCourse(intVal);
        sendDebugMsg(messenger.commandID(), intVal);
    }    
    else if (dataIdx == dValTXPDR) {
        intVal = messenger.readInt32Arg();
        disp.setTransponderCode(intVal);
        sendDebugMsg(messenger.commandID(), intVal);
    }   
    else if (dataIdx == dValRFREQ_A) {
        floatVal = messenger.readFloatArg();
        disp.setRadioFrequencyActive(floatVal);
        sendDebugMsg(messenger.commandID(), floatVal);
    }   
    else if (dataIdx == dValRFREQ_S) {
        floatVal = messenger.readFloatArg();
        disp.setRadioFrequencyStandby(floatVal);
        sendDebugMsg(messenger.commandID(), floatVal);
    }   
    else if (dataIdx == dValBARO) {
        floatVal = messenger.readFloatArg();
        disp.setBarometer(floatVal);
        sendDebugMsg(messenger.commandID(), floatVal);
    }   
    else {
        messenger.sendCmd(kDebug, "Unknown DATA index " + String(dataIdx) + "."); // Writing the Spad Log that we turned the FD Annunciator ON...
    }
}


void subscribeNextData()
{

    if (subscriptions[subscribeIndex].enable)
    {
        // Expose AP Mode ... Mode..
        messenger.sendCmdStart(kCommand);     // This is a "1" or Command:1 from Spad list
        messenger.sendCmdArg(F("SUBSCRIBE")); // Subcommand..ADD - SUBSCRIBE - UNSUBSCRIBE - EMULATE
        messenger.sendCmdArg(subscriptions[subscribeIndex].cmd);
        messenger.sendCmdArg(subscriptions[subscribeIndex].data);
        messenger.sendCmdEnd();

#ifdef DEBUG
        disp.printDebug(String(subscriptions[subscribeIndex].data).substring(0, 24));
#endif
    }
    subscribeIndex++;

    if (subscribeIndex == SUBSCRIPTIONS)
        isReady = true;
}


// Define callbacks for the different SPAD command sets
void attachCommandCallbacks()
{
    // Attach callback methods
    messenger.attach(onUnknownCommand);
    messenger.attach(kRequest, onIdentifyRequest);
    messenger.attach(kEvent, onEvent);
    messenger.attach(kData, onData);

}

void updateRadioSource(uint8_t selection)
{

    // Provides currently selected Radio
    messenger.sendCmdStart(kInput);
    messenger.sendCmdArg(iSelRadio);
    messenger.sendCmdArg(selection);
    messenger.sendCmdEnd();

    // Changes the subscriptions
    messenger.sendCmdStart(kCommand);
    messenger.sendCmdArg(F("UNSUBSCRIBE"));
    messenger.sendCmdArg(dValRFREQ_A);
    messenger.sendCmdEnd();

    messenger.sendCmdStart(kCommand);
    messenger.sendCmdArg(F("SUBSCRIBE"));
    messenger.sendCmdArg(dValRFREQ_A);
    messenger.sendCmdArg(nav_subscribe[selection][0]);
    messenger.sendCmdEnd();

    messenger.sendCmdStart(kCommand);       // This is a "1" or Command:1 from Spad list
    messenger.sendCmdArg(F("UNSUBSCRIBE")); // Subcommand..ADD - SUBSCRIBE - UNSUBSCRIBE - EMULATE
    messenger.sendCmdArg(dValRFREQ_S);         // CMDID value defined at the top as "26" this will be the DATA channel
    messenger.sendCmdEnd();

    messenger.sendCmdStart(kCommand);     // This is a "1" or Command:1 from Spad list
    messenger.sendCmdArg(F("SUBSCRIBE")); // Subcommand..ADD - SUBSCRIBE - UNSUBSCRIBE - EMULATE
    messenger.sendCmdArg(dValRFREQ_S);       // CMDID value defined at the top as "26" this will be the DATA channel
    messenger.sendCmdArg(nav_subscribe[selection][1]);
    messenger.sendCmdEnd();

#ifdef DEBUG
    String msg = F("Radio change: ");
    disp.printDebug(msg + selection);
#endif
}

void updateCourseSource(uint8_t selection)
{
    // Provides currently selected Radio
    messenger.sendCmdStart(kInput);
    messenger.sendCmdArg(iSelCRS);
    messenger.sendCmdArg(selection);
    messenger.sendCmdEnd();

    messenger.sendCmdStart(kCommand);
    messenger.sendCmdArg(F("UNSUBSCRIBE"));
    messenger.sendCmdArg(dValCRS);
    messenger.sendCmdEnd();

    messenger.sendCmdStart(kCommand);     // This is a "1" or Command:1 from Spad list
    messenger.sendCmdArg(F("SUBSCRIBE")); // Subcommand..ADD - SUBSCRIBE - UNSUBSCRIBE - EMULATE
    messenger.sendCmdArg(dValCRS);          // CMDID value defined at the top as "26" this will be the DATA channel
    messenger.sendCmdArg(crs_subscribe[selection]);
    messenger.sendCmdEnd();

#ifdef DEBUG
    String msg = F("Course change: ");
    disp.printDebug(msg + selection);
#endif
}

