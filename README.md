# SHIELDprng
A UUID Generator library specially made for SHIELD on ESP8266.

**This is variant of original work originally implemented as: [tinkerit TrueRandom](https://code.google.com/archive/p/tinkerit/wikis/TrueRandom.wiki) and slightly modified by [marvinroger](https://github.com/marvinroger/ESP8266TrueRandom). The *uuid* and *uuidtostring* functions are the only used functions in this library.**

## Introduction

This library generates a "true" random numbers on ESP8266, but are not ensured to be cryptographically secure. They are different every time you start your program, and are truly unpredictable unlike the default Arduino random() function.

You get a random number. Really random. Different every time you restart.

## How the PRNG works
Randomization achieves random numbers by reading the ESP8266 internal hardware random number generator register. However, is not noisy enough so a [von Neumann whitening algorithm](http://en.wikipedia.org/wiki/Hardware_random_number_generator) gathers enough entropy from multiple readings to ensure a fair distribution of 1s and 0s.

## Notes

It uses the ESP8266 internal hardware random number generator register ```RANDOM_REG32``` to generate true random bits. Byte generation uses the ```generatePRN()``` function to generate true random bits then Software whitening bits using Von Neuman Algorithm.


## SHIELDprng functions

### ESP8266TrueRandom.uuid(address)

UUIDs are unique identifiers. They are 16 bytes (128 bits) long, which means that generating them randomly This generates a random UUID, and writes it to an array. UUIDs are globally unique numbers that are often used in web services and production electronics. ESP8266TrueRandom can produce any one of 5,316,911,983,139,663,491,615,228,241,121,378,304 different numbers. You're more likely to win top prize in the national lottery 3 times in a row than get two matching UUIDs.

### ESP8266TrueRandom.uuidToString(address)

Returns a String containing the string representation of the given UUID.
