#include "Arduino.h"
#include "lores_display.hh"
#include "lights.hh"
#include "messaging.hh"

constexpr CommsController::InputOutput CommsController::in_outs[];

template <class T>
void CommsController::sendCmdDebugMsg(uint8_t command, uint8_t idx, T arg)
{
    if (state.debug && state.isReady())
        disp.printLastCommand(command, idx, static_cast<int32_t>(arg));

    if (state.serial_debug)        
        messenger.sendCmd(kDebug, arg);
}

template <>
void CommsController::sendCmdDebugMsg<const char*>(uint8_t command, uint8_t idx, const char* arg)
{
    if (state.debug && state.isReady()) 
        disp.printLastCommand(command, idx, arg);

    if (state.serial_debug)
        messenger.sendCmd(kDebug, arg);
}

template <>
void CommsController::sendCmdDebugMsg<String>(uint8_t command, uint8_t idx, const String arg)
{
    if (state.debug && state.isReady()) 
        disp.printLastCommand(command, idx, arg);

    if (state.serial_debug)
        messenger.sendCmd(kDebug, arg);
}

template <>
void CommsController::sendCmdDebugMsg<DisplayField>(uint8_t command, uint8_t idx, DisplayField arg)
{
    if (state.debug && state.isReady())
        disp.printLastCommand(command, idx, arg.to_string().c_str());

    if (state.serial_debug)
        messenger.sendCmd(kDebug, arg.to_string());
}

// ------------------  C A L L B A C K S -----------------------

// Called when a received command has no attached function
void CommsController::onUnknownCommand()
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
void CommsController::onIdentifyRequest()
{
    char *szRequest = messenger.readStringArg();

    if (strcmp(szRequest, "INIT") == 0)
    { // Initial Configuration declaration

        state.power = false;
        state.configured = false;

        if(messenger.readInt32Arg())
        ; // uint8_t apiVersion
        String spadVersion = messenger.readStringArg();
        String spadAuthToken = messenger.readStringArg();

        messenger.sendCmdStart(kRequest);
        messenger.sendCmdArg(F("SPAD"));                                   // Serial Protocol
        messenger.sendCmdArg(F("{7432c03c-ebc0-4aa6-b108-de06e30ca10a}")); // Device GUID
        messenger.sendCmdArg(F("AP_GFC600_Display"));                             // Device Display Name
        messenger.sendCmdArg(2);                                           // SPAD.NEXT Serial Version
        messenger.sendCmdArg(F("0.3"));                                    // FW Version
        messenger.sendCmdArg(F("AUTHOR=aea714065890bf70fe52c90206ba09f2"));
        messenger.sendCmdArg(F("ALLOWLOCAL=0"));
        messenger.sendCmdArg(F("PID=SIMDISPLAY")); // Device ID
        messenger.sendCmdArg(F("VID=SHIMAVAK"));     // Author ID
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
        messenger.sendCmdArg("DEFAULT_PANEL=Display");
        messenger.sendCmdEnd();

        // Expose Inputs and Outputs
        for (uint i = 0; i < sizeof(in_outs)/sizeof(in_outs[0]); i++)
        {
            messenger.sendCmdStart(kRequest);
            messenger.sendCmdArg(in_outs[i].io);
            messenger.sendCmdArg(in_outs[i].idx);
            messenger.sendCmdArg(in_outs[i].name);
            messenger.sendCmdArg(in_outs[i].type);
            messenger.sendCmdArg(in_outs[i].inherit);
            messenger.sendCmdArg(in_outs[i].args);
            messenger.sendCmdEnd();
            delay(MESSAGING_DELAY);
        }

        // tell SPAD.neXT we are done with config
        messenger.sendCmd(kRequest, F("CONFIG"));

        state.configured = true;
        disp.printSplash(F("Standby"));

        return;
    }

    if (strcmp(szRequest, "SCANSTATE") == 0)
    {
        
        // Wait before responding to SCANSTATE
        delay(STARTUP_DELAY);

        messenger.sendCmd(kRequest, F("STATESCAN,1"));

        // Send Virtual Power off.
        sendInput(iPower, 0, F("Power change: "));
        delay(MESSAGING_DELAY);

        /*
        // Provides currently selected Radio
        updateRadioSource(state.radio.sel);
        delay(MESSAGING_DELAY);

        // Provides currently selected CRS
        updateCourseSource(state.nav.crs_sel);
        delay(MESSAGING_DELAY);

        // Provides currently selected Baro mode
        updateBaroMode(state.nav.baro_mode_sel);
        delay(MESSAGING_DELAY);
        */

        messenger.sendCmd(kRequest, F("STATESCAN,2"));
        return;
    }
}

void CommsController::onEvent()
{
    char *szRequest = messenger.readStringArg();

    char *str = NULL;

    if (strcmp(szRequest, "VIRTUALPOWER") == 0)
        state.power = messenger.readBoolArg();
    else if (strcmp(szRequest, "PROFILECHANGED") == 0)
        str = messenger.readStringArg();
    else if (strcmp(szRequest, "PROFILECHANGING") == 0)
        str = messenger.readStringArg();
    else if (strcmp(szRequest, "PROVIDER") == 0)
        str = messenger.readStringArg();
    else if (strcmp(szRequest, "AIRCRAFTCHANGED") == 0)
        str = messenger.readStringArg();
    else if (strcmp(szRequest, "PAGE") == 0)
        str = messenger.readStringArg();
    else if (strcmp(szRequest, "START") == 0)
        state.start_time = millis();
    else if (strcmp(szRequest, "GAMESTATE") == 0)
        str = messenger.readStringArg();

    if(str != NULL && state.debug)
    {
        disp.printDebug(String(str));
    }
        
    // Ensure everything has been read in.
    //while (messenger.readStringArg()[0] != '\0');
    while (messenger.available())
    {
        if(messenger.readStringArg());
    }

}

void CommsController::updateDisplayField(DisplayField *field, uint8_t row)
{
    field->vtype = row;
    updateDisplayField(field);
}

void CommsController::updateDisplayField(DisplayField* field) {
    switch(field->vtype){

        case 0:
            field->value = messenger.readFloatArg();
            break;
        case 1:
            field->dashes = messenger.readBoolArg();
            break;
        case 2:
            field->dot = messenger.readBoolArg();
            break;
        case 3:
            String lbl(messenger.readStringArg());
            lbl.trim();
            field->label = lbl;
            break;
    }   
}

void CommsController::onData()
{
    if (messenger.commandID() == kDisplay)
    {
        // Clear next two fields for DISPLAY input.
        int16_t dataIdx = messenger.readInt16Arg();
        int16_t row = messenger.readInt16Arg();
        int16_t command = messenger.readInt16Arg();
        const char *val = messenger.readStringArg();

        sendCmdDebugMsg(messenger.commandID(), dataIdx, val);
        #if defined(USE_LORES_DISPLAY)
        switch(dataIdx){
            case dLatMode:
                disp.LatMode_TextBox.update(val, command);
                break;
            case dLatModeValue:
                disp.LatModeValue_TextBox.update(val, command);
                break;
            case dLatModeArm:
                disp.LatModeArm_TextBox.update(val, command);
                break;
            case dVertMode:
                disp.VertMode_TextBox.update(val, command);
                break;
            case dVertModeValue:
                disp.VertModeValue_TextBox.update(val, command);
                break;
            case dVertModeUnits:
                disp.VertModeUnits_TextBox.update(val, command);
                break;
            case dVertModeArmLeft:
                disp.VertModeArmLeft_TextBox.update(val, command);
                break;
            case dVertModeArmLeftSpecialMode:
                if (row == 1)
                    disp.VertModeArmLeftSpecialMode_TextBox.update(val, command);
                else
                    disp.VertModeArmLeftSpecialMode_TextBox.updateALT(atoi(val), command);
                break;
            case dVertModeArmRight:
                disp.VertModeArmRight_TextBox.update(val, command);
                break;
        }
        #endif // USE_LORES_DISPLAY
        #ifdef USE_HIRES_DISPLAY
        if (command == 2)
        {
            if (dataIdx == dValALT)
            {
                updateDisplayField(&state.nav.alt, row);
                disp.updateAltitude();
                sendCmdDebugMsg(messenger.commandID(), dataIdx, state.nav.alt);
            }
            else if (dataIdx == dValVS)
            {
                updateDisplayField(&state.nav.vs, row);
                disp.updateVS();
                disp.updateVSLabel();
                sendCmdDebugMsg(messenger.commandID(), dataIdx, state.nav.vs);
            }
            else if (dataIdx == dValIAS)
            {
                updateDisplayField(&state.nav.ias, row);
                disp.updateIAS();
                disp.updateIASLabel();
                sendCmdDebugMsg(messenger.commandID(), dataIdx, state.nav.ias);
            }            
            else if (dataIdx == dValHDG)
            {
                updateDisplayField(&state.nav.hdg, row);
                disp.updateHeading();
                disp.updateHeadingLabel();
                sendCmdDebugMsg(messenger.commandID(), dataIdx, state.nav.hdg);
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
        }
        else
        {
            messenger.sendCmd(kDebug, "Unknown DATA index " + String(dataIdx) + "."); // Writing the Spad Log that we turned the FD Annunciator ON...
        }
        #endif // USE_HIRES_DISPLAY
        // Ensure everything has been read in.
        //while (messenger.readStringArg()[0] != '\0');
        // Get rid of linker warnings
        while(messenger.available())
        {
            if(messenger.readStringArg());
        }
        
    }
}

void CommsController::onLED()
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

    sendCmdDebugMsg(messenger.commandID(), dataIdx, enable);

    return;

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
}

// Define callbacks for the different SPAD command sets
void CommsController::attachCommandCallbacks()
{
    // Attach callback methods
    messenger.attach(onUnknownCommand);
    messenger.attach(kRequest, onIdentifyRequest);
    messenger.attach(kEvent, onEvent);
    messenger.attach(kData, onData);
    messenger.attach(kDisplay, onData);
    messenger.attach(kLED, onLED);
}

template <class T>
void CommsController::sendInput(uint8_t input, T selection, String msg)
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

void CommsController::updateEncoder1(int32_t val){
    String msg = F("Encoder1 Change: ");
    sendInput(iEncoder1, val, msg);
}


void CommsController::updateButton1(bool _state){
    String msg = F("Button1 Change: ");
    sendInput(iEncoder1Btn, _state, msg);
}


void CommsController::updateRadioSource(uint8_t selection)
{
    String msg = F("Radio change: ");
    sendInput(iSelRadio, selection, msg);
}

void CommsController::updateCourseSource(uint8_t selection)
{
    String msg = F("Course change: ");
    sendInput(iSelCRS, selection, msg);
}

void CommsController::updateBaroMode(uint8_t selection)
{
    String msg = F("Baro change: ");
    state.nav.baro = 0.0;
    sendInput(iSelBaro, selection, msg);
}

void CommsController::processInputData()
{
    messenger.feedinSerialData();
}