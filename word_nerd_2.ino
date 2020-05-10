//WORD NERD
//PLAYER 2 (Red + Blue LEDs)
#include "nRF24L01.h"
#include "RF24.h"
#include "SPI.h"
#include <LiquidCrystal.h>

const int rs = A5, en = A4, d4 = 5, d5 = 6, d6 = 7, d7 = 8; //pins for LCD screen wiring
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

int count = 0; //number of words identified, starts with 0
int theirScore = 0; //other player's score
bool keepPlaying = true;
bool youWin = false;
bool youLose = false;
bool firstTurn = true;
int text[1] = {};
int wordIndex = 0;
String myWord;
const int bigButton = A0; //big yellow button you gotta press
const int lilButton = 3; //small button used to switch words
const int led = 4;

const int winNumber = 5; //Number of words you need to get to win, SET HERE!

//DECLARE WORD DICTIONARIES HERE! currently one sample dictionary included
//Will add option to choose category before gameplay (i.e select dictionary)
char *wordList [] = {"right?", "and umm...", "clearly", "however", "as proven...", "so..."};
int lenWordList = 6; //set value for length of word list

RF24 radio(9,10); //start radio
byte addresses[][6] = {"1Node", "2Node"}; //address list for pipes

//SETUP LOOP
void setup(void) {
  pinMode(bigButton, INPUT_PULLUP);//button to press when you hear a word
  pinMode(lilButton, INPUT_PULLUP);//button to press for switching words
  pinMode(led, OUTPUT); //LED lights up when your opponent gets a word

  radio.begin();
  radio.enableAckPayload();
  radio.enableDynamicPayloads();//not sure if either of these lines are necessary...
  radio.openWritingPipe(addresses[1]); //Get NRF24L01 ready to transmit
  radio.openReadingPipe(1, addresses[0]);
  radio.startListening();
  
  Serial.begin(9600);
  
  //intro message
  lcd.begin(16,2);
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Hello world,");
  lcd.setCursor(0,1);
  lcd.print("Goodbye boredom");
  delay(4000);
}

void loop(void) {
  count = 0;
  theirScore = 0;
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("New game!");
  lcd.setCursor(0,1);
  lcd.print("Your word is...");
  delay(2000);
  lcd.clear();
  lcd.setCursor(0,0);
  wordIndex = random(0, lenWordList);
  myWord = wordList[wordIndex];
  lcd.print(myWord);
  delay(2000);
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Word:" + myWord);
  lcd.setCursor(0,1);
  lcd.print("You: ");
  lcd.print(count);
  lcd.print(" Them: ");
  lcd.print(theirScore);
  keepPlaying = true;
  
  while (keepPlaying) {
    if (radio.available()) { //sends their score
      int text[1] = {};
      radio.read(&text, sizeof(text));
      theirScore = theirScore + 1;
      if (theirScore == winNumber) //if opponent has reached winning number, they won!
      {
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("You lost!");
        delay(2000);
        keepPlaying = false;
      }
      else { //update score display
        digitalWrite(led, HIGH);
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("Word:" + myWord);
        lcd.setCursor(0,1);
        lcd.print("You:");
        lcd.print(count);
        lcd.print(" Them:");
        lcd.print(theirScore);
        delay(500);
        digitalWrite(led, LOW);
      }
    }
    if (digitalRead(bigButton) == LOW) { //if big button pressed, you got a word!
      delay(500);
      count = count + 1;
      if (count == winNumber) {
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.println("Congrats winner!");
        delay(3000);
        keepPlaying = false;
      }
      else {
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("Word:" + myWord);
        lcd.setCursor(0,1);
        lcd.print("You:");
        lcd.print(count);
        lcd.print(" Them: ");
        lcd.print(theirScore);
      }
      radio.stopListening(); //send your score to the opponent
      int text[1] = {count};
      radio.write(text[0], 1);
      radio.startListening();
    }
    if (digitalRead(lilButton) == LOW) { //switch word option here
      wordIndex = random(0, lenWordList);
      myWord = wordList[wordIndex];
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Word:" + myWord);
      lcd.setCursor(0,1);
      lcd.print("You:");
      lcd.print(count);
      lcd.print(" Them: ");
      lcd.print(theirScore);
    }
  }
}
