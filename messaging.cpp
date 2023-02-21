#include "Arduino.h"
#include "state.hh"
#include "display.hh"
#include "lights.hh"
#include "messaging.hh"

//// ------   Spad Coms Section ------ ///////
CmdMessenger messenger(Serial);

template <class T>
void sendCmdDebugMsg(uint8_t command, uint8_t idx, T arg)
{
    if (state.debug && state.isReady())
        disp.printLastCommand(command, idx, static_cast<int32_t>(arg));
    // messenger.sendCmd(kDebug, arg);
}

// ------------------  C A L L B A C K S -----------------------

// Called when a received command has no attached function
void onUnknownCommand()
{
    uint8_t cmd = messenger.commandID();

    if (state.debug)
    {
        String msg = F("Unknown command: ");
        disp.printDebug(msg + cmd);
    }

    messenger.sendCmd(kDebug, F("UNKNOWN COMMAND")); // if a command comes in that is not reckognized from sketch write to the spad log
}

// Callback function to respond to indentify request. This is part of the
// Auto connection handshake.
void onIdentifyRequest()
{
    char *szRequest = messenger.readStringArg();

    if (strcmp(szRequest, "INIT") == 0)
    { // Initial Configuration declaration

        state.power = false;
        state.configured = false;

        uint8_t apiVersion = messenger.readInt32Arg();
        String spadVersion = messenger.readStringArg();
        String spadAuthToken = messenger.readStringArg();

        messenger.sendCmdStart(kRequest);
        messenger.sendCmdArg(F("SPAD"));                                   // Serial Protocol
        messenger.sendCmdArg(F("{7eb4b953-64c6-4c94-a958-1fac034a0370}")); // Device GUID
        messenger.sendCmdArg(F("SimDisplay"));                             // Device Display Name
        messenger.sendCmdArg(2);                                           // SPAD.NEXT Serial Version
        messenger.sendCmdArg(F("0.3"));                                    // FW Version
        messenger.sendCmdArg(F("AUTHOR=1683e5ce90820838a39d0d3990f4c266"));
        messenger.sendCmdArg(F("ALLOWLOCAL=0"));
        messenger.sendCmdArg(F("PID=SIMDISPLAY")); // Device ID
        messenger.sendCmdArg(F("VID=LARSLL"));     // Author ID
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

        messenger.sendCmdStart(kRequest);
        messenger.sendCmdArg("OPTION");
        messenger.sendCmdArg("ISGENERIC=1");
        messenger.sendCmdArg("PAGESUPPORT=0");
        messenger.sendCmdArg("OUT_COOLDOWN=" + String(MESSAGING_DELAY));
        messenger.sendCmdArg("NO_DISPLAY_CLEAR=1");
        messenger.sendCmdArg("NO_LED_CLEAR=1");
        messenger.sendCmdArg("VPSUPPORT=1");
        messenger.sendCmdArg("UI_TYPE=0");
        messenger.sendCmdArg("DEFAULT_PANEL=Switches");
        messenger.sendCmdEnd();

        // Expose Inputs and Outputs
        for (int i = 0; i < MSG_INOUTS; i++)
        {
            messenger.sendCmdStart(kRequest);
            messenger.sendCmdArg(in_outs[i].io);
            messenger.sendCmdArg(in_outs[i].idx);
            messenger.sendCmdArg(in_outs[i].name);
            messenger.sendCmdArg(in_outs[i].type);
            messenger.sendCmdArg(in_outs[i].inherit);
            messenger.sendCmdArg(in_outs[i].args);
            messenger.sendCmdEnd();
        }

        // tell SPAD.neXT we are done with config
        messenger.sendCmd(kRequest, F("CONFIG"));

        state.configured = true;
        disp.printSplash(F("Standby"));

        return;
    }

    if (strcmp(szRequest, "SCANSTATE") == 0)
    {
        char *str = messenger.readStringArg();

        messenger.sendCmd(kRequest, F("STATESCAN,1"));

        // Send Virtual Power off.
        sendInput(iPower, 0, F("Power change: "));

        // Provides currently selected Radio
        updateRadioSource(state.radio.sel);

        // Provides currently selected CRS
        updateCourseSource(state.nav.crs_sel);

        // Provides currently selected Speed Mode
        updateSpeedMode(state.nav.speed_mode_sel);

        // Provides currently selected Baro mode
        updateBaroMode(state.nav.baro_mode_sel);

        messenger.sendCmd(kRequest, F("STATESCAN,2"));
        return;
    }
}

void onEvent()
{
    char *szRequest = messenger.readStringArg();

    if (strcmp(szRequest, "VIRTUALPOWER") == 0)
    {
        bool flag = messenger.readBoolArg();
        state.power = flag;
    }
    else if (strcmp(szRequest, "PROFILECHANGED") == 0)
    {
        char *str = messenger.readStringArg();
        return;
    }
    else if (strcmp(szRequest, "PROFILECHANGING") == 0)
    {
        char *str = messenger.readStringArg();
        return;
    }
    else if (strcmp(szRequest, "PROVIDER") == 0)
    {
        char *str = messenger.readStringArg();
        uint8_t avail = messenger.readBoolArg();
        return;
    }
    else if (strcmp(szRequest, "AIRCRAFTCHANGED") == 0)
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
        state.start_time = millis();
    }
}

void onData()
{
    uint8_t dataIdx = messenger.readInt16Arg();
    bool modeSwitch = false;
    int32_t intVal = 0;
    float_t floatVal = 0.0;

    if (messenger.commandID() == kDisplay)
    {
        // Clear next two fields for DISPLAY input.
        messenger.readInt16Arg();
    }

    if (messenger.readInt16Arg() == 2)
    {
        if (dataIdx == dValALT)
        {
            state.nav.alt = messenger.readInt32Arg();
            disp.updateAltitude();
            sendCmdDebugMsg(messenger.commandID(), dataIdx, state.nav.alt);
        }
        else if (dataIdx == dValSpeed)
        {
            state.nav.speed = messenger.readFloatArg();
            disp.updateSpeed();
            sendCmdDebugMsg(messenger.commandID(), dataIdx, state.nav.speed);
        }
        else if (dataIdx == dValHDG)
        {
            state.nav.hdg = messenger.readInt32Arg();
            disp.updateHeading();
            sendCmdDebugMsg(messenger.commandID(), dataIdx, state.nav.hdg);
        }
        else if (dataIdx == dValCRS)
        {
            state.nav.crs = messenger.readInt32Arg();
            disp.updateCourse();
            sendCmdDebugMsg(messenger.commandID(), dataIdx, state.nav.crs);
        }
        else if (dataIdx == dValTXPDR)
        {
            state.radio.xpdr = messenger.readInt32Arg();
            disp.updateTransponderCode();
            sendCmdDebugMsg(messenger.commandID(), dataIdx, state.radio.xpdr);
        }
        else if (dataIdx == dValRFREQ_A)
        {
            state.radio.freq.active = messenger.readFloatArg();
            disp.updateRadioFrequencyActive();
            sendCmdDebugMsg(messenger.commandID(), dataIdx, state.radio.freq.active);
        }
        else if (dataIdx == dValRFREQ_S)
        {
            state.radio.freq.standby = messenger.readFloatArg();
            disp.updateRadioFrequencyStandby();
            sendCmdDebugMsg(messenger.commandID(), dataIdx, state.radio.freq.standby);
        }
        else if (dataIdx == dValBARO)
        {
            state.nav.baro = messenger.readFloatArg();
            disp.updateBarometer();
            sendCmdDebugMsg(messenger.commandID(), dataIdx, state.nav.baro);
        }
        else
        {
            messenger.sendCmd(kDebug, "Unknown DATA index " + String(dataIdx) + "."); // Writing the Spad Log that we turned the FD Annunciator ON...
        }
    }
}

void onLED()
{

    uint8_t dataIdx = messenger.readInt16Arg();
    bool enable = messenger.readBoolArg();
    String tag = messenger.readStringArg();
    String color = messenger.readStringArg();

    LightState ls = LightState();

    if (enable)
    {
        ls.style = LightStyle::BRIGHT;

        if (color.equals("YELLOW"))
        {
            ls.color = LightColor::YELLOW;
        }
        else if (color.equals("GREEN"))
        {
            ls.color = LightColor::GREEN;
        }
    }

    switch (dataIdx)
    {
    case dModeAP:
        lights.setAutopilot(ls);
        break;
    case dModeFD:
        messenger.sendCmd(kDebug, F("FD Mode not enabled")); // Writing the Spad Log that we turned the FD Annunciator ON...
        break;
    case dModeHDG:
        lights.setHeading(ls);
        break;
    case dModeNAV:
        lights.setNavigation(ls);
        break;
    case dModeALT:
        lights.setAltitude(ls);
        break;
    case dModeIAS:
        messenger.sendCmd(kDebug, F("IAS Mode not enabled")); // Writing the Spad Log that we turned the FD Annunciator ON...
        break;
    case dModeVS:
        lights.setVerticalSpeed(ls);
        break;
    case dModeAPR:
        lights.setApproach(ls);
        break;
    case dModeREV:
        messenger.sendCmd(kDebug, F("REV Mode not enabled")); // Writing the Spad Log that we turned the FD Annunciator ON...
    default:
        break;
    }

    sendCmdDebugMsg(messenger.commandID(), dataIdx, enable);
}

// Define callbacks for the different SPAD command sets
void attachCommandCallbacks()
{
    // Attach callback methods
    messenger.attach(onUnknownCommand);
    messenger.attach(kRequest, onIdentifyRequest);
    messenger.attach(kEvent, onEvent);
    messenger.attach(kData, onData);
    messenger.attach(kDisplay, onData);
    messenger.attach(kLED, onLED);
}

void sendInput(uint8_t input, uint8_t selection, String msg)
{
    // Provides currently selected Radio
    messenger.sendCmdStart(kInput);
    messenger.sendCmdArg(input);
    messenger.sendCmdArg(selection);
    messenger.sendCmdEnd();

    if (state.debug && state.isReady())
    {
        disp.printDebug(msg + selection + "  ");
    }
}

void updateRadioSource(uint8_t selection)
{
    String msg = F("Radio change: ");
    sendInput(iSelRadio, selection, msg);
}

void updateCourseSource(uint8_t selection)
{
    String msg = F("Course change: ");
    sendInput(iSelCRS, selection, msg);
}

void updateSpeedMode(uint8_t selection)
{
    String msg = F("Speed change: ");
    state.nav.speed = 0.0;
    sendInput(iSelAPSpeed, selection, msg);
}

void updateBaroMode(uint8_t selection)
{
    String msg = F("Baro change: ");
    state.nav.baro = 0.0;
    sendInput(iSelBaro, selection, msg);
}
