#include <Arduino.h>
#include <Keyboard.h>
#include <Mouse.h>
#include <Keyboard_fr_FR.h>
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
byte rowPins[ROWS] = { 10, 16, 14, 15 };
byte colPins[COLS] = { 7, 8, 9 };

Keypad kpd = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

// ULTRASONIC
Ultrasonic ultrasonic(4, 5);	// An ultrasonic sensor HC-04 - TRIGGER / ECHO
int distance;
unsigned long lastDistanceUpdate = 0;
unsigned long distanceDelay = 1000;

// BUTTON2
const uint8_t BUTTON_PICKUP = 6;
Button2 button;

// LCD
LiquidCrystal_I2C lcd(0x27, 16, 2);

// SERIAL1
const byte bufferSize = 64;
char inputBuffer[bufferSize];
byte bufferIndex = 0;


void setup() {
  Serial.begin(9600);

  Serial1.begin(9600);
  Serial1.println("READY !");

  Keyboard.begin(KeyboardLayout_fr_FR);
  Mouse.begin();

  button.begin(BUTTON_PICKUP);
  button.setPressedHandler(onButtonPressed);
  button.setReleasedHandler(onButtonReleased);

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

  button.loop();

  char customKey = kpd.getKey();
  if (customKey) {
    switch (customKey) {
      case '0':
        Keyboard.write('0');
        break;
      case '1':
          Keyboard.write('1');
        break;
      case '2':
        Keyboard.write('2');
        break;
      case '3':
        Keyboard.write('3');
        break;
      case '4':
        Keyboard.write('4');
        break;
      case '5':
        Keyboard.write('5');
        break;
      case '6':
        Keyboard.write('6');
        break;
      case '7':
        Keyboard.write('7');
        break;
      case '8':
        Keyboard.write('8');
        break;
      case '9':
        Keyboard.write('9');
        break;
      case '*':
        Keyboard.write('*');
        break;
      case '#':
        Keyboard.write('#');
        break;
    }
    //Serial.println(customKey);
    lcd.setCursor(15, 0);
    lcd.print(customKey);
  }

  while (Serial1.available() > 0) {
    char receivedChar = Serial1.read(); // Lire un caractère

    // Si le caractère est un caractère de fin de ligne (\n), traiter la chaîne
    if (receivedChar == '\n' || receivedChar == '$') {
      inputBuffer[bufferIndex] = '\0'; // Terminer la chaîne avec un caractère nul
      Serial.print("Chaîne reçue sur Serial1 : ");
      Serial.println(inputBuffer);
      bufferIndex = 0;
      
      Serial1.println("");

      lcd.setCursor(0, 0);
      lcd.print(inputBuffer);
    } else {
      if (bufferIndex < bufferSize - 1) {
        inputBuffer[bufferIndex++] = receivedChar;
        Serial1.print(receivedChar);
      }
    }
  }
}


void onButtonPressed(Button2 &btn) {
  switch (btn.getPin())
  {
    case BUTTON_PICKUP:
      Keyboard.write('R');
      lcd.setCursor(11, 0);
      lcd.print("Rac");
      break;
    default:
      break;
  }
}

void onButtonReleased(Button2 &btn) {
  switch (btn.getPin())
  {
    case BUTTON_PICKUP:
      Keyboard.write('D');
      lcd.setCursor(11, 0);
      lcd.print("Dec");
      break;
    default:
      break;
  }
}
