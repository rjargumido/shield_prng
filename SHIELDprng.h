/*
 * This is variant of original work originally implemented as:
 * https://code.google.com/archive/p/tinkerit/ https://github.com/Cathedrow/TrueRandom
 * Copyright (c) 2010 Peter Knight, Tinker.it! All rights reserved.
 * as can be seen at https://github.com/marvinroger/ESP8266TrueRandom
 * 
 * Specially modified for SHIELD. All rights reserved to original authors.
 */
#ifndef SHIELDprng_h
#define SHIELDprng_h

#include <Arduino.h>
#include <inttypes.h>

class SHIELDprng
{
  public:
    ICACHE_FLASH_ATTR SHIELDprng();	
    ICACHE_FLASH_ATTR void uuid(uint8_t* uuidLocation);
    ICACHE_FLASH_ATTR String uuidToString(uint8_t* uuidLocation);

  private:
    unsigned long lastYield;
    ICACHE_FLASH_ATTR int generatePRN(void);
    ICACHE_FLASH_ATTR int randomBit(void);
    ICACHE_FLASH_ATTR char randomByte(void);
};
extern SHIELDprng prng;
#endif
