#include <Wire.h>
#include <Adafruit_NFCShield_I2C.h>

#define IRQ             2
#define RESET           3  // Not connected by default on the NFC Shield
#define VALID_ID_COUNT  2

const int greenLedPin            =    7;  //  LOW is on
const int redLedPin              =   12;  //  LOW is on
const int lockPin                =    8;  //  HIGH is open
const int timeOpenInMs           = 5000;
const int resetTimeOnBadReadInMs = 3000;


typedef struct
{
  uint8_t id[7];
} RingId;


//  Replace with the ids of your nfc tags
RingId validIds[VALID_ID_COUNT] = {
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01 }
};


Adafruit_NFCShield_I2C nfc(IRQ, RESET);

void setup(void) {
  nfc.begin();

  pinMode(greenLedPin, OUTPUT);
  pinMode(redLedPin, OUTPUT);
  pinMode(lockPin, OUTPUT);

  digitalWrite(greenLedPin, HIGH);   //  Green LED
  digitalWrite(redLedPin, HIGH);  //  Red LED
  digitalWrite(lockPin, LOW);    //  Door Lock

  uint32_t versiondata = nfc.getFirmwareVersion();
  if (! versiondata) {
    while (1); // halt
  }

  // configure board to read RFID tags
  nfc.SAMConfig();
}


void loop(void) {
  uint8_t success;
  uint8_t uid[] = { 0, 0, 0, 0, 0, 0, 0 };  // Buffer to store the UID
  uint8_t uidLength;                        // Length of the UID
  bool found = false;

  // Wait for an ISO14443A type cards (Mifare, etc.).  When one is found
  // 'uid' will be populated with the UID, and uidLength will indicate
  // if the uid is 4 bytes (Mifare Classic) or 7 bytes (Mifare Ultralight)
  success = nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength);
  
  if (success) {
    found = false;
    for (int i = 0; i < VALID_ID_COUNT; i++) {
      if (memcmp(validIds[i].id, uid, 7) == 0) {
        found = true;
        break;
      }
    }

    if (found) {
      digitalWrite(greenLedPin, LOW);   // turn the green LED on
      digitalWrite(lockPin, HIGH);  // open the door lock
      delay(timeOpenInMs);
      digitalWrite(lockPin, LOW);   // close the door lock
      digitalWrite(greenLedPin, HIGH);  // turn the green LED off
    } else {
      digitalWrite(redLedPin, LOW);  // turn the red LED on
      delay(resetTimeOnBadReadInMs);
      digitalWrite(redLedPin, HIGH); // turn the red LED off
    }
  }
}

