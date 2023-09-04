#include <SPI.h>
#include <PN532_SPI.h>
#include <PN532.h>
#include <NfcAdapter.h>
#include <AESLib.h>
#include <LiquidCrystal_I2C.h>

uint8_t correct_message = 0;
PN532_SPI interface(SPI, 10); // create a PN532 SPI interface with the SPI CS terminal located at digital pin 10
NfcAdapter nfc = NfcAdapter(interface); // create an NFC adapter object
char result_hash[2][64] = {"efad2e38f54056f74641a5fd3f4e9be50016c6140d5985da33f6eb023c11f668", "a53070313269586d21fb34f97e261cd15dd7b6bf25be708029a6f69440a049f4"};
char data[64];
LiquidCrystal_I2C lcd(0x27,16,2); 

uint8_t ndefBuf[120];

void setup(void) {
  nfc.begin();
  Serial.begin(9600);
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
            Serial.println(data);
            if(data==result_hash[i])
            {
              correct_message+=50;
              lcd.print("value:");
              lcd.print(correct_message+1);
              if(correct_message>=99){
                  lcd.clear();
                  lcd.print("Gate open");
                  delay(2000);
                }
              
            }
        }
      delay(2000);
      lcd.clear();
      lcd.print("Enter key");
   }
  delay(2000);
  
}
