/*
 * This is variant of original work originally implemented as:
 * https://code.google.com/archive/p/tinkerit/ https://github.com/Cathedrow/TrueRandom
 * Copyright (c) 2010 Peter Knight, Tinker.it! All rights reserved.
 * as can be seen at https://github.com/marvinroger/ESP8266TrueRandom
 * 
 * Specially modified for SHIELD. All rights reserved to original authors.
 */
#include "SHIELDprng.h"

ICACHE_FLASH_ATTR SHIELDprng::SHIELDprng() {
	lastYield = 0;
}

ICACHE_FLASH_ATTR int SHIELDprng::generatePRN(void) {
  // Needed to keep wifi stack running smoothly and to avoid wdt reset
  if (lastYield == 0 || millis() - lastYield >= 50) {
    yield();
    lastYield = millis();
  }
  uint8_t bit = (int)RANDOM_REG32;  //This uses the onboard Random Number Generator (esp8266_peri.h)

  return bit & 1;
}

ICACHE_FLASH_ATTR int SHIELDprng::randomBit(void) {
  /* 
   * Software whitening bits using Von Neuman Algorithm which fixes simple bias and reduce correlation
   * 
   * VNA considers two bits at a time (non-overlapping), taking one of three actions:
   *  - when two successive bits are equal, they are discarded;
   *  - a sequence of 1,0 becomes a 1;
   *  - and a sequence of 0,1 becomes a zero.
   * It thus represents a falling edge with a 1, and a rising edge with a 0.
   * 
   * See:
   * https://dornsifecms.usc.edu/assets/sites/520/docs/VonNeumann-ams12p36-38.pdf
   * https://en.wikipedia.org/wiki/Hardware_random_number_generator#Software_whitening
   */
  for(;;) { /* will loop forever until a value is returned, does not execute the last return*/
    int a = generatePRN() | generatePRN() << 1;
    if (a==1) return 0; // 1 to 0 transition: log a zero bit
    if (a==2) return 1; // 0 to 1 transition: log a one bit
    // For other cases, try again.
  }

  return 0;
}

ICACHE_FLASH_ATTR char SHIELDprng::randomByte(void) {
  char result = 0;
  for (uint8_t i = 8; i--;)
    result += result + randomBit();
  return result;
}

ICACHE_FLASH_ATTR void SHIELDprng::uuid(uint8_t* uuidLocation) {
  /* 
   * Generate a Version 4 (random data) UUID according to RFC4122.
   * With probability to have collission of 1:1 billion within 103 trillion version-4 UUIDs.
   * 
   * Note:
   * Although the UUID contains 128 bits, only 122 of those are random.
   * The other 6 bits are fixed, to indicate a version number - which is 4.
   * 
   * The UUID version M, is represented by the 13th digit of a hexadecimal UUID string
   * The variant V is represented in the 17th digit.
   * 175bf621-7da7-4240-9b18-04767213ab98
   * xxxxxxxx -xxxx-Мххх-Vxxx-xxxxxxxxxxxx
   * 
   * See:
   * https://en.wikipedia.org/wiki/Universally_unique_identifier#Version_4_(random)
   * https://www.uuidtools.com/decode
   * https://sci-hub.ru/10.1137/1033051 
   */

  //Fills the array with random bytes
  char* location = (char*)uuidLocation;
  int size = 16;
  for (;size--;) *location++ = randomByte();

  //Indicate UUID version and variant
  uuidLocation[6] = 0x40 | (0x0F & uuidLocation[6]);
  uuidLocation[8] = 0x80 | (0x3F & uuidLocation[8]);
}

ICACHE_FLASH_ATTR String SHIELDprng::uuidToString(uint8_t* uuidLocation) {
  /* 
   * UUIDs are written in base 16 (hexadecimal) which uses numbers 0-9 and characters a-f.
   * There is no distinction between upper and lowercase letters.
   * However, RCF 4122 section 3 requires that UUID generators output in lowercase and
   * systems accept UUIDs in upper and lowercase.
   * 
   * See:
   * https://datatracker.ietf.org/doc/html/rfc4122#section-3
   * https://www.uuidtools.com/uuid-versions-explained
   */
  String string = "";
  for (int index = 0; index < 16; index++) {
    if (index == 4) string += "-";
    if (index == 6) string += "-";
    if (index == 8) string += "-";
    if (index == 10) string += "-";
    int topDigit = uuidLocation[index] >> 4;
    int bottomDigit = uuidLocation[index] & 0x0f;
    string += "0123456789abcdef"[topDigit];     // High Hex digit
    string += "0123456789abcdef"[bottomDigit];  // Low Hex digit
  }
  
  return string;
}

SHIELDprng prng;
