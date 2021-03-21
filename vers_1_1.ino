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
#define LED_RED    A3              // Порт 13, красный светодиод
#define LED_YELLOW A4             // Порт 12, желтый светодиод
#define LED_GREEN  A5              // Порт 11, зеленый светодиод

#define RED_1 7
#define YELLOW_1 6
#define GREEN_1 5

#define P_R 3
#define P_Y A1
#define P_G A0

const int echoPin = 9;                 //Порт 9, echoPin
const int trigPin = 10;                //Порт 10, trigPin
#define INTERRUPT_PIN 2
#define TEST_LED 4
const int TIMEOUT_RED = 3000;          // Время горения красного сетодиода
const int TIMEOUT_YEL = 1690;          // Время горения желтого светодиода
const int TIMEOUT_GREEN = 2000;        // Время горения зеленого светодиода

const int TIMEOUT_FLASH_GREEN = 500;      // Время мигания зеленого светодиода

int state_now = 0;

#define GO 0
#define GO_END 1
#define YELLOW 2
#define STOP 3
#define READY 4
#define EMERGENCY 5

#include <SPI.h>  
#include "RF24.h" 


RF24 myRadio (7, 8); 

struct package
{
  int msg;
};
typedef struct package Package;
Package data;

byte addresses[][6] = {"0"}; 

void setup() 
{
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  // Все порты светодиодов будут у нас установлены в режим "внешняя нагрузка", OUTPUT
  pinMode(LED_RED, OUTPUT);
  pinMode(LED_YELLOW, OUTPUT);
  pinMode(LED_GREEN, OUTPUT);
  //
  pinMode(RED_1, OUTPUT);
  pinMode(YELLOW_1, OUTPUT);
  pinMode(GREEN_1, OUTPUT);
  //
  pinMode(P_R, OUTPUT);
  pinMode(P_Y, OUTPUT);
  pinMode(P_G, OUTPUT);
  //
  pinMode(TEST_LED, OUTPUT);
  // Устанавливаем начальное значение светодиодов
  digitalWrite(TEST_LED, LOW);
  digitalWrite(LED_RED, LOW);
  digitalWrite(LED_YELLOW, LOW);
  digitalWrite(LED_GREEN, LOW);
  //
  digitalWrite(P_R, LOW);
  digitalWrite(P_Y, LOW);
  digitalWrite(P_G, LOW);
  //
  digitalWrite(RED_1, LOW);
  digitalWrite(YELLOW_1, LOW);
  digitalWrite(GREEN_1, LOW);
  // INTERRUPT
  Serial.begin(9600);
  myRadio.begin();
  myRadio.setChannel(115);           // Диапазон 115 находится выше WIFI
  myRadio.setPALevel(RF24_PA_MIN);   // минимальная выходная мощность
  myRadio.setDataRate(RF24_250KBPS); // Минимальная скорость
  
  Serial.print("Setup Initialized");
  delay(500);
  attachInterrupt(0, interr_func, LOW);
}

volatile uint32_t debounce;
volatile int FLAG = 0;
void interr_func()
{
  if (millis() - debounce >= 100 && digitalRead(2)) {
    debounce = millis();
    FLAG = 1;
  }
}
void emergency_mode()
{
  digitalWrite(TEST_LED, HIGH);
  
  digitalWrite(LED_RED, LOW);
  digitalWrite(LED_YELLOW, HIGH);
  digitalWrite(LED_GREEN, LOW);

  digitalWrite(RED_1, LOW);
  digitalWrite(YELLOW_1, HIGH);
  digitalWrite(GREEN_1, LOW);

  digitalWrite(P_R, LOW);
  digitalWrite(P_Y, HIGH);
  digitalWrite(P_G, LOW);
  
  delay(1000);

  digitalWrite(P_Y, LOW);
  digitalWrite(YELLOW_1, LOW);
  digitalWrite(LED_YELLOW, LOW);  

  digitalWrite(LED_RED, HIGH);
  digitalWrite(RED_1, HIGH);
  digitalWrite(P_R, HIGH);
  

  while(distance() >= 15\
  )
  {
    blink_dist(distance());
  }
  delay(5000);
  digitalWrite(TEST_LED, LOW);
  state_now = YELLOW;
  FLAG = 0;
}


void loop()  
{
  switch(state_now)
  {
    case GO:
      digitalWrite(LED_YELLOW, LOW);
      digitalWrite(LED_RED, LOW);
      digitalWrite(LED_GREEN, HIGH);
      //
      digitalWrite(P_R, HIGH);
      digitalWrite(P_Y, LOW);
      digitalWrite(P_G, LOW);
      //
      digitalWrite(RED_1, HIGH);
      digitalWrite(YELLOW_1, LOW);
      digitalWrite(GREEN_1, LOW);
      if(my_delay(TIMEOUT_GREEN)==1) // Ждем
      {
        state_now = READY;
        break;
      }
      state_now = GO_END;
      
      break;
      
    case GO_END: 
      for (int i = 0; i < 3; i++)
      {
        digitalWrite(LED_GREEN, LOW);
        
        if(my_delay(TIMEOUT_FLASH_GREEN)==1)
        {
        state_now = READY;
        break;
        }
        digitalWrite(LED_GREEN, HIGH);
        if(my_delay(TIMEOUT_FLASH_GREEN)==1)
        {
        state_now = READY;
        break;
        }
        
        digitalWrite(RED_1, HIGH);
        digitalWrite(YELLOW_1, LOW);
        digitalWrite(GREEN_1, LOW);
        //
        digitalWrite(P_R, HIGH);
        digitalWrite(P_Y, LOW);
        digitalWrite(P_G, LOW);
      }
      state_now = YELLOW;
      break;
      
    case YELLOW:
      digitalWrite(LED_GREEN, LOW);
      digitalWrite(LED_YELLOW, HIGH);
      //
      digitalWrite(RED_1, LOW);
      digitalWrite(YELLOW_1, HIGH);
      digitalWrite(GREEN_1, LOW);
      //
      digitalWrite(P_R, LOW);
      digitalWrite(P_Y, HIGH);
      digitalWrite(P_G, LOW);
      //
      if(my_delay(TIMEOUT_YEL)==1)
      {
        state_now = READY;
        break;
      }

      state_now = STOP;
      break;
      
    case STOP:
      digitalWrite(LED_YELLOW, LOW);
      digitalWrite(LED_RED, HIGH);
      //
      digitalWrite(RED_1, LOW);
      digitalWrite(YELLOW_1, LOW);
      digitalWrite(GREEN_1, HIGH);
      //
      if(my_delay(750)==1) // Ждем
      {
        state_now = READY;
        break;
      }

      digitalWrite(P_R, LOW);
      digitalWrite(P_Y, LOW);
      digitalWrite(P_G, HIGH);
      
      if(my_delay(TIMEOUT_GREEN-750)==1) // Ждем
      {
        state_now = READY;
        break;
      }
      for (int i = 0; i < 3; i++)
      {
        digitalWrite(GREEN_1, LOW);
        digitalWrite(P_G, LOW);
        if(my_delay(TIMEOUT_FLASH_GREEN)==1)
        {
        state_now = READY;
        break;
        }
        digitalWrite(GREEN_1, HIGH);
        digitalWrite(P_G, HIGH);
        if(my_delay(TIMEOUT_FLASH_GREEN)==1)
        {
        state_now = READY;
        break;
        }
        
        
      }
      state_now = READY;
      break;
      
    case READY:
        
        digitalWrite(LED_YELLOW, HIGH);
        
        digitalWrite(YELLOW_1, HIGH);
        digitalWrite(GREEN_1, LOW);
        digitalWrite(P_R, LOW);
        digitalWrite(P_Y, HIGH);
        digitalWrite(P_G, LOW);
        if(my_delay(TIMEOUT_YEL)==1)
        {
          state_now = READY;
          break;
        }
      
      
      state_now = GO;
      break;
      
    default:
      break;
  }
}
int my_delay(int t)
{
  int a = 0;
  while(a != t)
  {
    delay(1);
    a++;
    ReadData();
    if(FLAG==1)
    {
      emergency_mode();
      return 1;
    }
  }
}
int distance()
{
  int duration, cm;
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  duration = pulseIn(echoPin, HIGH);
  cm = duration / 58;
  Serial.print(cm);
  Serial.println(" cm");
  delay(100);
  return cm;
}
void ReadData()
{
  myRadio.openReadingPipe(1, 0xF0F0F0F0AA); // Какой канал считывать, 40-разрядный адрес
  myRadio.startListening();                 // Остановить передачу и начать прием

  if ( myRadio.available()) 
  {
    while (myRadio.available())
    {
      myRadio.read(&data, sizeof(data));
    }
    Serial.print("\nReceived:");
    Serial.println(data.msg);
  }
}
void blink_dist(int duration)
{
  if(duration >=40) duration = 40;
  int f = map(duration, 15, 40, 15, 5);
  int t = 1000/(2*f);
  digitalWrite(P_R, LOW);
  digitalWrite(RED_1, LOW);
  digitalWrite(LED_RED, LOW);
  delay(t);
  digitalWrite(P_R, HIGH);
  digitalWrite(RED_1, HIGH);
  digitalWrite(LED_RED, HIGH);
  delay(t);
}
void WriteData()
{
  myRadio.stopListening();              //Остановить прием и начать передачу
  myRadio.openWritingPipe(0xF0F0F0F066);//Отправить данные на этот 40-разрядный адрес
  myRadio.write(&data, sizeof(data)); 
  Serial.print("\nSent:");
  Serial.println(data.msg);
  delay(300);
}
