#include <SPI.h>
#include <PN532_SPI.h>
#include <PN532.h>
#include <NfcAdapter.h>
#include <AESLib.h>

PN532_SPI pn532spi(SPI, 10);
NfcAdapter nfc = NfcAdapter(pn532spi);
char data[2][17] = {"D4eR1vG7wX9zY2sL", "X5fG2hJ7kL9mN3pQ"};
const uint8_t key[] = {0x03, 0x81, 0xaa, 0x26, 0x88, 0x66, 0x1d, 0x15,
0x53, 0xe1, 0x55, 0x20, 0x67, 0x5b, 0xbf, 0xd2,
0xfe, 0x79, 0xdb, 0x44, 0x35, 0x49, 0x5a, 0xab,
0xc3, 0x4a, 0x4b, 0xc6, 0x2e, 0xb9, 0xc0, 0xcc};
    
void setup(void) {
    Serial.begin(9600);
    Serial.println("NDEF Formatter");
    nfc.begin();
}

void loop(void) {
    
    Serial.println("\nPlace an unformatted Mifare Classic tag on the reader.");
    if (nfc.tagPresent()) { 
        unsigned long StartTime = micros();
        NdefMessage message = NdefMessage();
        unsigned long StartTimeEncryption = micros();
        aes256_enc_single(key, data[0]);
        unsigned long CurrentTime = micros();
        unsigned long ElapsedTime = CurrentTime - StartTimeEncryption;
        message.addTextRecord(data[0]); // Text Message you want to Record
        StartTimeEncryption = micros();
        aes256_enc_single(key, data[1]);
        CurrentTime = micros();
        ElapsedTime+=CurrentTime - StartTimeEncryption;
        Serial.print("Time encryption: ");
        Serial.print(ElapsedTime);Serial.print("us");
        Serial.println();
        message.addTextRecord(data[1]);  // Ednding Message for you to Record
        boolean success = nfc.write(message); 
        CurrentTime = micros();
        ElapsedTime = CurrentTime - StartTime;
        Serial.print("Time taken for Encryption and write: ");
        Serial.print(ElapsedTime);Serial.print("us");
        Serial.println();
        if (success) {
            Serial.println("Success write!"); // if it works you will see this message 
        } else {
            Serial.println("Write failed"); // If the the rewrite failed you will see this message
        }
    }
    delay(5000);
}
