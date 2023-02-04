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
    if (state.debug) disp.printLastCommand(command, idx, static_cast<int32_t>(arg));
    // messenger.sendCmd(kDebug, arg);
}

// ------------------  C A L L B A C K S -----------------------

// Called when a received command has no attached function
void onUnknownCommand()
{
    uint8_t cmd = messenger.commandID();

    if (state.debug) {
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
        messenger.sendCmdArg(F("ALLOWLOCAL=2"));
        messenger.sendCmdArg(F("PID=SIMDISPLAY")); // AUTHOR ID
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
        messenger.sendCmdArg("ISGENERIC=" + String(1));
        messenger.sendCmdArg("PAGESUPPORT=" + String(0));
        messenger.sendCmdArg("OUT_COOLDOWN=" + String(50));
        messenger.sendCmdArg("NO_DISPLAY_CLEAR=" + String(1));
        messenger.sendCmdArg("VPSUPPORT=" + String(1));
        messenger.sendCmdArg("UI_TYPE=0");
        messenger.sendCmdEnd();

        messenger.sendCmdStart(kRequest);
        messenger.sendCmdArg(F("INPUT"));
        messenger.sendCmdArg(F("0"));
        messenger.sendCmdArg(F("CONFIGURE_PANEL_STATUS"));
        messenger.sendCmdArg(F("SYSTEM"));
        messenger.sendCmdArg(F("SPAD_VIRTUAL_POWER"));
        messenger.sendCmdArg(F("UI_TYPE=3"));
        messenger.sendCmdArg(F("CUSTOM_TYPE=POWER"));
        messenger.sendCmdEnd();

        // Expose Inputs
        for (int i = 0; i < MSG_INPUTS; i++)
        {
            messenger.sendCmdStart(kRequest);
            messenger.sendCmdArg(F("INPUT"));
            messenger.sendCmdArg(inputs[i].idx);
            messenger.sendCmdArg(inputs[i].name);
            messenger.sendCmdArg(inputs[i].type);
            messenger.sendCmdArg(inputs[i].inherit);
            messenger.sendCmdArg(inputs[i].args);
            messenger.sendCmdEnd();
        }

        // Expose Outputs
        for (int i = 0; i < MSG_OUTPUTS; i++)
        {
            messenger.sendCmdStart(kRequest);
            messenger.sendCmdArg(F("OUTPUT"));
            messenger.sendCmdArg(outputs[i].idx);
            messenger.sendCmdArg(outputs[i].name);
            messenger.sendCmdArg(outputs[i].type);
            messenger.sendCmdArg(outputs[i].inherit);
            messenger.sendCmdArg(outputs[i].args);
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
        if (flag != state.power) {
            state.power = flag;

            if (!state.power) {
                disp.printSplash(F("-"));
            }

        }
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
        if (!state.power) {
            disp.printSplash(F("-"));
        }
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
        messenger.readInt16Arg();
    }

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

void onLED()
{

    uint8_t dataIdx = messenger.readInt16Arg();
    bool enable = messenger.readBoolArg();
    String tag = messenger.readStringArg();
    String color = messenger.readStringArg();

    LightState ls = LightState();

    if (enable) {
        ls.style = LightStyle::BRIGHT;

        if (color.equals("YELLOW")) {
            ls.color = LightColor::YELLOW;
        } else if (color.equals("GREEN")) {
            ls.color = LightColor::GREEN;
        }
    }

    if (dataIdx == dModeAP)
    {
        lights.setAutopilot(ls);
    }
    else if (dataIdx == dModeFD)
    {
        messenger.sendCmd(kDebug, F("FD Mode not enabled")); // Writing the Spad Log that we turned the FD Annunciator ON...
    }
    else if (dataIdx == dModeHDG)
    {
        lights.setHeading(ls);
    }
    else if (dataIdx == dModeNAV)
    {
        lights.setNavigation(ls);
    }
    else if (dataIdx == dModeALT)
    {
        lights.setAltitude(ls);
    }
    else if (dataIdx == dModeIAS)
    {
        messenger.sendCmd(kDebug, F("IAS Mode not enabled")); // Writing the Spad Log that we turned the FD Annunciator ON...
    }
    else if (dataIdx == dModeVS)
    {
        lights.setVerticalSpeed(ls);
    }
    else if (dataIdx == dModeAPR)
    {
        lights.setApproach(ls);
    }
    else if (dataIdx == dModeREV)
    {
        messenger.sendCmd(kDebug, F("REV Mode not enabled")); // Writing the Spad Log that we turned the FD Annunciator ON...
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

void updateRadioSource(uint8_t selection)
{
    // Provides currently selected Radio
    messenger.sendCmdStart(kInput);
    messenger.sendCmdArg(iSelRadio);
    messenger.sendCmdArg(selection);
    messenger.sendCmdEnd();

    if (state.debug)
    {
        String msg = F("Radio change: ");
        disp.printDebug(msg + selection + "  ");
    }
}

void updateCourseSource(uint8_t selection)
{
    // Provides currently selected Course
    messenger.sendCmdStart(kInput);
    messenger.sendCmdArg(iSelCRS);
    messenger.sendCmdArg(selection);
    messenger.sendCmdEnd();

    if (state.debug)
    {
        String msg = F("Course change: ");
        disp.printDebug(msg + selection);
    }
}

void updateSpeedMode(uint8_t selection)
{
    // Updates the speed mode
    messenger.sendCmdStart(kInput);
    messenger.sendCmdArg(iSelAPSpeed);
    messenger.sendCmdArg(selection);
    messenger.sendCmdEnd();

    if (state.debug) { 
        String msg = F("Speed change: ");
        disp.printDebug(msg + selection);
    }
}

void updateBaroMode(uint8_t selection)
{
    // Updates the baro mode
    messenger.sendCmdStart(kInput);
    messenger.sendCmdArg(iSelBaro);
    messenger.sendCmdArg(selection);
    messenger.sendCmdEnd();

    if (state.debug) { 
        String msg = F("Baro change: ");
        disp.printDebug(msg + selection);
    }
}
