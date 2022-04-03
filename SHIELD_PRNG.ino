#include <AESLib.h>
#include "SHIELDprng.h"

#define MAX_BLOCKS  16

AESLib aesLib;

byte PID[MAX_BLOCKS];         // Profile Identifier - AES Plain Text
byte CIRCADIAN[MAX_BLOCKS];   // Circadian - AES Encryption Key
byte PUK[MAX_BLOCKS];         // Profile Unlocking Key - AES Nonce/IV

String createCircadian() {
  // Generate a new Circadian
  prng.uuid(CIRCADIAN);
  
  return prng.uuidToString(CIRCADIAN);
}

String createPUK() {
  // Generate a new Profile Unlocking Key (PUK)
  prng.uuid(PUK);
  
  return prng.uuidToString(PUK);
}

String createPID() {
  // Generate a new PID
  prng.uuid(PID);

  return prng.uuidToString(PID);
}

String getProfile(String pid, const byte* circadian, byte* nonce) {
  uint16_t len = pid.length();  // 36 characters
  int cipherlength = aesLib.get_cipher64_length(len); // 88 characters
  
  char encrypted[cipherlength]; //Generated Profile
  char cleartext[len];  //PID as the raw data
  sprintf(cleartext, "%s", pid.c_str());  //Converts the String PID to char array
  
  aesLib.encrypt64(cleartext, len, encrypted, circadian, MAX_BLOCKS, nonce);

  return String(encrypted);
}

String decryptProfile(String profile, const byte* circadian, byte nonce[]) {
  char _ciphertext[512];
  uint16_t dlen = profile.length();

  sprintf(_ciphertext, "%s", profile.c_str());
  char decrypted[dlen];
  aesLib.decrypt64(_ciphertext, dlen, decrypted, circadian, MAX_BLOCKS, nonce);

  return String(decrypted);
}

void setup() {
  Serial.begin(115200);
  while (!Serial);

  Serial.println();
  Serial.println();

  Serial.println("Circadian (Key):\t" + createCircadian());   //Creates the Circadian Key every 24 Hrs

  // Padding is used in a block cipher where we fill up the blocks with padding bytes
  // Since we use AES in CBC Mode (Cipher block chaining).
  // We uses CMS (Cryptographic Message Syntax) padding mode here.
  // This pads with the same value as the number of padding bytes.
  // Defined in RFC 5652, PKCS#5, PKCS#7 (X.509 certificate) and RFC 1423 PEM.
  // See: https://asecuritysite.com/symmetric/padding
  aesLib.set_paddingmode(paddingMode::CMS);

  //Creates the PID
  String curr_pid = createPID();
  Serial.println("PID (Cleartext):\t" + curr_pid);

  //Creates the Profile Unlocking Key (PUK)
  String puk = createPUK();
  Serial.println("PUK (IV/Nonce):\t" + puk);

  //Duplicates original nonce
  byte orig_nonce[MAX_BLOCKS];
  for (int i = 0; i < MAX_BLOCKS; i++) {
      orig_nonce[i] = PUK[i];
  }  
  //String dup = prng.uuidToString(orig_nonce);
  //Serial.println("Duplicated Nonce:\t" + dup);

  //Creates the Profile out of the PID, PUK, and Circadian
  String x = "z+PNihQWY/N4HoY3Gmy/yMo24fbjSTPja0QT3t3j2YgK6Hnx8fAehIZjwWUfDytLpUWjDSssAsAjtpt9zEoLHg==";
  String profile = getProfile(curr_pid, CIRCADIAN, PUK);
  Serial.println("Profile:\t" + profile);
  
  // Decrypt the Profile using the Nonce
  String decrypted = decryptProfile(profile, CIRCADIAN, orig_nonce);

  char cleartext[256];
  sprintf(cleartext, "%s", curr_pid.c_str());

  Serial.println("\nStatus:");
  Serial.println("====================================================");
  Serial.println("CIRRUS PID:\t" + decrypted);
  Serial.println("Local PID:\t" + String(cleartext));
  Serial.println("====================================================");
  
  if (decrypted.equals(cleartext))
    Serial.println("Decryption successful.");
  else
    Serial.println("Decryption failed.");
}

int i = 1;
bool _stop = true;
void loop() {
  /*
  while(_stop) {
    for(; i < 10001; i++) {
      String x = createPID();
      Serial.println(x);
      delay(10);
    }
    _stop = false;
    Serial.println(i);
  }
  */
}
