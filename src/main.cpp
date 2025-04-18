#include <Arduino.h>
#include <Keypad.h>
#include "Button2.h"
#include <Ultrasonic.h>
#include <LiquidCrystal_I2C.h>

void onButtonPressed(Button2 &btn);
void onButtonReleased(Button2 &btn);
void processInput(char* inputBuffer);
bool isValidNumber(String str);
void setupCustomChars();
void lcdInit(String strInputBuffer);

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
  setupCustomChars();
  lcd.backlight();

  // Welcome message
  lcd.setCursor(0, 0);
  lcd.print("SafeZone Ready !");
  lcd.setCursor(0, 1);
  lcd.print("v1.0");
} 

void loop() {
  if (millis() - lastDistanceUpdate > distanceDelay) {
    lastDistanceUpdate = millis();

    distance = ultrasonic.read(CM);
    Serial2.print("PIR_");
    Serial2.println(distance);
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
        Serial2.println("9");
        break;
      case '*':
        Serial2.println("*");
        break;
      case '#':
        Serial2.println("#");
        break;
    }
    //Serial.println(customKey);
  }

  while (Serial2.available() > 0) {
    char receivedChar = Serial2.read();
    /*
    Pour debug
    Serial.print(receivedChar);
    Serial.print("=");
    Serial.println((int)receivedChar);
    */
    
    // Si le caractère est un caractère de fin de ligne (\n), traiter la chaîne
    if (receivedChar == '\n' || receivedChar == '\r') {
      inputBuffer[bufferIndex] = '\0'; // Terminer la chaîne avec un caractère nul
      bufferIndex = 0;
      processInput(inputBuffer);
    } else {
      if (bufferIndex < bufferSize - 1) {
        inputBuffer[bufferIndex++] = receivedChar;
        // Si on veux de l'echo pour les tests
        //Serial2.print(receivedChar);
      }
    }
  }
}

void processInput(char* inputBuffer) {

  String strInputBuffer = "";

  // Replace les accents français
  for (int i = 0; inputBuffer[i] != '\0'; i++) {
    switch (inputBuffer[i]) {
      case 195: // UTF-8 encoding starts with 0xC3
        if (inputBuffer[i + 1] == 169) { // é
          strInputBuffer += char(6);
        } else if (inputBuffer[i + 1] == 160) { // à
          strInputBuffer += char(7);
        }
        break;
      case 169:
      case 170:
        // Ne pas traiter le 2 eme caractères de UTF-8
        break;
      default:
        strInputBuffer += inputBuffer[i];
        break;
    }
  }

  if (strInputBuffer == "CLS") {
    lcd.clear();
  } else if (strInputBuffer == "BACKLIGHT") {
    lcd.backlight();
  } else if (strInputBuffer == "NO_BACKLIGHT") {
    lcd.noBacklight();
  } else if (strInputBuffer == "BLINK") {
    lcd.blink();
  } else if (strInputBuffer == "NO_BLINK") {
    lcd.noBlink();
  } else if (strInputBuffer == "CURSOR") {
    lcd.cursor();
  } else if (strInputBuffer == "NO_CURSOR") {
    lcd.noCursor();
  } else if (strInputBuffer.startsWith("CUSTOM")) {
    String idxStr = strInputBuffer.substring(7,8);
    if (isValidNumber(idxStr)) {
      int idx = idxStr.toInt();
      lcd.write(byte(idx));
    }
  } else if (strInputBuffer.startsWith("INIT")) {
    lcdInit(strInputBuffer);
  } else if (strInputBuffer == "PING") {
    Serial2.println("PONG");
  } else {
    // Check le format "CC LL TEXT"
    String colStr = strInputBuffer.substring(0,2);
    String rawStr = strInputBuffer.substring(3,5);
    //lcd.clear();
    //lcd.print(strInputBuffer + "." + colStr + "." + rawStr);
    if (isValidNumber(colStr) && isValidNumber(rawStr)) {
      int col = colStr.toInt();
      int raw = rawStr.toInt();
      String text = strInputBuffer.substring(6);
      lcd.setCursor(col, raw);
      if (text.length() > 0) {
        lcd.print(text);
      }
    } else {
      lcd.clear();
      lcd.print(strInputBuffer);
    }
  }
}

void lcdInit(String strInputBuffer) {
  lcd.noCursor();
  lcd.noBlink();
  lcd.clear();
  String dlyStr = strInputBuffer.substring(5,9);
  if (isValidNumber(dlyStr)) {
    distanceDelay = dlyStr.toInt();
  }
  if (buttonMove.isPressed()) {
    Serial2.println("MOVE_OFF");
  } else {
    Serial2.println("MOVE_ON");
  }
  if (buttonPickup.isPressed()) {
    Serial2.println("RAC");
  } else {
    Serial2.println("DEC");
  }
}

void onButtonPressed(Button2 &btn) {
  switch (btn.getPin())
  {
    case BUTTON_PICKUP:
      Serial2.println("RAC");
      break;
    case BUTTON_MOVE:
      Serial2.println("MOVE_OFF");
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
      break;
    case BUTTON_MOVE:
      Serial2.println("MOVE_ON");
      break;
    default:
      break;
  }
}

bool isValidNumber(String str) {
  // Vérifier si la chaîne est vide ou contient des caractères non numériques
  if (str.length() == 0) return false;
  for (char c : str) {
    if (!isDigit(c)) return false;
  }
  return true;
}

// https://deepbluembedded.com/lcd-custom-character-generator/
void setupCustomChars() {
/*
  byte smiley[8] = {
      B00000,
      B10001,
      B00000,
      B00000,
      B10001,
      B01110,
      B00000,
    };
*/

  uint8_t smile[8] = {0x00, 0x00, 0x0a, 0x00, 0x11, 0x0e, 0x00, 0x00};
  uint8_t sad[8] = {0x00, 0x00, 0x0a, 0x00, 0x0e, 0x11, 0x00, 0x00};
  uint8_t heart[8] = {0x00, 0x00, 0x0a, 0x15, 0x11, 0x0a, 0x04, 0x00};
  uint8_t copy[8] = {0x0e, 0x11, 0x15, 0x19, 0x15, 0x11, 0x0e, 0x00};
  uint8_t humain[8] = {0x0e, 0x0e, 0x04, 0x0e, 0x15, 0x04, 0x0a, 0x0a};
  uint8_t clock[8] = {0x00, 0x00, 0x0e, 0x15, 0x17, 0x11, 0x0e, 0x00};
  uint8_t eAccentAigu[8] = {0x02, 0x04, 0x00, 0x0e, 0x11, 0x1f, 0x10, 0x0e};
  uint8_t aAccentGrave[8] = {0x04, 0x02, 0x00, 0x0e, 0x01, 0x0f, 0x11, 0x0f};

  lcd.createChar(0, smile);
  lcd.createChar(1, sad);
  lcd.createChar(2, heart);
  lcd.createChar(3, copy);
  lcd.createChar(4, humain);
  lcd.createChar(5, clock);
  lcd.createChar(6, eAccentAigu);
  lcd.createChar(7, aAccentGrave);
}
