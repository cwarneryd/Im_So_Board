# I'm So Board
## 2.00b Final Toy Product, Spring 2020
### Introduction
This is my final product for 2.00b, Toy Product Design, which was re-structured to be a remote design challenge
after students were kicked off campus due to Coronavirus. Each student was shipped a kit of materials (plus a $20 giftcard) and tasked with creating a toy that would entertain the people quarantined with them on "their island." Embracing this theme, I decided to create a toy that could be played while watching Zoom lectures - a tedious part of my daily routine when classes became virtualized.
### Game Overview
I'm So Board is a wireless game console combining two competitive, two-player games designed to be played while watching Zoom lectures. The first game, Simon 2.0, is a twist on the classic memory game Simon, but with two colors and two players competing to see who fails first! The second game, Word Nerd, is drinking game-inspired, where each player races to hear the most words mentioned in their lecture.
### Toy Specifications and Gameplay
I'm So Board runs on an Arduino Uno. The two boards communicate wirelessly via NRF24L01 chips. Individual single-color LEDs light up to portray a pattern in Simon 2.0 and are also connected to buttons for the purpose of repeating the pattern. Each board also includes two additional buttons for controlling game aspects in Word Nerd. Each board also includes a screen –  a Monochrome 128x32 12C OLED graphic display or an LCD display (I did not have access to two of the same kind). The electronic components are contained on a breadboard, which is attached (along with the Arduino) to a balsa-wood base with screws. I also added a balsa-wood cover made from hot-glued balsa wood pieces to cover many of the wires. This cover includes a wooden dowel that acts as a push stick for the "reset" button the Arduino. To play the games, the code for either game just needs to be uploaded to the Arduino using a USB cord. The boards can run off a battery or remain plugged into the computer. Simon 2.0 can be restarted by pressing the "reset" button on the boards, and Word Nerd plays on a continuous loop. The game is meant to be played by players in separate rooms and will work over the given range of the NRF24L01 chips.
### Simon 2.0 – Code Specifics
The two files needed to play Simon 2.0 are "simon_1" and "simon_2." 
### Word Nerd – Code Specifics
