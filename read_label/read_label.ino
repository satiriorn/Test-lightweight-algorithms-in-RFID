#include <SPI.h>
#include <PN532_SPI.h>
#include <PN532.h>
#include <NfcAdapter.h>
#include <AESLib.h>
#include <LiquidCrystal_I2C.h>

PN532_SPI interface(SPI, 10); // create a PN532 SPI interface with the SPI CS terminal located at digital pin 10
NfcAdapter nfc = NfcAdapter(interface); // create an NFC adapter object
char data[32];
const uint8_t key[] = {0x03, 0x81, 0xaa, 0x26, 0x88, 0x66, 0x1d, 0x15,
0x53, 0xe1, 0x55, 0x20, 0x67, 0x5b, 0xbf, 0xd2,
0xfe, 0x79, 0xdb, 0x44, 0x35, 0x49, 0x5a, 0xab,
0xc3, 0x4a, 0x4b, 0xc6, 0x2e, 0xb9, 0xc0, 0xcc};//256 bit key
LiquidCrystal_I2C lcd(0x27,16,2); 

uint8_t ndefBuf[120];

void setup(void) {
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
            
            aes256_dec_single(key, data);
            if(i>0){
              lcd.setCursor(0,1);
              }
            lcd.print(data);
        }
      delay(2000);
      lcd.clear();
      lcd.print("Enter key");
   }
  delay(2000);
  
}
