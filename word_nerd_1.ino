//WORD NERD
//PLAYER 1 (Green + Yellow LEDS)
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

//GREEN YELLOW BOARD, PLAYER 1
int count = 0; //number of words identified, starts with 0
int theirScore = 0;
bool keepPlaying = true;
bool youWin = false;
bool youLose = false;
bool firstTurn = true;
int text[1] = {};
int wordIndex = 0;
String myWord;
const int bigButton = 8; //big yellow button you gotta press
const int lilButton = 7; //small button used to switch words
const int led = A2;

const int winNumber = 5; //Number of words you need to get to win, SET HERE!
//DECLARE WORD DICTIONARIES HERE!

char *wordList [] = {"play", "design", "prototype", "CAD", "toy", "fun"};
int lenWordList = 6;
RF24 radio(9,10); //start radio

byte addresses[][6] = {"1Node", "2Node"}; //address list for pipes

void setup(void) {
  //pinMode(pushButton, INPUT_PULLUP);
  pinMode(bigButton, INPUT_PULLUP);
  pinMode(lilButton, INPUT_PULLUP);
  pinMode(led, OUTPUT); //victory LED lights up when the other person gets a word, also their count is displayed!

  radio.begin();
  radio.enableAckPayload();
  radio.enableDynamicPayloads();//not sure if either of these lines are necessary...
  radio.openWritingPipe(addresses[0]); //Get NRF24L01 ready to transmit
  radio.openReadingPipe(1, addresses[1]);
  radio.startListening(); //stop listening because we start as the sender
  
  Serial.begin(9600);

  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();
  display.setTextSize(1); //Set our text size, size 1 correlates to 8pt font
  display.setTextColor(WHITE);
  display.setCursor(0,0); //Start position for the font to appear
  display.println("Your lecture just got");
  display.println("10 times more");
  display.println("interesting");
  display.display();
  delay(4000);
}

void loop(void) {
  count = 0;
  theirScore = 0;
  display.clearDisplay();
  display.setCursor(0,0);
  display.println("New game!");
  display.println("Your word is...");
  display.display();
  delay(2000);
  display.clearDisplay();
  wordIndex = random(0, lenWordList);
  myWord = wordList[wordIndex];
  display.println(myWord);
  display.display();
  delay(2000);
  display.clearDisplay();
  display.setCursor(0,0);
  display.print("Word: " + myWord);
  display.setCursor(2,16);
  display.print("You: ");
  display.print(count);
  display.print(" Them: ");
  display.print(theirScore);
  display.display();
  keepPlaying = true;
  while (keepPlaying) {
    if (radio.available()) { //sends their score
      int text[1] = {};
      radio.read(&text, sizeof(text));
      theirScore = theirScore + 1;
      if (theirScore == winNumber) //if ten, they won!
      {
        display.clearDisplay();
        display.setCursor(0,0);
        display.println("You lost!");
        display.display();
        delay(2000);
        keepPlaying = false;
      }
      else {
        digitalWrite(led, HIGH);
        display.clearDisplay();
        display.setCursor(0,0);
        display.println("Word:" + myWord);
        display.setCursor(2,16);
        display.print("You:");
        display.print(count);
        display.print(" Them:");
        display.print(theirScore);
        display.display();
        digitalWrite(led, LOW);
      }
    }
    if (digitalRead(bigButton) == LOW) { //if big button pressed you got a word!
      count = count + 1;
      delay(500);
      if (count == winNumber) {
        display.clearDisplay();
        display.setCursor(0,0);
        display.println("Congrats winner!");
        display.display();
        delay(3000);
        keepPlaying = false;
      }
      else {
        display.clearDisplay();
        display.setCursor(0,0);
        display.println("Word:" + myWord);
        display.setCursor(2,16);
        display.print("You:");
        display.print(count);
        display.print(" Them: ");
        display.print(theirScore);
        display.display();
      }
      radio.stopListening();
      int text[1] = {count};
      radio.write(text[0], 1);
      radio.startListening();
    }
    if (digitalRead(lilButton) == LOW) { //switch word option here
      wordIndex = random(0, lenWordList);
      myWord = wordList[wordIndex];
      display.clearDisplay();
      display.setCursor(0,0);
      display.print("Word:" + myWord);
      display.setCursor(2,16);
      display.print("You:");
      display.print(count);
      display.print(" Them: ");
      display.print(theirScore);
      display.display();
    }
  }
}
