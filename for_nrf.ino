/* Передает положение потенциометра через NRF24L01 с помощью Arduino
 * 
 *  Соединение выводов
 *  CE   - A2
    MISO - 12
    MOSI - 11
    SCK  - 13
    CS   - 8
      //POT  - A7
*/

#include <SPI.h>
#include "RF24.h"

RF24 myRadio (A2, 8);

struct package
{
  int msg = 0;
};

byte addresses[][6] = {"0"};

typedef struct package Package;
Package data;

void setup()
{
  Serial.begin(9600);
  myRadio.begin();  
  myRadio.setChannel(115);           // Диапазон 115 находится выше WIFI
  myRadio.setPALevel(RF24_PA_MIN);   // минимальная выходная мощность
  myRadio.setDataRate(RF24_250KBPS); // Минимальная скорость
  delay(500);
  Serial.print("Setup Initialized");
}

void loop()
{
  
  if (servo_value>1)
    data.msg = servo_value;
  WriteData();
  delay(50);
  // ReadData(); 
  //delay(200);
}

void WriteData()
{
  myRadio.stopListening();                // Остановить прием и начать передачу
  myRadio.openWritingPipe( 0xF0F0F0F0AA); // Отправить данные на этот 40-разрядный адрес
  myRadio.write(&data, sizeof(data)); 
  Serial.print("\nSent:");
  Serial.println(data.msg);
  delay(300);
}

void ReadData()
{ 
  myRadio.openReadingPipe(1, 0xF0F0F0F066); // Какой канал считывать, 40-разрядный адрес
  myRadio.startListening();                 // Остановить передачу и начать прием
  if (myRadio.available()) 
  {
    while (myRadio.available())
    {
      myRadio.read(&data, sizeof(data));
    }
    Serial.print("\nReceived:");
    Serial.println(data.msg);
  }
}
