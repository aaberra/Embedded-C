/* Lib */
#include <stdint.h>
#include <io430xG46x.h>
#include <intrinsics.h>
#include <stdbool.h>
#include "oled.h"
#include "font5x7.h"
//OLED
extern bool u_delay_flag;
extern uint16_t delay_val;
extern char BLOCKS[21];
extern char PAGES[4];

uint16_t tbiv_temp = 0;

// Debounce
#define DEBOUNCE_PERIOD  10
#define SW1             BIT0
#define FALSE 0
#define TRUE 1

#define quarter_sec     8192
#define half_sec        16384
#define LEVEL_UP_CANNON   819
#define LEVEL_UP_ARROW    LEVEL_UP_CANNON/2
#define three_sec       98304
#define CLOCK_FREQ      32768
#define ARROW_START       3
//Keypad
uint16_t Keypad_Value; 


/*Variables */
// Start Menu
unsigned char Start_String[] = "Defend The Ramparts!";
unsigned char Press_A_String[] = "Press A To Start";
unsigned char Time_String[] = "HH : MM : SS";
unsigned char Score_String[] = "High Score: ";
unsigned char High_Score_Print[3][3] = {"0","0","0"};
unsigned char *PrintPtr;

// Win/Lose Strings and control
#define START   0
#define WIN     1
#define LOSE    2

unsigned char String_Sel = START;
unsigned char Lose_String[] = "The Kingdom is Lost!";
unsigned char Win_String[]  = "The Kingdom is Saved!";
unsigned char Score = 0;

// Game Board and Objects
unsigned char game_board[4][21] = {0};          //used to track all game pieces
unsigned char arrow_index[2] = {0,ARROW_START};
unsigned char shield_index[2] = {0,18};

char col = 4, page = 0;

// For Keypad
uint16_t isBouncing = 0; uint8_t button_pressed = FALSE;



// Flags
bool arrow_moved = FALSE;
bool start_screen_flag = FALSE, game_start_flag = FALSE, update_flag = FALSE,
next_arrow_flag = FALSE, game_mode = FALSE,
move_arrow_flag = FALSE;

//Levels & Scores

bool level_up_flag = FALSE;
uint16_t level = 1;
unsigned char Top_Score[3] = {0,0,0}; 
unsigned char Print[3][3] = {"0","0","0"};
char game = 0;

// Win & Lose
bool win_flag = FALSE, lose_flag = FALSE, reset_flag = FALSE;
char Hit_Count = 0;             // Castles Hit
char Block_Count = 0;           // Arrows Blocked

// Shield Movement
#define UP      2
#define DOWN    3
#define LEFT    1
#define RIGHT   4
#define STAY    0


char move = STAY; 

// Variables For block generator
const uint64_t a = 1664525, c = 1013904223, m = 4294967295;
uint32_t last_gen = 1;

/* Function Prototypes */
void Game_Board_Set(unsigned char s[][21]);
void init_timer(void);
void Start_Screen();
void Game_Screen();
void next_arrow();
void Game_Start();
void Move_Arrow();
void Move_Shield(char move);
void Collision_Check(char page, char block);
void Castle_Check();
void fill_print();
void fill_score();

void main(void){
    WDTCTL = WDTPW|WDTHOLD;
    OLED_Init(); KeypadInit();init_timer();
    OLED_Clear();
    start_screen_flag = TRUE;
    asm("EINT");
    while(1){
      //Keypad_Value = KeypadRead();
      
      if(win_flag == TRUE){
        Score = Block_Count;
        //String_Sel = WIN;
        if(lose_flag == TRUE){
           if(game != 3){
              game++;
           }
           else{
           }
           if(Score > Top_Score[game-1]){
              Top_Score[game-1] = Score;
           }
           fill_score();
           reset_flag = TRUE; 
           String_Sel = WIN;
           lose_flag = FALSE;
           game_mode = FALSE;
           win_flag = FALSE;
        }
        //win_flag = FALSE;
      }
      
      if(lose_flag == TRUE){
        reset_flag = TRUE; 
        String_Sel = LOSE;
        lose_flag = FALSE;
        game_mode = FALSE;
      }
      
      if(reset_flag == TRUE){      // Restart Game
         start_screen_flag = TRUE;
         level = 1; Block_Count = 0; 
         reset_flag = FALSE; 
      }

      if(start_screen_flag == TRUE){      // Start Game
         Start_Screen();
         start_screen_flag = FALSE; update_flag = FALSE;
      }
      
      if(game_start_flag == TRUE){
          Game_Start();
          game_start_flag = FALSE; game_mode = TRUE;
          String_Sel = START;
      }
      
      if( move_arrow_flag == TRUE){
          Castle_Check();
          Move_Arrow();
          move_arrow_flag = FALSE; update_flag = TRUE;
      }
      
      if( next_arrow_flag == TRUE){
          next_arrow(last_gen);
          game_board[arrow_index[0]][arrow_index[1]] = ARROW;
          next_arrow_flag = FALSE;
          update_flag = TRUE;
      }
      
      if(move != STAY && game_mode == TRUE){
         Move_Shield(move);
         move = STAY;
         update_flag = TRUE;
      }
      
      if(update_flag == TRUE && game_mode == TRUE){
        Game_Screen();
        update_flag = FALSE;
        Castle_Check();
      }

    }
}

#pragma vector = PORT1_VECTOR
  __interrupt void PORT1_ISR (void){
    Keypad_Value = KeypadRead();
    switch(Keypad_Value){
           case TWO:
                move = UP;
                P1IFG &= 0x00;
                break;
           case LETTER_A:
                String_Sel = START;
                level = 1; Block_Count = 0;  
                game_start_flag = TRUE;
                P1IFG &= 0x00;
                break;
           case FOUR:
                move = LEFT;
                P1IFG &= 0x00;
                break;
           case SIX:
                move = RIGHT;
                P1IFG &= 0x00;
                break;
           case EIGHT:
                move = DOWN;
                P1IFG &= 0x00;
                break;     
    }

    
  }

 //Basic Timer ISR
/*#pragma vector = BASICTIMER_VECTOR
__interrupt void BASICTIMER_ISR (void){
    if ( isBouncing > 0){
         isBouncing = isBouncing - 1;
    }
}
*/
#pragma vector = TIMERA0_VECTOR
__interrupt void TIMERA0_ISR (void){
      TACCTL0 &= ~CCIFG;
      //__low_power_mode_off_on_exit();
}

#pragma vector = TIMERB1_VECTOR
__interrupt void TIMERB1_ISR (void){     // ISR for TACCRn CCIFG and TAIFG
    tbiv_temp = TBIV;                    // necessary because accessing TAIV resets it
    switch (tbiv_temp){                  // Efficient switch-implementation
        case TBIV_TBCCR1:
            if(game_mode == TRUE){
               next_arrow_flag = TRUE; 
               update_flag = TRUE;
            }
            TBCCR1 += half_sec - ((level - 1)*LEVEL_UP_CANNON);
            TBCCTL1 &= ~CCIFG;        //clear TACCR2 flag
            break;    
        case TBIV_TBCCR2:
            if(game_mode == TRUE){
                move_arrow_flag = TRUE;
                update_flag = TRUE;
            }
            TBCCR2 += quarter_sec - ((level - 1)*LEVEL_UP_ARROW);
            TBCCTL2 &= ~CCIFG;        //clear TACCR2 flag
            break; 
    }
    return;
}

void Start_Screen(){
    OLED_Clear();
    col = BLOCKS[1] ;page = 0;
    switch(String_Sel){
        case START:
            OLED_Write_String(Start_String,page,col);
            break;
        case WIN:
            OLED_Write_String(Win_String,page,col);
            break;
        case LOSE:
            OLED_Write_String(Lose_String,page,col);
            break;
    }
    page = 1; col = BLOCKS[3];
    OLED_Write_String(Press_A_String,page,col);
    page = 2; col = BLOCKS[5];
    OLED_Write_String(Time_String,page,col);
    page = 3; col = BLOCKS[2];
    OLED_Write_String(Score_String,page,col);
    page = 3; col = BLOCKS[13];
    OLED_Write_String(High_Score_Print[0],page,col);
    
    page = 3; col = BLOCKS[16];
    OLED_Write_String(High_Score_Print[1],page,col);
    
    page = 3; col = BLOCKS[19];
    OLED_Write_String(High_Score_Print[2],page,col);
}

void Game_Start(){
     char i,j;
     OLED_Clear();
     for(i= PAGE_START;i<MAX_PAGE;i++){
        for(j = 0; j<22;j++){
            game_board[i][j] = 0;
        } 
     }
     Game_Board_Set(game_board);
     OLED_Frame(game_board,Print);
}

void Game_Screen(){
     //Game_Board_Set(game_board);
     fill_print();
     OLED_Frame(game_board,Print);
}

void Game_Board_Set(unsigned char s[][21]){
    char i;
    
    for(i= PAGE_START;i<MAX_PAGE;i++){ 
        game_board[i][2] = CANNON;
        game_board[i][19] = CASTLE;
        game_board[i][20] = CASTLE;
    }
    fill_print();

    game_board[PAGE_START][0] = NUM; game_board[PAGE_START][1] = NUM;
    game_board[2][0] = NUM; game_board[2][1] = NUM;
    game_board[3][0] = NUM; game_board[3][1] = NUM; 
    game_board[shield_index[0]][shield_index[1]] = SHIELD;
}

void init_timer(void){
    
    TBCCR1 = half_sec;              //compare value for first interupt
    TBCCTL1 = CCIE;                 //enable interupt
    
    TBCCR2 = quarter_sec;           //compare value for first interupt
    TBCCTL2 = CCIE;                 //enable interupt
    
    
    TBCTL = TBSSEL0|MC_2|ID0|TACLR;  
    
    /*//Init Basic Timer - for Debouncing
    BTCTL  = (BTSSEL|(BTDIV));      // [BTIP = 000 => aclk/256/2]
    BTCNT1 = 0; BTCNT2 = 0;
    IE2   |= BTIE;                  //Interrupt Enable
    */
    return;
}

void next_arrow(uint32_t x){
      uint64_t nextgen; char block_gen;
      // LCG Calculation
      nextgen = ((a * x) + c) % m;  // 64 bits are required for a 32bit multiplier
      nextgen = (uint32_t)nextgen;     // cast as a 32 bit integer
      block_gen = nextgen % 4;
      last_gen = (uint32_t)nextgen;
      arrow_index[0] = block_gen;
}

void Castle_Check(){
  char i;
  for(i = PAGE_START;i<MAX_PAGE;i++){
    if(game_board[i][20] != CASTLE){
       lose_flag = TRUE;
    }
    }
  }

void Move_Arrow(){
     char i,j;
     for(i=PAGE_START;i<MAX_PAGE;i++){
         for(j=3;j < 21;j++){
           Collision_Check(i, j);                      // Check for Shield or Castle
           if (arrow_moved == FALSE){
             switch (game_board[i][j]){
             case ARROW:
                  game_board[i][j+1] = game_board[i][j];
                  game_board[i][j] = CLEAR;

                  arrow_moved = TRUE;
                  break;
             }
         }
         else arrow_moved = 0;
         }
     }
}

void Move_Shield(char move){
  switch(move){
      case UP:
        if( shield_index[0] != 0){
            shield_index[0] -= 1;
            game_board[shield_index[0]][shield_index[1]] = game_board[shield_index[0]+1][shield_index[1]];
            game_board[shield_index[0]+1][shield_index[1]] = CLEAR;
        }
        break;
      case DOWN:
        if( shield_index[0] != 3){
            shield_index[0] += 1;
            game_board[shield_index[0]][shield_index[1]] = game_board[shield_index[0]-1][shield_index[1]];
            game_board[shield_index[0]-1][shield_index[1]] = CLEAR;
        }
        break;
      case LEFT:
        if( shield_index[1] > 4){
            shield_index[1] -= 1;
            game_board[shield_index[0]][shield_index[1]] = game_board[shield_index[0]][shield_index[1]+1];
            game_board[shield_index[0]][shield_index[1]+1] = CLEAR;
        }
        break;
      case RIGHT:
        if( shield_index[1] < 18){
            shield_index[1] += 1;
            game_board[shield_index[0]][shield_index[1]] = game_board[shield_index[0]][shield_index[1]-1];
            game_board[shield_index[0]][shield_index[1]-1] = CLEAR;
        }
        break;
      default:
        break;
 
  }

}

void Collision_Check(char page, char block){
     // arrow to castle
  switch(game_board[page][block]){
    case ARROW: 
      if(game_board[page][block+1] == SHIELD){
         game_board[page][block] = CLEAR;
         Block_Count++;
         if(level < 10){
            level += 1;
         }
         else{
            win_flag = TRUE;
         }
      }
      else if(game_board[page][block+1] == CASTLE){
         game_board[page][block+1] = CLEAR;
         game_board[page][block] = CLEAR;
         arrow_moved = TRUE;
      }
      else if((block+1) == 20){
         game_board[page][block] = CLEAR; 
         arrow_moved = TRUE;
      }
      break;
    case SHIELD:
     // shield down
      // shield up
      //shield left
      // shield right??
     break;
  }

}

void fill_score(){
     char i;
      for( i=0;i<3;i++){
       if(i == 0){                              //TOP SCORE
          High_Score_Print[i][0] =  '0' + (Top_Score[0]/10); 
          High_Score_Print[i][1] = '0' +(Top_Score[0] % 10); 
          High_Score_Print[i][2] =  '\0';
       }
      if(i == 1){                               //BLOCK COUNT
          High_Score_Print[i][0] =  '0' + (Top_Score[1]/10); 
          High_Score_Print[i][1] = '0' +(Top_Score[1]% 10); 
          High_Score_Print[i][2] =  '\0';
       }
      if(i == 2){                               //LEVEL
          High_Score_Print[i][0] =  '0' + (Top_Score[2]/10); 
          High_Score_Print[i][1] = '0' +(Top_Score[2]% 10); 
          High_Score_Print[i][2] =  '\0';
       }
     }

}

void fill_print(){
     char i,j,high;
     
     for(j=1;j<3;j++){
       if ((Top_Score[j] > Top_Score[j-1])){
          high = j;
       }
       else if(j == 1){
         high = 0;
       }
     }
     
     for( i=0;i<3;i++){
       if(i == 0){                              //TOP SCORE
          Print[i][0] =  '0' + (Top_Score[high]/10); 
          Print[i][1] = '0' +(Top_Score[high] % 10); 
          Print[i][2] =  '\0';
       }
      if(i == 1){                               //BLOCK COUNT
          Print[i][0] =  '0' + (Block_Count/10); 
          Print[i][1] = '0' +(Block_Count% 10); 
          Print[i][2] =  '\0';
       }
      if(i == 2){                               //LEVEL
          Print[i][0] =  '0' + (level/10); 
          Print[i][1] = '0' +(level % 10); 
          Print[i][2] =  '\0';
       }
     }
 }