//SIMON 2.0
//PLAYER 1 (Green + Yellow LEDs) - starts as TRANSMITTER
#include "nRF24L01.h"
#include "RF24.h"
#include "SPI.h"
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET     4 // Reset pin #
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

int l = 1; //l is the sequence length, starts with 1
bool gameOver = false;
bool youWin = false;
bool firstTurn = true;
int myLEDlist[30]; //LED list, won't be above 30 colors!
int text[30];
int lastMessage[30];
int ledIndex = 0; //keep track of which space you are in
bool takeInputs = true; //We send first, so this is TRUE
int countSequence = 0; //count until we reach the length we want
const int green = 2; //green LED pin
const int yellow = A2; //yellow LED pin
const int greenButton = 5; //button controlling green
const int yellowButton = 6;//button controlling yellow

RF24 radio(9,10); //start radio

byte addresses[][6] = {"1Node", "2Node"}; //address list for pipes

//SETUP LOOP
void setup(void) {
  //pinMode(pushButton, INPUT_PULLUP);
  pinMode(green, OUTPUT);
  pinMode(yellow, OUTPUT);
  pinMode(greenButton, INPUT_PULLUP);
  pinMode(yellowButton, INPUT_PULLUP);

  radio.begin();
  radio.enableAckPayload();
  radio.enableDynamicPayloads();//not sure if either of these lines are necessary...
  radio.openWritingPipe(addresses[0]); //Get NRF24L01 ready to transmit
  radio.openReadingPipe(1, addresses[1]);
  radio.stopListening(); //stop listening because we start as the sender
  firstTurn = true;
  
  Serial.begin(9600);

  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();
  display.setTextSize(1); //Set our text size, size 1 correlates to 8pt font
  display.setTextColor(WHITE);
  display.setCursor(0,0); //Start position for the font to appear
  display.println("Hello world,");
  display.println("Goodbye boredom");
  display.display();
}

//MAIN GAME LOOP, outer loop = send, inner = receive
void loop(void) {
  ledIndex = 0;
  takeInputs = true;
  int myLEDlist[30] = {};

  while (takeInputs == true)
  {
    if (digitalRead(greenButton) == LOW) {
      digitalWrite(green, HIGH);
      delay(500);
      digitalWrite(green, LOW);
      myLEDlist[ledIndex] = 0;
      if (firstTurn == true)
      {
        firstTurn = false;
      }
      else
      {
        if (ledIndex < l-1 and lastMessage[ledIndex] != myLEDlist[ledIndex]) {
          gameOver = true;
        }
      }
      ledIndex = ledIndex + 1;
    }
    else if (digitalRead(yellowButton) == LOW) {
      digitalWrite(yellow, HIGH);
      delay(500);
      digitalWrite(yellow, LOW);
      myLEDlist[ledIndex] = 1;
      if (firstTurn == true)
      {
        firstTurn = false;
      }
      else
      {
        if (ledIndex < l-1 and lastMessage[ledIndex] != myLEDlist[ledIndex]) {
          gameOver = true;
        }
      }
      ledIndex = ledIndex + 1;
    }
    if (gameOver)
    {
      takeInputs = false;
    }
    else if (ledIndex == l) { //stop when our index reaches the length of our sequence
      takeInputs = false;
    }
  }
  if (gameOver) {
    radio.write("lose",1);
    display.clearDisplay();
    display.println("You lose dude.");
    display.display();
    gameOver = false;
    l = 1;
  }
  else {
    radio.write(&myLEDlist, sizeof(myLEDlist));
    delay(50);
    radio.startListening();
  
    //RECEIVE LOOP!!!
    while(true)
    {
      if (radio.available())
      {
        l = l+1;
        int text[30] = {};
        radio.read(&text, sizeof(text));
        if (text[0] == 1 or text[0]==0) {
          radio.stopListening();
          for (int i = 0; i < l; i = i + 1) {
            lastMessage[i] = text[i];
            if (text[i] == 1)
            {
              digitalWrite(yellow, HIGH);
              digitalWrite(green, LOW);
            }
            else if (text[i] == 0)
            {
              digitalWrite(yellow, LOW);
              digitalWrite(green, HIGH);
            }
            delay(500);
            digitalWrite(yellow, LOW);
            digitalWrite(green, LOW);
            delay(100);
          }
        } else {
          display.clearDisplay();
          display.println("You win!");
          display.display();
          l = 1;
        }
        break;
      }
    }
    l = l+1;
  }
}
