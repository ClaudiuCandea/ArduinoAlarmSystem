#include <SoftwareSerial.h>
#include <Keypad.h>
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27, 16, 2);
char input_bluetooth;
int pirPin = 17;
int pirStat = LOW;

const byte ROWS = 4; 
const byte COLS = 4; 

char hexaKeys[ROWS][COLS] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};

byte rowPins[ROWS] = {9, 8, 7, 6}; 
byte colPins[COLS] = {5, 4, 3, 2};

Keypad customKeypad = Keypad(makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS);
char password[] = "12345";
char keypadInput[5];
int dataCounter = 0;
int changePassCounter = 0;

bool activeBuzzer = false;
bool activeSensor = false;
bool corrPass = false;
bool activeTimer = false;
bool activeChangePass = false;
bool activeSystem = false;

unsigned long currTime = 0;
unsigned long prevTime = 0;

SoftwareSerial BT(11,12);

void setup() {
  // put your setup code here, to run once:
  pinMode(13,OUTPUT);
  //pinMode(13,OUTPUT);
  pinMode(pirPin,INPUT);
  BT.begin(9600);
  Serial.begin(9600);
  lcd.begin();
  lcd.setCursor(0,0);
  lcd.print("Not Active");
}
void controlSensor(){
  if(activeSensor){
    pirStat = digitalRead(pirPin);
  }
}
void controlBT(){
   while(BT.available()>0){
    input_bluetooth = BT.read();
    if(input_bluetooth == 'o'){
      digitalWrite(13,HIGH);
      prevTime = millis();
      corrPass = false;
      activeTimer = true;
      lcd.clear();
      lcd.print("ACTIVE");
      activeSystem = true;
      prevTime = millis();
    }
    else if(input_bluetooth == 'c'){
      digitalWrite(13,LOW);
      noTone(10);
      activeBuzzer = false;
      activeSensor=false;
      pirStat = LOW;
      lcd.clear();
      lcd.print("NOT ACTIVE");
      activeSystem = false;
    }
  }
}

void submitPassword(){
  if(dataCounter == 5){;
    Serial.print(keypadInput);
    if(strncmp(keypadInput,password,5)==0){
        noTone(10);
        activeBuzzer = false;
        prevTime = millis();
        activeTimer = true;
        activeSensor = false;
        dataCounter = 0;
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("CORRECT");
        delay(5000);
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("ACTIVE");
        
    }
    else{
        dataCounter = 0;
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("WRONG");
        delay(5000);
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("ACTIVE");
    }
  }
}

void changePassword(){
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("CHANGE PASS");
  while(changePassCounter <5){
    char keypadChar = customKeypad.getKey();
    if(keypadChar){
      password[changePassCounter] = keypadChar;
      lcd.setCursor(changePassCounter,1);
      lcd.print(keypadChar);
      changePassCounter++;
    }
 }
 changePassCounter = 0;
 lcd.clear();
 lcd.setCursor(0,0);
 lcd.print("CHANGED");
 delay(5000);
 lcd.clear();
 lcd.setCursor(0,0);
 lcd.print("ACTIVE");
 
}
void processPassword(){
  char keypadChar = customKeypad.getKey();
  if(keypadChar){
    if(keypadChar == '*'){
      submitPassword();
      
    }
    else if(keypadChar == 'A' && activeBuzzer == false){
      changePassword();
    }
    else{
      if(dataCounter<5){
        keypadInput[dataCounter] = keypadChar;
        lcd.setCursor(dataCounter,1);
        lcd.print(keypadChar);
        dataCounter++;
      }
    }
  }
}
void controlBuzzer(){
  if(pirStat == HIGH){
    tone(10,1000);
    activeBuzzer = true;
  }
}

void counter(){
  
  if(activeTimer == true){
    //digitalWrite(13,HIGH);
    if((millis() - prevTime) >= 10000){
    //delay(120000);
      activeTimer = false;
      activeSensor = true;
      //digitalWrite(13,LOW);
}
}
}

void loop() {
  // put your main code here, to run repeatedly:
  controlBT();
  if(activeSystem){
    counter();
    controlSensor();
    controlBuzzer();
    processPassword();
  }
}
