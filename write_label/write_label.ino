#include <SPI.h>
#include <PN532_SPI.h>
#include <PN532.h>
#include <NfcAdapter.h>
#include <AESLib.h>

PN532_SPI pn532spi(SPI, 10);
NfcAdapter nfc = NfcAdapter(pn532spi);
char data[32];
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
        NdefMessage message = NdefMessage();
        strcpy(data, "Tag Write");
        aes256_enc_single(key, data);
        message.addTextRecord(data); // Text Message you want to Record
        strcpy(data,"https://dl.nure");
        aes256_enc_single(key, data);
        message.addUriRecord(data); // Website you want to Record
        strcpy(data,"Go to this site");
        aes256_enc_single(key, data);
        message.addTextRecord(data);  // Ednding Message for you to Record
        boolean success = nfc.write(message); 
        if (success) {
            Serial.println("Success write!"); // if it works you will see this message 
        } else {
            Serial.println("Write failed"); // If the the rewrite failed you will see this message
        }
    }
    delay(5000);
}
