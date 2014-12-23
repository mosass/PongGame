/*
********************************************* 
*
* Program : pongpong V1 (header)
* Author: Phanomphon Yotchon (TheMosass)
*
********************************************* 
*/

#include <16F877A.h>
#device adc=8

#FUSES NOWDT                    //No Watch Dog Timer
#FUSES HS                       //High speed Osc (> 4mhz for PCM/PCH) (>10mhz for PCD)
#FUSES NOPUT                    //No Power Up Timer
#FUSES NOPROTECT                //Code not protected from reading
#FUSES NODEBUG                  //No Debug mode for ICD
#FUSES NOBROWNOUT               //No brownout reset
#FUSES NOLVP                    //No low voltage prgming, B3(PIC16) or B5(PIC18) used for I/O
#FUSES NOCPD                    //No EE protection
#FUSES NOWRT                    //Program memory not write protected
#FUSES RESERVED                 //Used to set the reserved FUSE bits

#use delay(clock=20000000)

//Defind PIN
#define LEFT_BTN PIN_B0
#define MODE_BTN PIN_B1
#define START_BTN PIN_B2
#define RIGHT_BTN PIN_B4

#define game_mode_0 0b00000001
#define game_mode_1 0b00000010
#define game_mode_2 0b00000100
#define game_mode_3 0b00001000
#define game_mode_4 0b00010000


