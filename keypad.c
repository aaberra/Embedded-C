#include <intrinsics.h>				// Intrinsic functions
#include <stdint.h>			        // Standard integer types
#include <stdbool.h>
#include "oled.h"

void KeypadInit (void){
     P1DIR = COLSMASK;			// Rows input, columns output
     P1OUT = ROWSMASK;
     P1IES = (BIT4|BIT5|BIT6|BIT7);
     P1IE = BIT4|BIT5|BIT6|BIT7;
     P1IFG = 0x00;
       
}

int16_t	KeypadRead (void){
        uint8_t i; 

        int16_t value;				// Value to be returned
        value = NONE;
        P1OUT = 0x0E;                           // First column
        i = P1IN | 0x0E;        
        switch(i){
          case ONE:
            value = ONE;
            
            break;
        case FOUR:
            value = FOUR;
            break;
        case SEVEN:
            value = SEVEN;
            break;
        case STAR:
          value = STAR;
          break;
        default:
          break;
        }
        
        P1OUT = 0x0D;                           // Second Column
        i = P1IN | 0x0D;
        switch(i){
          case TWO:
            value = TWO;
            break;
          case FIVE:
            value = FIVE;
            break;
          case EIGHT:
            value = EIGHT;
            break;
          case ZERO:
            value = ZERO;
            break;
          default:
            break;
        }
        
        P1OUT = 0x0B;                           // Third column
        i = P1IN | 0x0B;
        switch(i){
          case THREE:
            value = THREE;
            break;
          case SIX:
              value = SIX;
              break;
          case NINE:
              value = NINE;
              break;
          case POUND:
            value = POUND;
            break;
          default:
            break;
        }
        P1OUT = 0x07;                   // Fourth Column
        i = P1IN;// | 0x07;
        
        switch(i){
          case LETTER_A:
            value = LETTER_A;
            break;
          case LETTER_B:
            value = LETTER_B;
            break;
          case LETTER_C:
            value = LETTER_C;
            break;
          case LETTER_D:
            value = LETTER_D;
            break;
          default:
            break;
        }
        KeypadInit();   				// Restore default settings
        return value;
}