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
                     // Command IDs 5-9 are Reserved.
                     // Command IDs 10-49 are for Data Updates.  Where we "expose/subscibe" to data to process in our sketch..
    kInput = 8,
    rAPm = 10,     // CMDID for exposed data to SPAD.neXt.  We will see the data later as a Local Variable in Spad.Next
    rFDm = 11,     //
    rHDGm = 12,    //
    rNAVm = 13,    //
    rALTm = 14,    //
    rIASm = 15,    //
    rVSm = 16,     //
    rAPRm = 17,    //
    rREVm = 18,    //
    sCRSs = 19,    //
    rAVMv = 20,    // UNUSED
    rALTv = 21,    //
    rVSv = 22,     //
    rIASv = 23,    //
    rHDGv = 24,    //
    rCRSv = 25,    //
    rTXPDRc = 26,  //
    rBARv = 27,    //
    sRADIOs = 28,  //
    rRFREQAv = 29, //
    rRFREQSv = 30, //
};

enum : byte
{
    iRadioSel = 1,
    iCrsSel = 2,
};

struct Subscription
{
    uint8_t cmd;
    const char *data;
    bool enable;
};

#define SUBSCRIPTIONS 18
const Subscription subscriptions[SUBSCRIPTIONS] PROGMEM = {{rAPm, "SIMCONNECT:AUTOPILOT MASTER", true},
                                               {rFDm, "SIMCONNECT:AUTOPILOT FLIGHT DIRECTOR ACTIVE", false},
                                               {rHDGm, "SIMCONNECT:AUTOPILOT HEADING LOCK", true},
                                               {rNAVm, "SIMCONNECT:AUTOPILOT NAV1 LOCK", true},
                                               {rALTm, "SIMCONNECT:AUTOPILOT ALTITUDE LOCK", true},
                                               {rIASm, "SIMCONNECT:AUTOPILOT AIRSPEED HOLD", false},
                                               {rVSm, "SIMCONNECT:AUTOPILOT VERTICAL HOLD", true},
                                               {rAPRm, "SIMCONNECT:AUTOPILOT APPROACH HOLD", true},
                                               {rREVm, "SIMCONNECT:AUTOPILOT BACKCOURSE HOLD", false},
                                               {rALTv, "SIMCONNECT:AUTOPILOT ALTITUDE LOCK VAR", true},
                                               {rVSv, "SIMCONNECT:AUTOPILOT VERTICAL HOLD VAR", true},
                                               {rIASv, "SIMCONNECT:AUTOPILOT AIRSPEED HOLD VAR", false},
                                               {rHDGv, "SIMCONNECT:AUTOPILOT HEADING LOCK DIR", true},
                                               {rCRSv, "SIMCONNECT:NAV OBS:1", true},
                                               {rTXPDRc, "SIMCONNECT:TRANSPONDER CODE:1", true},
                                               {rBARv, "SIMCONNECT:KOHLSMAN SETTING HG", true},
                                               {rRFREQAv, "SIMCONNECT:NAV ACTIVE FREQUENCY:1", true},
                                               {rRFREQSv, "SIMCONNECT:NAV STANDBY FREQUENCY:1", true}};

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