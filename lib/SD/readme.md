##SD Library for LaunchPad LM4F / TM4C


* Adaptation for LaunchPad LM4F / TM4C

Rei Vilo - May 20, 2014

* SD - a slightly more friendly wrapper for sdfatlib

(C) Copyright 2010 SparkFun Electronics

* Arduino SdFat Library

Copyright (C) 2009 by William Greiman

----

### Requirements

SD-card in SPI mode requires a 512 bytes buffer for read-write operations. 

Energia 12 

Boards

* LaunchPad Stellaris LM4F120 — *tested*
* LaunchPad Tiva C Series TM4C123 — *tested*
* LaunchPad Connected TM4C129 — *not tested*

Other candidates

* LaunchPad MSP430F5529 — *not tested*
* LaunchPad MSP430FR5969 — *not tested*

----

### Initialise the SD-card

Parameters

* `chipSelectPin` pin number for SD-card chip select
* `sckRateID` SPI speed, optional, default=SPI_HALF_SPEED
* `SPI_Port` SPI port number, optional, default=-1=none
* `cardDetectionPin` card detection pin number, optional, default=-1=none
* `level` expected level when SD-card available, default=LOW

Result

* true is success, false otherwise

```
    boolean begin(uint8_t chipSelectPin,
                  uint8_t sckRateID = SPI_HALF_SPEED,
                  int8_t SPI_Port = -1,
                  int8_t cardDetectionPin = -1,
                  int8_t level = LOW);
    
```

### SD-card hardware detection

Result

* true is SD-card inserted, false otherwise

Uses previously defined cardDetectionPin and level

```
    uint8_t detectSD();
```

### Licence

Creative Commons [Attribution-NonCommercial-ShareAlike 4.0 International License](http://creativecommons.org/licenses/by-nc-sa/4.0/)