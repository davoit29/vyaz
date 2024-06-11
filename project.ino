#define  avoidPin  A1 // задаем имя для порта с датчиком
#define  ledPin  13     // задаем имя для порта со светодиодом
#define DIR_PIN 5 // Пин направления
#define STEP_PIN 6 // Пин шага
//encoder part:
#define ENCODER_A_PIN 2
#define ENCODER_B_PIN 3

int  avoid;
int  prevAvoid = LOW;  // добавляем переменную для отслеживания предыдущего состояния
int  counter = 0;  // добавляем счетчик
//encoder part:
volatile long encoderValue = 1;
long lastEncoderValue = 0;
int lastMSB = 0;
int lastLSB = 0;


void setup() {
  pinMode(DIR_PIN, OUTPUT);
  pinMode(STEP_PIN, OUTPUT);
  Serial.begin(57600);  // инициализация монитора порта

  pinMode(avoidPin, INPUT);
  pinMode(ledPin, OUTPUT);

   pinMode(ENCODER_A_PIN, INPUT_PULLUP);
  pinMode(ENCODER_B_PIN, INPUT_PULLUP);

  // Прерывания для пинов A и B энкодера
  attachInterrupt(digitalPinToInterrupt(ENCODER_A_PIN), updateEncoder, CHANGE);
  attachInterrupt(digitalPinToInterrupt(ENCODER_B_PIN), updateEncoder, CHANGE);

  Serial.begin(57600);
}

void loop() {

//работа датчика движения

//encoder part:
 if (lastEncoderValue != encoderValue) {
    Serial.println(encoderValue);
    lastEncoderValue = encoderValue;
  }

  
  digitalWrite(DIR_PIN, HIGH); // Задаем направление вращения

  // Делаем 200 шагов (полный оборот для двигателя с шагом 1.8 градуса)
  for(int i = 0; i<10000; i++) {
    if( counter < 40) {
      digitalWrite(STEP_PIN, HIGH);
      delayMicroseconds(1000); // Задержка определяет скорость вращения
      digitalWrite(STEP_PIN, LOW);
      delayMicroseconds(1000);
      avoid = digitalRead(avoidPin);  // получаем данные с датчика препятствий

     Serial.print("Avoid Sensor - ");  // выводим данные с датчика на монитор
   Serial.println(avoid);
     Serial.print("Encoder - ");  // выводим данные с датчика на монитор
   Serial.println(encoderValue);

   if (avoid == HIGH) {
      digitalWrite(ledPin, HIGH);
      if (prevAvoid == LOW) {  // если предыдущее состояние было LOW, увеличиваем счетчик
         counter++;
         Serial.print("Counter: ");  // выводим значение счетчика на монитор
         Serial.println(counter);
      }
   }
   else {
      digitalWrite(ledPin, LOW);
   }

   prevAvoid = avoid;
   delay(encoderValue);  // сохраняем текущее состояние как предыдущее для следующей итерации
    }
    else {
      delay(1000);
      counter = 0;
    }
  }
//работа двигателя
}
//encoder part:
void updateEncoder() {
  int MSB = digitalRead(ENCODER_A_PIN); //Most significant bit
  int LSB = digitalRead(ENCODER_B_PIN); //Least significant bit

  int encoded = (MSB << 1) | LSB; //converting the 2 pin value to single number
  int sum = (lastMSB << 3) | (lastLSB << 2) | encoded; //adding it to the previous encoded value

  if(sum == 0b0001 || sum == 0b0111 || sum == 0b1110 || sum == 0b1000) encoderValue += 1;
  if(sum == 0b0010 || sum == 0b1011 || sum == 0b1101 || sum == 0b0100) encoderValue -= 1;

  lastMSB = MSB;
  lastLSB = LSB;
}