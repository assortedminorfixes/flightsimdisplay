#ifndef MESSAGING_H
#define MESSAGING_H

#include <CmdMessenger.h> // CmdMessenger ... v4.2 was used when making this sketch

#define MESSAGING_DELAY 50
#define MESSAGING_START_DELAY 2500

enum : byte
{
    kRequest = 0,    // Request from SPAD.neXt              ... Documentation lists as "Command:0"
    kCommand = 1,    // Command to SPAD.neXt                ... Documentation lists as "Command:1"
    kEvent = 2,      // Events from SPAD.neXt                 ... Documentation lists as "Command:2"
    kDebug = 3,      // Debug strings to SPAD.neXt Logfile    ... Documentation lists as "Debug Channel"
    kSimCommand = 4, // Send Event to Simulation         ... Documentation lists as "Command:4"
    kData = 5,       // Send Data to Device
    kInput = 8,      // Update Input Value from Device
};

enum : byte
{
    dModeAP = 10,     // CMDID for exposed data to SPAD.neXt.  We will see the data later as a Local Variable in Spad.Next
    dModeFD = 11,     //
    dModeHDG = 12,    //
    dModeNAV = 13,    //
    dModeALT = 14,    //
    dModeIAS = 15,    //
    dModeVS = 16,     //
    dModeAPR = 17,    //
    dModeREV = 18,    //
    dValALT = 21,    //
    dValVS = 22,     //
    dValIAS = 23,    //
    dValHDG = 24,    //
    dValCRS = 25,    //
    dValTXPDR = 26,  //
    dValBARO = 27,    //
    dValRFREQ_A = 29, //
    dValRFREQ_S = 30, //
};

enum : byte
{
    iSelRadio = 1,
    iSelCRS = 2,
};

struct Subscription
{
    uint8_t cmd;
    const char *data;
    bool enable;
};

#define SUBSCRIPTIONS 18
const Subscription subscriptions[SUBSCRIPTIONS] PROGMEM = {{dModeAP, "SIMCONNECT:AUTOPILOT MASTER", true},
                                               {dModeFD, "SIMCONNECT:AUTOPILOT FLIGHT DIRECTOR ACTIVE", false},
                                               {dModeHDG, "SIMCONNECT:AUTOPILOT HEADING LOCK", true},
                                               {dModeNAV, "SIMCONNECT:AUTOPILOT NAV1 LOCK", true},
                                               {dModeALT, "SIMCONNECT:AUTOPILOT ALTITUDE LOCK", true},
                                               {dModeIAS, "SIMCONNECT:AUTOPILOT AIRSPEED HOLD", false},
                                               {dModeVS, "SIMCONNECT:AUTOPILOT VERTICAL HOLD", true},
                                               {dModeAPR, "SIMCONNECT:AUTOPILOT APPROACH HOLD", true},
                                               {dModeREV, "SIMCONNECT:AUTOPILOT BACKCOURSE HOLD", false},
                                               {dValALT, "SIMCONNECT:AUTOPILOT ALTITUDE LOCK VAR", true},
                                               {dValVS, "SIMCONNECT:AUTOPILOT VERTICAL HOLD VAR", true},
                                               {dValIAS, "SIMCONNECT:AUTOPILOT AIRSPEED HOLD VAR", false},
                                               {dValHDG, "SIMCONNECT:AUTOPILOT HEADING LOCK DIR", true},
                                               {dValCRS, "SIMCONNECT:NAV OBS:1", true},
                                               {dValTXPDR, "SIMCONNECT:TRANSPONDER CODE:1", true},
                                               {dValBARO, "SIMCONNECT:KOHLSMAN SETTING HG", true},
                                               {dValRFREQ_A, "SIMCONNECT:NAV ACTIVE FREQUENCY:1", true},
                                               {dValRFREQ_S, "SIMCONNECT:NAV STANDBY FREQUENCY:1", true}};

const char *const nav_subscribe[5][2] PROGMEM = {{"SIMCONNECT:NAV ACTIVE FREQUENCY:1", "SIMCONNECT:NAV STANDBY FREQUENCY:1"},
                                                 {"SIMCONNECT:NAV ACTIVE FREQUENCY:2", "SIMCONNECT:NAV STANDBY FREQUENCY:2"},
                                                 {"SIMCONNECT:COM ACTIVE FREQUENCY:1", "SIMCONNECT:COM STANDBY FREQUENCY:1"},
                                                 {"SIMCONNECT:COM ACTIVE FREQUENCY:2", "SIMCONNECT:COM STANDBY FREQUENCY:2"},
                                                 {"SIMCONNECT:ADF ACTIVE FREQUENCY:1", "SIMCONNECT:ADF STANDBY FREQUENCY:1"}};

const char *const crs_subscribe[2] PROGMEM = {"SIMCONNECT:NAV OBS:1", "SIMCONNECT:NAV OBS:2"};

void attachCommandCallbacks();
void updateRadioSource(uint8_t selection);
void updateCourseSource(uint8_t selection);
extern CmdMessenger messenger;
extern bool isReady; // We will use this later to allow us to know when configuration is done.
extern bool isPowerOn;
extern bool isConfig;
extern bool isDisplay;
extern unsigned long subscribeTime;
extern void subscribeNextData();
#endif