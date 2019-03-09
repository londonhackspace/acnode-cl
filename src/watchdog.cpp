#include <stdint.h>
#include <driverlib/sysctl.h>
#include <driverlib/watchdog.h>

#include "watchdog.h"

Watchdog::Watchdog(void) {
  // how long the watchdog can run for before resetting the connected launchpad
  // 15 seconds, EthernetClient.connect() timeout is 10 seconds (?)
  _period = 15 * F_CPU;
}

// period in seconds, must be less that 35 or we get a 32 bit int overflow
Watchdog::Watchdog(uint32_t period) {
  // For a TM4C1294XL connected launchpad the System clock is 120Mhz, but SysCtlClockGet()
  // gives us 9.6Mhz not sure whats up with that
  
  // how long the watchdog can run for before resetting the connected launchpad
  _period = period * F_CPU;
}

void Watchdog::begin() {
  
  Serial.print("Running with watchdog period of ");
  Serial.println(_period);

  Serial.print("system clock might be ");
  Serial.print(SysCtlClockGet());
  Serial.print(", F_CPU is ");
  Serial.println(F_CPU);

  _was_wdog_reset = false;

  // see why we reset last time
  
  uint32_t rc = SysCtlResetCauseGet();
  
  if (rc) {
    Serial.print("Last reset was from: ");
    if (rc & SYSCTL_CAUSE_WDOG1) {
      Serial.print("WDOG1 ");
    }
    if (rc & SYSCTL_CAUSE_SW) {
      Serial.print("SW ");
    }
    if (rc & SYSCTL_CAUSE_WDOG0) {
      Serial.print("WDOG0 ");
       _was_wdog_reset = true;
    }
    if (rc & SYSCTL_CAUSE_BOR) {
      Serial.print("BOR ");
    }
    if (rc & SYSCTL_CAUSE_POR) {
      Serial.print("POR ");
    }
    if (rc & SYSCTL_CAUSE_EXT) {
      Serial.print("EXT ");
    }
    Serial.println();
    SysCtlResetCauseClear(SYSCTL_CAUSE_WDOG1 | SYSCTL_CAUSE_SW |
                          SYSCTL_CAUSE_WDOG0 | SYSCTL_CAUSE_BOR |
                          SYSCTL_CAUSE_POR | SYSCTL_CAUSE_EXT);
  }
  
  if (SysCtlPeripheralPresent(SYSCTL_PERIPH_WDOG0)) {    
    // enable it
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_WDOG0);

    // stall the watchdog when debuging
    WatchdogStallEnable(WATCHDOG0_BASE);

    // dunno what units the value here is
    // systemclock or PIOSC
    WatchdogReloadSet(WATCHDOG0_BASE, _period);

    // enable the watchdog resetting the system
    WatchdogResetEnable(WATCHDOG0_BASE);

    // enable watchdog
    WatchdogEnable(WATCHDOG0_BASE);
    
    _running = true;
  }

}

void Watchdog::disable() {
  WatchdogResetDisable(WATCHDOG0_BASE);
}

void Watchdog::enable() {
  WatchdogResetEnable(WATCHDOG0_BASE);
}

void Watchdog::feed() {
  if (_running) {
//    Serial.print("feeding the watchdog at: ");
//    Serial.println(WatchdogReloadGet(WATCHDOG0_BASE) - WatchdogValueGet(WATCHDOG0_BASE));

// not using the interrupt.
//    WatchdogIntClear(WATCHDOG0_BASE);
    WatchdogReloadSet(WATCHDOG0_BASE, _period);
  }
}

void Watchdog::stop() {
  _running = false;
}

boolean Watchdog::was_reset() {
  return _was_wdog_reset;
}


