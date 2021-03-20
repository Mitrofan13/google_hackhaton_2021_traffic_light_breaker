const int LED_RED = 13;                // Порт 13, красный светодиод
const int LED_YELLOW = 12;             // Порт 12, желтый светодиод
const int LED_GREEN = 11;              // Порт 11, зеленый светодиод

const int echoPin = 9;                 //Порт 9, echoPin
const int trigPin = 10;                //Порт 10, trigPin
#define INTERRUPT_PIN 2
#define TEST_LED 4
const int TIMEOUT_RED = 3000;          // Время горения красного сетодиода
const int TIMEOUT_YEL = 1690;          // Время горения желтого светодиода
const int TIMEOUT_GREEN = 2000;        // Время горения зеленого светодиода

const int TIMEOUT_FLASH_GREEN = 500;      // Время мигания зеленого светодиода

void setup() {
  // put your setup code here, to run once:
  Serial.begin (9600);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  // Все порты светодиодов будут у нас установлены в режим "внешняя нагрузка", OUTPUT
  pinMode(LED_RED, OUTPUT);
  pinMode(LED_YELLOW, OUTPUT);
  pinMode(LED_GREEN, OUTPUT);
  pinMode(TEST_LED, OUTPUT);
  // Устанавливаем начальное значение светодиодов
  digitalWrite(TEST_LED, LOW);
  digitalWrite(LED_RED, LOW);
  digitalWrite(LED_YELLOW, LOW);
  digitalWrite(LED_GREEN, LOW);
  // INTERRUPT
  attachInterrupt(0, interr_func, CHANGE);

}
#define GO 0
#define GO_END 1
#define YELLOW 2
#define STOP 3
#define READY 4
#define EMERGENCY 5
int FLAG = 0;

volatile uint32_t debounce;
void interr_func()
{
  if (millis() - debounce >= 100 && digitalRead(2)) {
    debounce = millis();
    FLAG = 1;
  }
  
}
int state_now = 0;

void loop() {
  // put your main code here, to run repeatedly:
  switch(state_now)
  {
    case GO:
      digitalWrite(LED_YELLOW, LOW);
      digitalWrite(LED_RED, LOW);
      digitalWrite(LED_GREEN, HIGH);    // Включаем светодиод
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
        state_now = YELLOW;
      }
      break;
      
    case YELLOW:
      digitalWrite(LED_GREEN, LOW);
      digitalWrite(LED_YELLOW, HIGH);
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
      if(my_delay(TIMEOUT_RED)==1)
      {
        state_now = READY;
        break;
      }
      state_now = READY;
      break;
      
    case READY:
      digitalWrite(LED_YELLOW, HIGH);
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
    if(FLAG==1)
    {
      emergency_mode();
      return 1;
    }
  }
}
void emergency_mode()
{
  digitalWrite(TEST_LED, HIGH);
  digitalWrite(LED_RED, LOW);
  digitalWrite(LED_YELLOW, HIGH);
  digitalWrite(LED_GREEN, LOW);
  delay(1000);
  digitalWrite(LED_RED, HIGH);
  digitalWrite(LED_YELLOW, LOW);
  while(distance() >= 5)
  {
    blink_dist(distance());
  }
  delay(5000);
  digitalWrite(TEST_LED, LOW);
  state_now = YELLOW;
  FLAG = 0;
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
void blink_dist(int duration)
{
  if(duration >=50) duration = 50;
  int f = map(duration, 5, 50, 15, 5);
  int t = 1000/(2*f);
  digitalWrite(LED_RED, LOW);
  delay(t);
  digitalWrite(LED_RED, HIGH);
  delay(t);
}
