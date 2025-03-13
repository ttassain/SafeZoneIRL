#include <Arduino.h>
#include <Keypad.h>
#include "Button2.h"
#include <Ultrasonic.h>
#include <LiquidCrystal_I2C.h>

void onButtonPressed(Button2 &btn);
void onButtonReleased(Button2 &btn);

// -------------------------------------------------------

// KEYPAD
const byte ROWS = 4;
const byte COLS = 3;

char keys[ROWS][COLS] = {
  { '1', '2', '3' },
  { '4', '5', '6' },
  { '7', '8', '9' },
  { '*', '0', '#' }
};
byte rowPins[ROWS] = { 32, 33, 18, 19 };
byte colPins[COLS] = { 25, 26, 27 };

Keypad kpd = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

// ULTRASONIC
Ultrasonic ultrasonic(13, 14);	// An ultrasonic sensor HC-04 - TRIGGER / ECHO
int distance;
unsigned long lastDistanceUpdate = 0;
unsigned long distanceDelay = 1000;

// BUTTON2
const uint8_t BUTTON_PICKUP = 4;
const uint8_t BUTTON_MOVE = 5;
Button2 buttonPickup;
Button2 buttonMove;

// LCD
LiquidCrystal_I2C lcd(0x27, 16, 2);

// SERIAL
const byte bufferSize = 64;
char inputBuffer[bufferSize];
byte bufferIndex = 0;


void setup() {
  Serial.begin(115200);

  Serial2.begin(9600);
  Serial2.println("READY !");

  buttonPickup.begin(BUTTON_PICKUP);
  buttonPickup.setPressedHandler(onButtonPressed);
  buttonPickup.setReleasedHandler(onButtonReleased);

  buttonMove.begin(BUTTON_MOVE);
  buttonMove.setPressedHandler(onButtonPressed);
  buttonMove.setReleasedHandler(onButtonReleased);


  lcd.init();
  lcd.backlight();

  lcd.setCursor(0, 0);
  lcd.print("Safe Zone");
  lcd.setCursor(0, 1);
  lcd.print("Distance :");
} 

void loop() {
  if (millis() - lastDistanceUpdate > distanceDelay) {
    lastDistanceUpdate = millis();

    distance = ultrasonic.read(CM);
    lcd.setCursor(11, 1);
    lcd.print("    ");
    lcd.setCursor(11, 1);
    lcd.print(distance);
    //Serial.print("Distance in CM: ");
    //Serial.println(distance);
  }

  buttonPickup.loop();
  buttonMove.loop();

  char customKey = kpd.getKey();
  if (customKey) {
    switch (customKey) {
      case '0':
        Serial2.println("0");
        break;
      case '1':
        Serial2.println("1");
        break;
      case '2':
        Serial2.println("2");
        break;
      case '3':
        Serial2.println("3");
        break;
      case '4':
        Serial2.println("4");
        break;
      case '5':
        Serial2.println("5");
        break;
      case '6':
        Serial2.println("6");
        break;
      case '7':
        Serial2.println("7");
        break;
      case '8':
        Serial2.println("8");
        break;
      case '9':
        Serial2.println("*");
        break;
      case '*':
        Serial2.println("*");
        break;
      case '#':
        Serial2.println("#");
        break;
    }
    //Serial.println(customKey);

    lcd.setCursor(15, 0);
    lcd.print(customKey);
  }

  while (Serial2.available() > 0) {
    char receivedChar = Serial2.read(); // Lire un caractère

    // Si le caractère est un caractère de fin de ligne (\n), traiter la chaîne
    if (receivedChar == '\n' || receivedChar == '$') {
      inputBuffer[bufferIndex] = '\0'; // Terminer la chaîne avec un caractère nul
      //Serial.print("Chaîne reçue sur Serial2 : ");
      Serial.println(inputBuffer);
      bufferIndex = 0;

      Serial2.println("");

      lcd.setCursor(0, 0);
      lcd.print(inputBuffer);
    } else {
      if (bufferIndex < bufferSize - 1) {
        inputBuffer[bufferIndex++] = receivedChar;
        Serial2.print(receivedChar);
      }
    }
  }
}


void onButtonPressed(Button2 &btn) {
  switch (btn.getPin())
  {
    case BUTTON_PICKUP:
      Serial2.println("RAC");
      lcd.setCursor(13, 0);
      lcd.print("R");
      break;
    case BUTTON_MOVE:
      Serial2.println("MOVE_OFF");
      lcd.setCursor(11, 0);
      lcd.print("-");
      //Serial.println("MOVE_OFF");
      break;
    default:
      break;
  }
}

void onButtonReleased(Button2 &btn) {
  switch (btn.getPin())
  {
    case BUTTON_PICKUP:
      Serial2.println("DEC");
      lcd.setCursor(13, 0);
      lcd.print("D");
      break;
    case BUTTON_MOVE:
      Serial2.println("MOVE_ON");
      lcd.setCursor(11, 0);
      lcd.print("M");
      //Serial.println("MOVE_ON");
      break;
    default:
      break;
  }
}
