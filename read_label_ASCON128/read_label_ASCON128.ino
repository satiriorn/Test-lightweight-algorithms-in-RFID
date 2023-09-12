#include <SPI.h>
#include <PN532_SPI.h>
#include <PN532.h>
#include <NfcAdapter.h>
#include <CryptoLW.h>
#include <Crypto.h>
#include <Ascon128.h>
#include <string.h>
#include <LiquidCrystal_I2C.h>

uint8_t correct_message = 0;
PN532_SPI interface(SPI, 10); // create a PN532 SPI interface with the SPI CS terminal located at digital pin 10
NfcAdapter nfc = NfcAdapter(interface); // create an NFC adapter object
char data[16];

Ascon128 ascon; //Initialises Ascon module
uint8_t key[16] = {0x72, 0xfd, 0x18, 0xde, 0xbd, 0xee, 0x86, 0x13,
                    0x4f, 0x7c, 0x44, 0x29, 0x84, 0x37, 0x56, 0x06};
                    
uint8_t iv[] = {0x91, 0x5f, 0xf8, 0xff, 0xca, 0xd8, 0xae, 0x1d,
                    0xf4, 0x45, 0xeb, 0x03, 0xe2, 0x18, 0xfd, 0x25};
                 
uint8_t authdata[] = {0x61, 0x73, 0x73, 0x64, 0x61, 0x74, 0x31, 0x32};
uint8_t tag[] = {0x16, 0x69, 0x74, 0xbf, 0xbd, 0x43, 0xd7, 0xa8,
                    0xfe, 0x43, 0xf0, 0xce, 0xe2, 0xdd, 0xb9, 0xf8};
              
LiquidCrystal_I2C lcd(0x27,16,2); 

char cipherdata[2][17] = {"D4eR1vG7wX9zY2sL", "X5fG2hJ7kL9mN3pQ"};

uint8_t ndefBuf[120];

String decryptData (uint8_t *cipherData, int sizeOfArray){
    //ascon.clear();
    unsigned long StartTime = micros();
    uint8_t buffer1[32];
    ascon.setKey((uint8_t *)key,sizeof(key)); 
    ascon.setIV(iv,sizeof(iv));
    ascon.addAuthData(authdata,sizeof(authdata));
    ascon.decrypt(buffer1,cipherData,sizeOfArray);
    unsigned long CurrentTime = micros();
    unsigned long ElapsedTime = CurrentTime - StartTime;
    Serial.print("Time taken for Decryption: ");
    Serial.print(ElapsedTime);
    Serial.print("us");
    Serial.println();
    //ascon.clear();
    return (char *)buffer1;
}

void setup(void) {
  Serial.begin(9600);
  nfc.begin();
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0,0);
  lcd.print("Enter key");
}

void loop() {
  readNFC();
}

void readNFC() {
  if (nfc.tagPresent())
  {
    lcd.clear();
    NfcTag tag = nfc.read();
    tag.print();
    NdefMessage message = tag.getNdefMessage();
    int recordCount = message.getRecordCount();
    unsigned long Time = micros();
    for (uint8_t i = 0; i < recordCount; i++)
      {
          NdefRecord record = message.getRecord(i);
          uint8_t payloadLength = record.getPayloadLength();
          char payload[payloadLength];
          record.getPayload(payload);
          int c = (record.getType()=="U")? 1 : 3;
          for (uint8_t x = 0; c < payloadLength; c++)
          {
              data[x] = payload[c];
              x++;
          }
          Serial.println(data);
          String decryptedText = decryptData(data, sizeof(data)); //Decrypts the Byte Array to a String format
          Serial.println(decryptedText);
          /*
          if(data==cipherdata[i])
          {
            correct_message+=50;
            lcd.print("value:");
            lcd.print(correct_message+1);
            if(correct_message>=99){
                lcd.clear();
                lcd.print("Gate open");
                delay(2000);
              }
            else{
                lcd.clear();
                lcd.print("Key incorrect");
              }
            
          }
            */
            
      }
      Time = micros()-Time;
      Serial.print("Time decryption: ");
      Serial.print(Time);Serial.print("us");
      Serial.println();
      delay(2000);
      lcd.clear();
      lcd.print("Enter key");
   }
  delay(2000);
  
}
