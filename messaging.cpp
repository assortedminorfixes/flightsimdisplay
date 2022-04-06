#include "Arduino.h"
#include "display.hh"
#include "lights.hh"
#include "messaging.hh"

//// ------   Spad Coms Section ------ ///////
CmdMessenger messenger(Serial);

bool isReady = false;

// vJoy Device ID to use...There is a way to confirm this but it should be the same for everyone...
// vid:pid:index ....  Example here when you copy vJoy Entire Device (after making events in your profile...) Then paste in Notepad
// you will find the data for the events and device... the vendor ID would show up as "0x1234" the pid would be "0xBEAD" and Index "1"
// Note this example has the Indes of 1 which is the "Second" vJoy...  I already had 1 vJoy in use for Spad to send camera events to MSFS...
// String vJoy="1234:BEAD:1";

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
    messenger.sendCmd(kDebug, F("UNKNOWN COMMAND")); // if a command comes in that is not reckognized from sketch write to the spad log
}

// Callback function to respond to indentify request. This is part of the
// Auto connection handshake.
void onIdentifyRequest()
{
    char *szRequest = messenger.readStringArg();

    if (strcmp(szRequest, "INIT") == 0)
    { // Initial Configuration declaration
        messenger.sendCmdStart(kRequest);
        messenger.sendCmdArg(F("SPAD"));
        messenger.sendCmdArg(F("{7eb4b953-64c6-4c94-a958-1fac034a0370}")); // Unique Device ID: Change this!
        messenger.sendCmdArg(F("SimDisplay"));                             // Device Name for UI
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
        disp.startConfig();

        // Expose Course selector ... Mode..
        messenger.sendCmdStart(kCommand);       // This is a "1" or Command:1 from Spad list
        messenger.sendCmdArg(F("ADD"));         // Subcommand..ADD - SUBSCRIBE - UNSUBSCRIBE - EMULATE
        messenger.sendCmdArg(sCRSs);            // CMDID value defined at the top as "19" this will be the DATA channel
        messenger.sendCmdArg(F("APm/CRSs"));    // will become "SERIAL:<guid>/APm/CRSs"
        messenger.sendCmdArg(F("U8"));          // Value Type .. (Signed/Unsigned) one of S8,S16,S32,S64,U8,U16,U32,U64,FLT32,FLT64,ASCIIZ
        messenger.sendCmdArg(F("RO"));          // Access Type RO - ReadOnly, RW - ReadWrite
        messenger.sendCmdArg(F("CRSselector")); // NAME of item for the UI
        messenger.sendCmdEnd();

        // Expose Radio selector ... Mode..
        messenger.sendCmdStart(kCommand);         // This is a "1" or Command:1 from Spad list
        messenger.sendCmdArg(F("ADD"));           // Subcommand..ADD - SUBSCRIBE - UNSUBSCRIBE - EMULATE
        messenger.sendCmdArg(sRADIOs);            // CMDID value defined at the top as "28" this will be the DATA channel
        messenger.sendCmdArg(F("COMm/RADIOs"));   // will become "SERIAL:<guid>/AP/APRmode"
        messenger.sendCmdArg(F("U8"));            // Value Type .. (Signed/Unsigned) one of S8,S16,S32,S64,U8,U16,U32,U64,FLT32,FLT64,ASCIIZ
        messenger.sendCmdArg(F("RO"));            // Access Type RO - ReadOnly, RW - ReadWrite
        messenger.sendCmdArg(F("RADIOselector")); // NAME of item for the UI
        messenger.sendCmdEnd();

        // tell SPAD.neXT we are done with config
        messenger.sendCmd(kRequest, F("CONFIG"));

        // Expose AP Mode ... Mode..
        messenger.sendCmdStart(kCommand);     // This is a "1" or Command:1 from Spad list
        messenger.sendCmdArg(F("SUBSCRIBE")); // Subcommand..ADD - SUBSCRIBE - UNSUBSCRIBE - EMULATE
        messenger.sendCmdArg(rAPm);
        messenger.sendCmdArg(F("SIMCONNECT:AUTOPILOT MASTER"));
        messenger.sendCmdEnd();

        delay(100);

        // Expose FD Mode ... Mode..
        messenger.sendCmdStart(kCommand);     // This is a "1" or Command:1 from Spad list
        messenger.sendCmdArg(F("SUBSCRIBE")); // Subcommand..ADD - SUBSCRIBE - UNSUBSCRIBE - EMULATE
        messenger.sendCmdArg(rFDm);
        messenger.sendCmdArg(F("SIMCONNECT:AUTOPILOT FLIGHT DIRECTOR ACTIVE"));
        messenger.sendCmdEnd();

        delay(100);

        // Expose HDG Mode ... Mode..
        messenger.sendCmdStart(kCommand);     // This is a "1" or Command:1 from Spad list
        messenger.sendCmdArg(F("SUBSCRIBE")); // Subcommand..ADD - SUBSCRIBE - UNSUBSCRIBE - EMULATE
        messenger.sendCmdArg(rHDGm);
        messenger.sendCmdArg(F("SIMCONNECT:AUTOPILOT HEADING LOCK"));
        messenger.sendCmdEnd();

        delay(100);
        
        // Expose NAV Mode ... Mode..
        messenger.sendCmdStart(kCommand);     // This is a "1" or Command:1 from Spad list
        messenger.sendCmdArg(F("SUBSCRIBE")); // Subcommand..ADD - SUBSCRIBE - UNSUBSCRIBE - EMULATE
        messenger.sendCmdArg(rNAVm);
        messenger.sendCmdArg(F("SIMCONNECT:AUTOPILOT NAV1 LOCK"));
        messenger.sendCmdEnd();

        delay(100);

        // Expose ALT Mode ... Mode..
        messenger.sendCmdStart(kCommand);     // This is a "1" or Command:1 from Spad list
        messenger.sendCmdArg(F("SUBSCRIBE")); // Subcommand..ADD - SUBSCRIBE - UNSUBSCRIBE - EMULATE
        messenger.sendCmdArg(rALTm);
        messenger.sendCmdArg(F("SIMCONNECT:AUTOPILOT ALTITUDE LOCK"));
        messenger.sendCmdEnd();

        delay(100);

        // Expose IAS Mode ... Mode..
        messenger.sendCmdStart(kCommand);     // This is a "1" or Command:1 from Spad list
        messenger.sendCmdArg(F("SUBSCRIBE")); // Subcommand..ADD - SUBSCRIBE - UNSUBSCRIBE - EMULATE
        messenger.sendCmdArg(rIASm);
        messenger.sendCmdArg(F("SIMCONNECT:AUTOPILOT AIRSPEED HOLD"));
        messenger.sendCmdEnd();

        delay(100);

        // Expose VS Mode ... Mode..
        messenger.sendCmdStart(kCommand);     // This is a "1" or Command:1 from Spad list
        messenger.sendCmdArg(F("SUBSCRIBE")); // Subcommand..ADD - SUBSCRIBE - UNSUBSCRIBE - EMULATE
        messenger.sendCmdArg(rVSm);
        messenger.sendCmdArg(F("SIMCONNECT:AUTOPILOT VERTICAL HOLD"));
        messenger.sendCmdEnd();

        delay(100);

        // Expose APR Mode ... Mode..
        messenger.sendCmdStart(kCommand);     // This is a "1" or Command:1 from Spad list
        messenger.sendCmdArg(F("SUBSCRIBE")); // Subcommand..ADD - SUBSCRIBE - UNSUBSCRIBE - EMULATE
        messenger.sendCmdArg(rAPRm);
        messenger.sendCmdArg(F("SIMCONNECT:AUTOPILOT APPROACH HOLD"));
        messenger.sendCmdEnd();

        delay(100);

        // Expose REV Mode ... Mode..
        messenger.sendCmdStart(kCommand);     // This is a "1" or Command:1 from Spad list
        messenger.sendCmdArg(F("SUBSCRIBE")); // Subcommand..ADD - SUBSCRIBE - UNSUBSCRIBE - EMULATE
        messenger.sendCmdArg(rREVm);
        messenger.sendCmdArg(F("SIMCONNECT:AUTOPILOT BACKCOURSE HOLD"));
        messenger.sendCmdEnd();

        delay(100);

        // Subscribe ALT Value
        messenger.sendCmdStart(kCommand);     // This is a "1" or Command:1 from Spad list
        messenger.sendCmdArg(F("SUBSCRIBE")); // Subcommand..ADD - SUBSCRIBE - UNSUBSCRIBE - EMULATE
        messenger.sendCmdArg(rALTv);
        messenger.sendCmdArg(F("SIMCONNECT:AUTOPILOT ALTITUDE LOCK VAR"));
        messenger.sendCmdEnd();

        delay(100);

        // Subscribe VS Value
        messenger.sendCmdStart(kCommand);     // This is a "1" or Command:1 from Spad list
        messenger.sendCmdArg(F("SUBSCRIBE")); // Subcommand..ADD - SUBSCRIBE - UNSUBSCRIBE - EMULATE
        messenger.sendCmdArg(rVSv);
        messenger.sendCmdArg(F("SIMCONNECT:AUTOPILOT VERTICAL HOLD VAR"));
        messenger.sendCmdEnd();

        delay(100);

        // Subscribe IAS Value
        messenger.sendCmdStart(kCommand);     // This is a "1" or Command:1 from Spad list
        messenger.sendCmdArg(F("SUBSCRIBE")); // Subcommand..ADD - SUBSCRIBE - UNSUBSCRIBE - EMULATE
        messenger.sendCmdArg(rIASv);
        messenger.sendCmdArg(F("SIMCONNECT:AUTOPILOT AIRSPEED HOLD VAR"));
        messenger.sendCmdEnd();

        delay(100);

        // Subscribe HDG Value
        messenger.sendCmdStart(kCommand);     // This is a "1" or Command:1 from Spad list
        messenger.sendCmdArg(F("SUBSCRIBE")); // Subcommand..ADD - SUBSCRIBE - UNSUBSCRIBE - EMULATE
        messenger.sendCmdArg(rHDGv);
        messenger.sendCmdArg(F("SIMCONNECT:AUTOPILOT HEADING LOCK DIR"));
        messenger.sendCmdEnd();

        delay(100);

        // Subscribe CRS Value
        messenger.sendCmdStart(kCommand);       // This is a "1" or Command:1 from Spad list
        messenger.sendCmdArg(F("SUBSCRIBE"));   // Subcommand..ADD - SUBSCRIBE - UNSUBSCRIBE - EMULATE
        messenger.sendCmdArg(rCRSv);            // CMDID value defined at the top as "25" this will be the DATA channel
        messenger.sendCmdArg(crs_subscribe[0]); // will become "SERIAL:<guid>/AP/CRSval"
        messenger.sendCmdEnd();

        delay(100);

        // Subscribe TXPDR Value
        messenger.sendCmdStart(kCommand);     // This is a "1" or Command:1 from Spad list
        messenger.sendCmdArg(F("SUBSCRIBE")); // Subcommand..ADD - SUBSCRIBE - UNSUBSCRIBE - EMULATE
        messenger.sendCmdArg(rTXPDRc);
        messenger.sendCmdArg(F("SIMCONNECT:TRANSPONDER CODE:1"));
        messenger.sendCmdEnd();

        delay(100);

        // Subscribe to BAR Value
        messenger.sendCmdStart(kCommand);     // This is a "1" or Command:1 from Spad list
        messenger.sendCmdArg(F("SUBSCRIBE")); // Subcommand..ADD - SUBSCRIBE - UNSUBSCRIBE - EMULATE
        messenger.sendCmdArg(rBARv);
        messenger.sendCmdArg(F("SIMCONNECT:KOHLSMAN SETTING HG"));
        messenger.sendCmdEnd();

        delay(100);

        // Subscribe Radio Active Frequency
        messenger.sendCmdStart(kCommand);     // This is a "1" or Command:1 from Spad list
        messenger.sendCmdArg(F("SUBSCRIBE")); // Subcommand..ADD - SUBSCRIBE - UNSUBSCRIBE - EMULATE
        messenger.sendCmdArg(rRFREQAv);   
        messenger.sendCmdArg(nav_subscribe[0][0]);
        messenger.sendCmdEnd();

        delay(100);

        // Subscribe Radio Active Frequency
        messenger.sendCmdStart(kCommand);          // This is a "1" or Command:1 from Spad list
        messenger.sendCmdArg(F("SUBSCRIBE"));      // Subcommand..ADD - SUBSCRIBE - UNSUBSCRIBE - EMULATE
        messenger.sendCmdArg(rRFREQSv);   
        messenger.sendCmdArg(nav_subscribe[0][1]);
        messenger.sendCmdEnd();


        isReady = true; // This is where we changed that Variable we declared up top now that Config is done.

        return;
    }
}

void onEvent()
{
    char *szRequest = messenger.readStringArg();

    if (strcmp(szRequest, "VIRTUALPOWER") == 0 && isReady)
    {
        uint8_t flag = messenger.readInt16Arg();

        if (flag == 1)
        {
            disp.printStatic();
            disp.setActiveRadio(0);
        }
    }
    else if (strcmp(szRequest, "START") == 0 && isReady)
    {
        disp.printStatic();
        disp.setActiveRadio(0);
    }
}

//  Now you need to figure out how to read all of the values back and then write them to the display.....

// Callback to perform some action

void onAPmodeOn()
{
    bool newAPmode = (bool) messenger.readInt16Arg();
    lights.setAutopilot(newAPmode);
}

void onFDmodeOn()
{
    int32_t newFDmode = messenger.readInt32Arg();
    messenger.sendCmd(kDebug, F("FD Mode not enabled")); // Writing the Spad Log that we turned the FD Annunciator ON...
}

void onHDGmodeOn()
{
    bool newHDGmode = (bool) messenger.readInt16Arg();
    lights.setHeading(newHDGmode);
}

void onNAVmodeOn()
{
    bool newNAVmode = (bool) messenger.readInt16Arg();
    lights.setNavigation(newNAVmode);
}

void onIASmodeOn()
{
    int32_t newIASmode = messenger.readInt32Arg();
    messenger.sendCmd(kDebug, F("IAS Mode not enabled")); // Writing the Spad Log that we turned the FD Annunciator ON...
}

void onVSmodeOn()
{
    bool newVSmode = (bool) messenger.readInt16Arg();
    lights.setVerticalSpeed(newVSmode);
}

void onALTmodeOn()
{
    bool newALTmode = (bool) messenger.readInt16Arg();
    lights.setAltitude(newALTmode);
}

void onAPRmodeOn()
{
    bool newAPRmode = (bool) messenger.readInt16Arg();
    lights.setApproach(newAPRmode);
}

void onREVmodeOn()
{
    int32_t newREVmode = messenger.readInt32Arg();
    messenger.sendCmd(kDebug, F("REV Mode not enabled")); // Writing the Spad Log that we turned the FD Annunciator ON...

}
void onALTvalOn()
{
    int32_t val = messenger.readInt32Arg();
    disp.setAltitude(val);
    sendDebugMsg(messenger.commandID(), val);
}
void onHDGvalOn()
{
    int16_t val = messenger.readInt16Arg();
    disp.setHeading(val);
    sendDebugMsg(messenger.commandID(), val);
}
void onCRSvalOn()
{
    int16_t val = messenger.readInt16Arg();
    disp.setCourse(val);
    sendDebugMsg(messenger.commandID(), val);
}
void onVSvalOn()
{
    int16_t val = messenger.readInt16Arg();
    disp.setVerticalSpeed(val);
    sendDebugMsg(messenger.commandID(), val);
}
void onIASvalOn()
{
    // IGNORE
    return;
}
void onTXPDRcOn()
{
    int16_t val = messenger.readInt16Arg();
    disp.setTransponderCode(val);
    sendDebugMsg(messenger.commandID(), val);
}
void onRFREQAvOn()
{
    float_t val = messenger.readFloatArg();
    disp.setRadioFrequencyActive(val);
    sendDebugMsg(messenger.commandID(), val);
}
void onRFREQSvOn()
{
    float_t val = messenger.readFloatArg();
    disp.setRadioFrequencyStandby(val);
    sendDebugMsg(messenger.commandID(), val);
}
void onRBARvOn()
{
    float_t val = messenger.readFloatArg();
    disp.setBarometer(val);
    sendDebugMsg(messenger.commandID(), val);
}
void attachCommandCallbacks()
{
    // Attach callback methods
    messenger.attach(onUnknownCommand);
    messenger.attach(kRequest, onIdentifyRequest);
    messenger.attach(kEvent, onEvent);
    messenger.attach(rAPm, onAPmodeOn);
    messenger.attach(rFDm, onFDmodeOn);
    messenger.attach(rHDGm, onHDGmodeOn);
    messenger.attach(rNAVm, onNAVmodeOn);
    messenger.attach(rALTm, onALTmodeOn);
    messenger.attach(rIASm, onIASmodeOn);
    messenger.attach(rVSm, onVSmodeOn);
    messenger.attach(rAPRm, onAPRmodeOn);
    messenger.attach(rREVm, onREVmodeOn);
    messenger.attach(rALTv, onALTvalOn);
    messenger.attach(rVSv, onVSvalOn);
    messenger.attach(rIASv, onIASvalOn);
    messenger.attach(rHDGv, onHDGvalOn);
    messenger.attach(rCRSv, onCRSvalOn);
    messenger.attach(rTXPDRc, onTXPDRcOn);
    messenger.attach(rRFREQAv, onRFREQAvOn);
    messenger.attach(rRFREQSv, onRFREQSvOn);
    messenger.attach(rBARv, onRBARvOn);
}

void updateRadioSource(uint8_t selection)
{

    messenger.sendCmd(sRADIOs, selection);

    messenger.sendCmdStart(kCommand);
    messenger.sendCmdArg(F("UNSUBSCRIBE"));
    messenger.sendCmdArg(rRFREQAv);
    messenger.sendCmdEnd();

    messenger.sendCmdStart(kCommand);
    messenger.sendCmdArg(F("SUBSCRIBE"));
    messenger.sendCmdArg(rRFREQAv);
    messenger.sendCmdArg(nav_subscribe[selection][0]);
    messenger.sendCmdEnd();

    messenger.sendCmdStart(kCommand);       // This is a "1" or Command:1 from Spad list
    messenger.sendCmdArg(F("UNSUBSCRIBE")); // Subcommand..ADD - SUBSCRIBE - UNSUBSCRIBE - EMULATE
    messenger.sendCmdArg(rRFREQSv);         // CMDID value defined at the top as "26" this will be the DATA channel
    messenger.sendCmdEnd();

    messenger.sendCmdStart(kCommand);     // This is a "1" or Command:1 from Spad list
    messenger.sendCmdArg(F("SUBSCRIBE")); // Subcommand..ADD - SUBSCRIBE - UNSUBSCRIBE - EMULATE
    messenger.sendCmdArg(rRFREQSv);       // CMDID value defined at the top as "26" this will be the DATA channel
    messenger.sendCmdArg(nav_subscribe[selection][1]);
    messenger.sendCmdEnd();
}

void updateCourseSource(uint8_t selection)
{

    messenger.sendCmd(sCRSs, selection);

    messenger.sendCmdStart(kCommand);
    messenger.sendCmdArg(F("UNSUBSCRIBE"));
    messenger.sendCmdArg(rCRSv);
    messenger.sendCmdEnd();

    messenger.sendCmdStart(kCommand);     // This is a "1" or Command:1 from Spad list
    messenger.sendCmdArg(F("SUBSCRIBE")); // Subcommand..ADD - SUBSCRIBE - UNSUBSCRIBE - EMULATE
    messenger.sendCmdArg(rCRSv);          // CMDID value defined at the top as "26" this will be the DATA channel
    messenger.sendCmdArg(crs_subscribe[selection]);
    messenger.sendCmdEnd();
}