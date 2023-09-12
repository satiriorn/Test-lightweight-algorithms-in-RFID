#include <CryptoLW.h>
#include <Crypto.h>
#include <Ascon128.h>
#include <SPI.h>
#include <PN532_SPI.h>
#include <PN532.h>
#include <NfcAdapter.h>

PN532_SPI pn532spi(SPI, 10);
NfcAdapter nfc = NfcAdapter(pn532spi);

Ascon128 ascon; //Initialises Ascon module
uint8_t key[16] = {0x72, 0xfd, 0x18, 0xde, 0xbd, 0xee, 0x86, 0x13,
                    0x4f, 0x7c, 0x44, 0x29, 0x84, 0x37, 0x56, 0x06};
                    
uint8_t iv[] = {0x91, 0x5f, 0xf8, 0xff, 0xca, 0xd8, 0xae, 0x1d,
                    0xf4, 0x45, 0xeb, 0x03, 0xe2, 0x18, 0xfd, 0x25};
                 
uint8_t authdata[] = {0x61, 0x73, 0x73, 0x64, 0x61, 0x74, 0x31, 0x32};
uint8_t tag[] = {0x16, 0x69, 0x74, 0xbf, 0xbd, 0x43, 0xd7, 0xa8,
                    0xfe, 0x43, 0xf0, 0xce, 0xe2, 0xdd, 0xb9, 0xf8};
              
char cipherdata[2][17] = {"D4eR1vG7wX9zY2sL", "X5fG2hJ7kL9mN3pQ"};
uint8_t count_data = 2;

void encryptData (){
    //ascon.clear();
    unsigned long Time = micros();
    ascon.setKey((uint8_t *)key,sizeof(key)); 
    ascon.setIV(iv,sizeof(iv));
    ascon.addAuthData(authdata,sizeof(authdata));
    ascon.encrypt(cipherdata[0],(uint8_t *)cipherdata[0], 16);
    ascon.encrypt(cipherdata[1],(uint8_t *)cipherdata[1], 16);
    ascon.computeTag(tag,sizeof(tag));
    Time = micros() - Time;
    Serial.print("Time taken for Encryption: ");
    Serial.print(Time);
    Serial.print("us");
    Serial.println();
    //ascon.clear();
  
}


void setup()
{
    Serial.begin(9600);
    delay(200);
    nfc.begin();

    Serial.println();
    encryptData();
    Serial.println("The Cipher text in string:");
    for(int j = 0; j<count_data;j++){
      Serial.print(cipherdata[j]);
      Serial.println();
    }
    
}

void loop()
{
  if (nfc.tagPresent()) {
        unsigned long Time = micros();
        NdefMessage message = NdefMessage();
        for (int i = 0; i<count_data; i++){
            message.addTextRecord(cipherdata[i]);
          }
          boolean success = nfc.write(message); 
        Time = micros() - Time;
        Serial.print("Time taken for Encryption: ");
        Serial.print(Time);
        Serial.print("us");
        Serial.println();
        if (success) {
            Serial.println("Success write!"); // if it works you will see this message 
        } else {
            Serial.println("Write failed"); // If the the rewrite failed you will see this message
        }
  }
  delay(2000);
}
