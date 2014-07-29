// Sections used from 
// Author: Aaron Hunter
// ECE 447 Graphical Display library
// Configured in Parallel 6800-interface mode
// Commands/data latched by OLED on falling edge of E pin

#include <stdint.h>
#include <io430xG46x.h>			       // Specific device
#include <stdbool.h>

// OLED display constants
#define  COLUMN_START   (4)
#define  COLUMN_END     (131)
#define  PAGE_START     (0)
#define  PAGE_END       (3) 

#define  MAX_COLUMN     (COLUMN_END - COLUMN_START + 1)
#define  MAX_ROW        (32)
#define  MAX_PAGE       (PAGE_END - PAGE_START + 1)

/* Defines */
#define CLOCK_FREQ      32768
#define FALSE   0
#define TRUE    1

/*For Game Board */ 
#define ARROW   1
#define CASTLE  2
#define CANNON  3
#define SHIELD  4
#define NUM  5
#define CLEAR 6

static unsigned char game_pieces[][6] = {
        {0x3E, 0x08, 0x08, 0x2A, 0x1C, 0x08}, // Arrow 
        {0x7E, 0x44, 0x76, 0x76, 0x44,0x7E},       // Castle
        {0xFF, 0x42, 0x24, 0x18, 0x00,0x00}, // Cannon
        {0xFF, 0xFF, 0x00, 0x00, 0x00,0x00}     // Shield
};



//----------------------- OLED display interface ports -----------------------
#define DPORT_OUT       P4OUT                   // Output data port for DB0-DB7
#define CTL_PORT        P7OUT	                    // Control port for pins
                                                // D/C, R/W, E, and /RES

#define DPORT_DIR       P4DIR  		        // Direction register for DPORT_OUT
#define CTL_DIR         P7DIR   	        // Direction register for CTL_PORT


//--------------------------- OLED control bits ------------------------------
#define DC              BIT0                    
#define RW              BIT1                   
#define E               BIT2                   

#define nRES            BIT3                    

// ------------ Pseudo-calls for reading and writing data to OLED -------------
#define OLED_SET_CMD()           CTL_PORT = nRES;
#define OLED_SET_DATA()          CTL_PORT |= DC;

#define OLED_SET_E()             CTL_PORT |= E;
#define OLED_RESET_E()           CTL_PORT &= ~E;

#define OLED_RESET_nRES()        CTL_PORT &= ~nRES;
#define OLED_SET_nRES()          CTL_PORT |= nRES;

int OLED_udelay(int usec);
void oled_Command(unsigned char Data);
void oled_Data(unsigned char Data);
void OLED_Set_Start_Column(unsigned char d);
void OLED_Set_Addressing_Mode(unsigned char d);
void OLED_Set_Column_Address(unsigned char a, unsigned char b);
void OLED_Set_Page_Address(unsigned char a, unsigned char b);
void OLED_Set_Start_Line(unsigned char d);
void OLED_Set_Contrast_Control(unsigned char d);
void OLED_Set_Area_Brightness(unsigned char d);
void OLED_Set_Segment_Remap(unsigned char d);
void OLED_Set_Entire_Display(unsigned char d);
void OLED_Set_Inverse_Display(unsigned char d);
void OLED_Set_Multiplex_Ratio(unsigned char d);
void OLED_Set_Dim_Mode(unsigned char a, unsigned char b);
void OLED_Set_Master_Config(unsigned char d);
void OLED_Set_Display_On_Off(unsigned char d);	
void OLED_Set_Start_Page(unsigned char d);
void OLED_Set_Common_Remap(unsigned char d);
void OLED_Set_Display_Offset(unsigned char d);
void OLED_Set_Display_Clock(unsigned char d);
void OLED_Set_Area_Color(unsigned char d);
void OLED_Set_Precharge_Period(unsigned char d);
void OLED_Set_Common_Config(unsigned char d);
void OLED_Set_VCOMH(unsigned char d);
void OLED_Set_Read_Modify_Write(unsigned char d);
void OLED_Set_NOP();
void OLED_Set_LUT(unsigned char a, unsigned char b, unsigned char c, unsigned char d);
void OLED_Set_Bank_Color();
void OLED_Fill_RAM(unsigned char Data);
void OLED_Fill_Block(unsigned char Data, unsigned char a, unsigned char b, unsigned char c, unsigned char d);
void OLED_Checkerboard();
void OLED_Frame(unsigned char s[][21], unsigned char Print[][3]);
void OLED_Write_Char(unsigned char b, unsigned char c, unsigned char d);
void OLED_Write_String(unsigned char *Data_Pointer, unsigned char b, unsigned char c);
void OLED_Show_Pattern(unsigned char *Data_Pointer, unsigned char a, unsigned char b, unsigned char c, unsigned char d);
void OLED_Write_Vert_Line(uint8_t column);

void OLED_Init();


void OLED_Clear();

// FOR KEYPAD
#define TRUE                    1
#define FALSE                   0

#define ONE                     0xEE
#define TWO                     0xED
#define THREE                   0xEB
#define LETTER_A                0xE7
#define FOUR                    0xDE
#define FIVE                    0xDD
#define SIX                     0xDB
#define LETTER_B                0xD7
#define SEVEN                   0xBE
#define EIGHT                   0xBD
#define NINE                    0xBB
#define LETTER_C                0xB7
#define STAR                    0x7E
#define ZERO                    0x7D
#define POUND                   0x7B
#define LETTER_D                0x77

#define	NONE	                0xF0

#define	ROWSMASK		0xF0
#define	COLSMASK		0x0F

// Function prototypes
void KeypadInit (void);
int16_t	KeypadRead (void);
