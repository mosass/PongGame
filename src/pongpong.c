/*
********************************************* 
*
* Program : pongpong V1
* Author: Phanomphon Yotchon (TheMosass)
*
********************************************* 
*/

#include "pongpong.h"
#include "max7219.h"

#define wait_ms delay_ms(170) //game speed
#define PAUSE 0
#define START 1

#define UP 1
#define UP_RIGHT 2
#define DOWN_RIGHT 3
#define DOWN 4
#define DOWN_LEFT 5
#define UP_LEFT 6

int1 game_status = 0; //0 => pause, 1 => start
int1 game_over = 0; // 0 => not over, 1 => game over
int game_mode = game_mode_0; //0, 1, 2, 3, 4
int game_mode_select = game_mode_0;

////////////////////
//pong_direction  //
//    6  1  2     //
//      \|/       //
//       0        //
//      /|\       //
//    5  4  3     //
////////////////////
int pong_direction = DOWN;

int racket_1 = 0;
int racket_2 = 0;

///////////////////
// 0 -> 00000000 //
// 1 -> 00000000 //
// 2 -> 00000000 //
// 3 -> 00000000 //
// 4 -> 00000000 //
// 5 -> 00000000 //
// 6 -> 00000000 //
// 7 -> 00000000 //
///////////////////
int pong_position = 0;
int pong_row = 0;


int disp[8] = {
   0b00000000,
   0b00000000,
   0b00000000,
   0b00000000,
   0b00000000,
   0b00000000,
   0b00000000,
   0b00000000
};

int disp_over[8] = {
   0b00000000,
   0b01000010,
   0b00100100,
   0b00011000,
   0b00011000,
   0b00100100,
   0b01000010,
   0b00111100
};

int disp_start[8] = {
   0b00000000,
   0b00010000,
   0b00011000,
   0b00011100,
   0b00011000,
   0b00010000,
   0b00000000,
   0b00011100
};

void animetion(){
   int i, j;
   int disp_anime[8] = {
      0b00000000,
      0b00000000,
      0b00000000,
      0b00000000,
      0b00000000,
      0b00000000,
      0b00000000,
      0b00000000
   };
   for(i = 0; i < 9; i++){
      for(j = 0; j < 9 && j < i; j++){
         disp_anime[j] = 0b11111111; 
      }
      render_dotmatrix(disp_anime);
   }
   for(i = 0; i < 9; i++){
      for(j = 0; j < 9 && j < i; j++){
         disp_anime[j] = 0b00000000; 
      }
      render_dotmatrix(disp_anime);
   }
}

int get_position_in_byte(int _byte){
   int i;
   for(i = 0; i < 8; i++){
      if((_byte >> i)&0b00000001)
         return i;
   }
}

/*
Functions for game mode 0, 1
Use : pong_direction, racket_1, pong_position, pong_row, disp
*/
void init_game_0_1(){   
   pong_direction = DOWN;
   racket_1 = 0b00111000;
   pong_position = 1;
   pong_row = 0b00001000;
}

void render_display_game_0_1(){
   int i;
   for(i = 0; i < 7; i++){
      disp[i] = 0;
   }
   disp[pong_position] = pong_row;
   disp[7] = racket_1;
}

void process_display_game_0_1(){
   switch(pong_direction){
      case UP://up
         if(pong_position == 0){
            pong_direction = DOWN;
         }
         else {
            pong_position -= 1;
         }
         break;
         
      case DOWN:
         if(pong_position == 6){
            int pong_pos = get_position_in_byte(pong_row);
            int recket_pos = get_position_in_byte(racket_1);
            if(pong_pos == recket_pos){
               pong_direction = UP_RIGHT;
            }
            else if(pong_pos == recket_pos + 1){
               pong_direction = UP;
            }
            else if(pong_pos == recket_pos + 2){
               pong_direction = UP_LEFT;
            }
            else {
               game_over = 1;
            }
         }
         else {
            pong_position += 1;
         }
         break;
      
      case UP_RIGHT:
         if(pong_position == 0 || pong_row == 0b00000001){
            if(pong_position > 0 && pong_row == 0b00000001){
               pong_direction = UP_LEFT;
            }
            else if(pong_position == 0 && pong_row != 0b00000001){
               pong_direction = DOWN_RIGHT;
            }
            else if(pong_position == 0 && pong_row == 0b00000001){
               pong_direction = DOWN_LEFT;
            }
         }
         else{
            pong_position -= 1;
            pong_row >>= 1;
         }
         break;
         
      case UP_LEFT:
         if(pong_position == 0 || pong_row == 0b10000000){
            if(pong_position > 0 && pong_row == 0b10000000){
               pong_direction = UP_RIGHT;
            }
            else if(pong_position == 0 && pong_row != 0b10000000){
               pong_direction = DOWN_LEFT;
            }
            else if(pong_position == 0 && pong_row == 0b10000000){
               pong_direction = DOWN_RIGHT;
            }
         }
         else{
            pong_position -= 1;
            pong_row <<= 1;
         }
         break;
         
      case DOWN_LEFT:
         if(pong_position == 6 || pong_row == 0b10000000){
            if(pong_position < 6 && pong_row == 0b10000000){
               pong_direction = DOWN_RIGHT;
            }
            else if(pong_position == 6){
               int pong_pos = get_position_in_byte(pong_row);
               int recket_pos = get_position_in_byte(racket_1);
               if(pong_pos == recket_pos - 1){
                  pong_direction = UP_RIGHT;
               }
               else if(pong_pos == recket_pos){
                  pong_direction = UP;
               }
               else if(pong_pos == recket_pos + 1){
                  pong_direction = UP_LEFT;
               }
               else if(pong_pos == recket_pos + 2){
                  pong_direction = UP_LEFT;
               }
               else {
                  game_over = 1;
               }
            }
         }
         else{
            pong_position += 1;
            pong_row <<= 1;
         }
         break;
         
      case DOWN_RIGHT:
         if(pong_position == 6 || pong_row == 0b00000001){
            if(pong_position < 6 && pong_row == 0b00000001){
               pong_direction = DOWN_LEFT;
            }
            else if(pong_position == 6){
               int pong_pos = get_position_in_byte(pong_row);
               int recket_pos = get_position_in_byte(racket_1);
               if(pong_pos == recket_pos){
                  pong_direction = UP_RIGHT;
               }
               else if(pong_pos == recket_pos + 1){
                  pong_direction = UP_RIGHT;
               }
               else if(pong_pos == recket_pos + 2){
                  pong_direction = UP;
               }
               else if(pong_pos == recket_pos + 3){
                  pong_direction = UP_LEFT;
               }
               else {
                  game_over = 1;
               }
            }
         }
         else{
            pong_position += 1;
            pong_row >>= 1;
         }
         break;
   }
   render_display_game_0_1();
}
///////////////////////////////////////////////////////////////////


/*
Functions for game mode 2, 3, 4
Use : pong_direction, racket_1, racket_2, pong_position, pong_row, disp
*/
void init_game(){   
   pong_direction = DOWN;
   pong_position = 3;
   pong_row = 0b00100000;
   
   switch(game_mode){
      case game_mode_2: case game_mode_3:
         racket_1 = 0b00111000;
         racket_2 = 0b00111000;
         break;
      case game_mode_4:
         racket_1 = 0b11100000;
         racket_2 = 0b00000111;
         break;
   }
}

void render_display_game(){
   int i;
   for(i = 1; i < 7; i++){
      disp[i] = 0;
   }
   disp[0] = racket_2;
   disp[pong_position] = pong_row;
   disp[7] = racket_1;
}

void process_display_game(){
   switch(pong_direction){
      case UP:
         if(pong_position == 1){
            int pong_pos = get_position_in_byte(pong_row);
            int recket_2_pos = get_position_in_byte(racket_2);
            if(pong_pos == recket_2_pos){
               pong_direction = DOWN_RIGHT;
            }
            else if(pong_pos == recket_2_pos + 1){
               pong_direction = DOWN;
            }
            else if(pong_pos == recket_2_pos + 2){
               pong_direction = DOWN_LEFT;
            }
            else {
               game_over = 1;
            }
         }
         else {
            pong_position -= 1;
         }
         break;
         
      case DOWN:
         if(pong_position == 6){
            int pong_pos = get_position_in_byte(pong_row);
            int recket_pos = get_position_in_byte(racket_1);
            if(pong_pos == recket_pos){
               pong_direction = UP_RIGHT;
            }
            else if(pong_pos == recket_pos + 1){
               pong_direction = UP;
            }
            else if(pong_pos == recket_pos + 2){
               pong_direction = UP_LEFT;
            }
            else {
               game_over = 1;
            }
         }
         else {
            pong_position += 1;
         }
         break;
      
      case UP_RIGHT:
         if(pong_position == 1 || pong_row == 0b00000001){
            if(pong_position > 1 && pong_row == 0b00000001){
               pong_direction = UP_LEFT;
            }
            else if(pong_position == 1){
               int pong_pos = get_position_in_byte(pong_row);
               int recket_2_pos = get_position_in_byte(racket_2);
               if(pong_pos == recket_2_pos){
                  pong_direction = DOWN_LEFT;
               }
               else if(pong_pos == recket_2_pos + 1){
                  pong_direction = DOWN_LEFT;
               }
               else if(pong_pos == recket_2_pos + 2){
                  pong_direction = DOWN;
               }
               else if(pong_pos == recket_2_pos + 3){
                  pong_direction = DOWN_RIGHT;
               }
               else {
                  game_over = 1;
               }
            }
         }
         else{
            pong_position -= 1;
            pong_row >>= 1;
         }
         break;
         
      case UP_LEFT:
         if(pong_position == 1 || pong_row == 0b10000000){
            if(pong_position > 1 && pong_row == 0b10000000){
               pong_direction = UP_RIGHT;
            }
            else if(pong_position == 1){
               int pong_pos = get_position_in_byte(pong_row);
               int recket_2_pos = get_position_in_byte(racket_2);
               if(pong_pos == recket_2_pos - 1){
                  pong_direction = DOWN_RIGHT;
               }
               else if(pong_pos == recket_2_pos){
                  pong_direction = DOWN;
               }
               else if(pong_pos == recket_2_pos + 1){
                  pong_direction = DOWN_LEFT;
               }
               else if(pong_pos == recket_2_pos + 2){
                  pong_direction = DOWN_LEFT;
               }
               else {
                  game_over = 1;
               }
            }
         }
         else{
            pong_position -= 1;
            pong_row <<= 1;
         }
         break;
         
      case DOWN_LEFT:
         if(pong_position == 6 || pong_row == 0b10000000){
            if(pong_position < 6 && pong_row == 0b10000000){
               pong_direction = DOWN_RIGHT;
            }
            else if(pong_position == 6){
               int pong_pos = get_position_in_byte(pong_row);
               int recket_pos = get_position_in_byte(racket_1);
               if(pong_pos == recket_pos - 1){
                  pong_direction = UP_RIGHT;
               }
               else if(pong_pos == recket_pos){
                  pong_direction = UP;
               }
               else if(pong_pos == recket_pos + 1){
                  pong_direction = UP_LEFT;
               }
               else if(pong_pos == recket_pos + 2){
                  pong_direction = UP_LEFT;
               }
               else {
                  game_over = 1;
               }
            }
         }
         else{
            pong_position += 1;
            pong_row <<= 1;
         }
         break;
         
      case DOWN_RIGHT:
         if(pong_position == 6 || pong_row == 0b00000001){
            if(pong_position < 6 && pong_row == 0b00000001){
               pong_direction = DOWN_LEFT;
            }
            else if(pong_position == 6){
               int pong_pos = get_position_in_byte(pong_row);
               int recket_pos = get_position_in_byte(racket_1);
               if(pong_pos == recket_pos){
                  pong_direction = UP_RIGHT;
               }
               else if(pong_pos == recket_pos + 1){
                  pong_direction = UP_RIGHT;
               }
               else if(pong_pos == recket_pos + 2){
                  pong_direction = UP;
               }
               else if(pong_pos == recket_pos + 3){
                  pong_direction = UP_LEFT;
               }
               else {
                  game_over = 1;
               }
            }
         }
         else{
            pong_position += 1;
            pong_row >>= 1;
         }
         break;
   }
   render_display_game();
}
///////////////////////////////////////////////////////////////////

void set_game_mode_select(int mode){
   switch(mode){
      case game_mode_0:
         game_mode_select = game_mode_0;
         break;
      case game_mode_1:
         game_mode_select = game_mode_1;
         break;
      case game_mode_2:
         game_mode_select = game_mode_2;
         break;
      case game_mode_3:
         game_mode_select = game_mode_3;
         break;
      case game_mode_4:
         game_mode_select = game_mode_4;
         break;
      default:
         game_mode_select = game_mode_0;
   }
   output_c(game_mode_select);
}

void set_game_mode(int mode){
   switch(mode){
      case game_mode_0:
         game_mode = game_mode_0;
         init_game_0_1();
         break;
      case game_mode_1:
         game_mode = game_mode_1;
         init_game_0_1();
         break;
      case game_mode_2:
         game_mode = game_mode_2;
         init_game();
         break;
      case game_mode_3:
         game_mode = game_mode_3;
         init_game();
         break;
      case game_mode_4:
         game_mode = game_mode_4;
         init_game();
         break;
      default:
         game_mode = game_mode_0;
         init_game_0_1();
   }
}

void main()
{
   setup_adc_ports(NO_ANALOGS);
   setup_adc(ADC_CLOCK_DIV_2);
   setup_psp(PSP_DISABLED);
   setup_spi(SPI_SS_DISABLED);
   setup_timer_0(RTCC_INTERNAL|RTCC_DIV_1);
   setup_timer_1(T1_DISABLED);
   setup_timer_2(T2_DISABLED,0,1);
   setup_comparator(NC_NC_NC_NC);
   setup_vref(FALSE);

   // TODO: USER CODE!!
   init_7219();
   
   game_status = 0;
   game_over = 0;
   game_mode_select = game_mode_0;
   game_mode = game_mode_0;
   set_game_mode_select(game_mode_0);
   set_game_mode(game_mode_0);
   
   animetion();
   render_dotmatrix(disp_start);
   
   
   while(1){
      if(input(START_BTN)){
         while(input(START_BTN));
         if(game_over){
            game_over = 0;
            set_game_mode(game_mode_select);
            animetion();
            game_status = START;
         }
         else if(game_mode_select == game_mode){
            game_status = !game_status;
         }
         else {
            set_game_mode(game_mode_select);
            animetion();
            game_status = START;
         }
      }
      if(input(MODE_BTN)){
         while(input(MODE_BTN));
         if(game_mode_select == game_mode_4){
            game_mode_select = game_mode_0;
         }
         else {
            game_mode_select <<= 1;
         }
         set_game_mode_select(game_mode_select);
      }
      if(input(LEFT_BTN)){
         switch(game_mode){
            case game_mode_0:
               if(racket_1 != 0b11100000){
                  racket_1 <<= 1;
               }
               break;
            case game_mode_1:
               if(racket_1 != 0b00000111){
                  racket_1 >>= 1;
               }
               break;
            case game_mode_2:
               if(racket_1 != 0b11100000){
                  racket_1 <<= 1;
                  racket_2 = racket_1;
               }
               break;
            case game_mode_3:
               switch(pong_direction){
                  case UP: case UP_LEFT: case UP_RIGHT:
                     if(racket_2 != 0b11100000){
                        racket_2 <<= 1;
                     }
                     break;
                  case DOWN: case DOWN_LEFT: case DOWN_RIGHT:
                     if(racket_1 != 0b11100000){
                        racket_1 <<= 1;
                     }
                     break;
               }
               break;
            case game_mode_4:
               if(racket_2 != 0b00000111){
                  racket_2 >>= 1;
               }
               if(racket_1 != 0b11100000){
                  racket_1 <<= 1;
               }
               break;  
         }
      }
      if(input(RIGHT_BTN)){
         switch(game_mode){
            case game_mode_0:
               if(racket_1 != 0b00000111){
                  racket_1 >>= 1;
               }
               break;
            case game_mode_1:
               if(racket_1 != 0b11100000){
                  racket_1 <<= 1;
               }
               break;
            case game_mode_2:
               if(racket_1 != 0b00000111){
                  racket_1 >>= 1;
                  racket_2 = racket_1;
               }
               break;
            case game_mode_3:
               switch(pong_direction){
                  case UP: case UP_LEFT: case UP_RIGHT:
                     if(racket_2 != 0b00000111){
                        racket_2 >>= 1;
                     }
                     break;
                  case DOWN: case DOWN_LEFT: case DOWN_RIGHT:
                     if(racket_1 != 0b00000111){
                        racket_1 >>= 1;
                     }
                     break;
               }
               break;
            case game_mode_4:
               if(racket_2 != 0b11100000){
                  racket_2 <<= 1;
               }
               if(racket_1 != 0b00000111){
                  racket_1 >>= 1;
               }
               break;
         }
      }
      
      if(game_over){
         render_dotmatrix(disp_over);
         game_status = PAUSE;
      }
      else if(game_status == PAUSE){
         render_dotmatrix(disp_start);
      }
      else{
         switch(game_mode){
            case game_mode_0: case game_mode_1:
               process_display_game_0_1();
               render_dotmatrix(disp);
               break;
            case game_mode_2: case game_mode_3: case game_mode_4:
               process_display_game();
               render_dotmatrix(disp);
               break;
         }
      }  
      wait_ms;
   }
}
