//SIMON 2.0
//PLAYER 2 (Red + Blue LEDS) - starts as RECEIVER
#include "nRF24L01.h"
#include "RF24.h"
#include "SPI.h"
#include <LiquidCrystal.h>

bool gameOver = false;
bool youWin = false;
int myLEDlist[30]; //LED list (holds pattern)
int red = 4;
int blue = A3;
int redButton = 3;
int blueButton = 2; 
int takeInputs = false; //player 2 recieves first, this is FALSE
int ledIndex = 0;
int l = 0; //expecting sequence of length 1, returns sequence of length 2, 4, 6, etc.

const int rs = A5, en = A4, d4 = 5, d5 = 6, d6 = 7, d7 = 8; //pins for LCD screen wiring
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

RF24 radio (9,10);
byte addresses[][6] = {"1Node", "2Node"};

//SETUP LOOP
void setup(void) {
  Serial.begin(9600);
  pinMode(red, OUTPUT);
  pinMode(blue, OUTPUT);
  pinMode(redButton, INPUT_PULLUP);
  pinMode(blueButton, INPUT_PULLUP);

  radio.begin();
  radio.enableAckPayload();
  radio.enableDynamicPayloads();
  radio.openReadingPipe(1, addresses[0]);
  radio.openWritingPipe(addresses[1]);
  radio.startListening();

  lcd.begin(16,2);
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Hello world,");
  lcd.setCursor(0,1);
  lcd.print("Goodbye boredom");
  digitalWrite(red, LOW);
  digitalWrite(blue, LOW);
}

void loop(void) {
  // OUTER LOOP = RECEIVE, INNER = SEND
  if (radio.available()) {
    Serial.print("t");
    int text[30] = {};
    radio.read(&text, sizeof(text));
    Serial.print("outside");
    Serial.print(text[0]);
    Serial.print(text[1]);
    Serial.print(text[2]);
    
    if (text[0] == 1 or text[0]==0) {
      radio.stopListening();
      l = l+1;
      for (int i = 0; i < l; i = i + 1) {
        Serial.print("i");
        Serial.println(text[i]);
        if (text[i] == 1)
        {
          Serial.println("RED");
          digitalWrite(red, HIGH);
          digitalWrite(blue, LOW);
        }
        else if (text[i] == 0)
        {
          Serial.println("BLUE");
          digitalWrite(red, LOW);
          digitalWrite(blue, HIGH);
        }
        delay(500);
        digitalWrite(red, LOW);
        digitalWrite(blue, LOW);
        delay(100);
      }
    }
    else {
      youWin = true;
      radio.stopListening();
    }
    
    l = l+1;
    if (youWin == false) {
      takeInputs = true;
    }
    int myLEDlist[30] = {};
    ledIndex = 0;
    
    while(takeInputs == true) {
      if (digitalRead(blueButton) == LOW) {
        myLEDlist[ledIndex] = 0;
        if (ledIndex < l-1 and text[ledIndex] != myLEDlist[ledIndex])
        {
          gameOver = true;
        }
        digitalWrite(blue, HIGH);
        delay(500);
        digitalWrite(blue, LOW);
        ledIndex = ledIndex + 1;
      }
      else if (digitalRead(redButton) == LOW) {
        myLEDlist[ledIndex] = 1;
        if (ledIndex < l-1 and text[ledIndex] != myLEDlist[ledIndex]) {
          gameOver = true;
        }
        digitalWrite(red, HIGH);
        delay(500);
        digitalWrite(red, LOW);
        ledIndex = ledIndex + 1;
      }
      if (gameOver) {
        takeInputs = false;
      }
      else if (ledIndex == l) {
        takeInputs = false;
      }
    }
    //check if you input pattern correctly
    if (youWin) {
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("You won dude!");
      l=0;
      youWin = false;
    }
    else if (gameOver) {
      //game resets
      radio.write(-1, 1); //special message letting other board know that they won!
      Serial.print("You lost dude");
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("You lose dude.");
      l = 0;
      gameOver = false;
    }
    else {
      radio.write(&myLEDlist, sizeof(myLEDlist));
      delay(10);
      radio.startListening();
    }
  }
}
