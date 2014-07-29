// Adapted by Aaron Hunter from NHD app notes at http://www.newhavendisplay.com/app_notes/OLED_2_23_12832.txt
// Date Created: 10/15/2012
// Date Modified: 10/20/2012

#include <stdlib.h>
#include "oled.h"
#include "font5x7.h"

/*VAR*/

static uint8_t Brightness; 
const float usec_cycle = .032768; 

    
// Waits usec microseconds before returning	
int OLED_udelay(int usec){
   TACCR0 = 1;
   TACCTL0 = CCIE;
   TACTL = TASSEL_2 | ID_0 | MC_2 | TACLR;
   //__low_power_mode_3();
   return(0);
}

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//  Write Character (5x7)
//
//    s: Ascii
//    p: Start Page
//    c: Start Column
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
void OLED_Write_Char(unsigned char s, unsigned char p, unsigned char c){
  char j;
  
  OLED_Set_Start_Page(p);
  OLED_Set_Start_Column(c);
  for(j=0;j<5;j++){
    oled_Data(Ascii_1[s-FONT_OFFSET][j]);
  }
}


//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//  Write String
//
//    s: String
//    p: Start Page
//    c: Start Column
//    * Must be Null terminated string
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
void OLED_Write_String(unsigned char *s, unsigned char p, unsigned char c)
{
  char i = 0;
  while( s[i] != '\0'){
      OLED_Write_Char(s[i],p,c);
      i++;
      c +=6;
  }
}

// Draws a vertical line
// column: column position to draw line. ranges from 4 - 131
//
void OLED_Write_Vert_Line(uint8_t column){
  char i;
  for(i=PAGE_START;i<MAX_PAGE;i++){
      OLED_Set_Start_Page(i);
      OLED_Set_Start_Column(column);
      oled_Data(0xFF);
  }
    
}


//--------------------------------------------------------------------------
//--------------------------------------------------------------------------
void oled_Command(unsigned char Data)
{
  DPORT_OUT = Data;
  OLED_SET_CMD();					// Set DC, RW, nRES pins as necessary
  OLED_SET_E();					// Pull E high
  OLED_udelay(100);				// Delay for 100 us
  OLED_RESET_E();				// Pull E low
  OLED_udelay(100);				// Delay for 100 us
}
//--------------------------------------------------------------------------
//--------------------------------------------------------------------------

void oled_Data(unsigned char Data)
{
  DPORT_OUT = Data;                             // Data to be written to the display
  OLED_SET_DATA();				// Set DC, RW, nRES pins as necessary
  OLED_SET_E(); 				// Pull E high
  OLED_udelay(100);				// Delay for 100 us
  OLED_RESET_E();				// Pull E low
  OLED_udelay(100);				// Delay for 100 us
}
//--------------------------------------------------------------------------
//--------------------------------------------------------------------------

void OLED_Set_Start_Column(unsigned char d)
{
  oled_Command(0x00+d%16);		// Set Lower Column Start Address for Page Addressing Mode
                                          //   Default => 0x00
  oled_Command(0x10+d/16);		// Set Higher Column Start Address for Page Addressing Mode
                                          //   Default => 0x10
}
//--------------------------------------------------------------------------

void OLED_Set_Addressing_Mode(unsigned char d)
{
  oled_Command(0x20);			// Set Memory Addressing Mode
  oled_Command(d);			//   Default => 0x02
                                          //     0x00 => Horizontal Addressing Mode
                                          //     0x01 => Vertical Addressing Mode
                                          //     0x02 => Page Addressing Mode
}
//--------------------------------------------------------------------------

void OLED_Set_Column_Address(unsigned char a, unsigned char b)
{
  oled_Command(0x21);			// Set Column Address
  oled_Command(a);			//   Default => 0x00 (Column Start Address)
  oled_Command(b);			//   Default => 0x83 (Column End Address)
}
//--------------------------------------------------------------------------

void OLED_Set_Page_Address(unsigned char a, unsigned char b)
{
  oled_Command(0x22);			// Set Page Address
  oled_Command(a);			//   Default => 0x00 (Page Start Address)
  oled_Command(b);			//   Default => 0x07 (Page End Address)
}
//--------------------------------------------------------------------------

void OLED_Set_Start_Line(unsigned char d)
{
  oled_Command(0x40|d);			// Set Display Start Line
                                          //   Default => 0x40 (0x00)
}
//--------------------------------------------------------------------------

void OLED_Set_Contrast_Control(unsigned char d)
{
  oled_Command(0x81);			// Set Contrast Control for Bank 0
  oled_Command(d);			//   Default => 0x80
}
//--------------------------------------------------------------------------

void OLED_Set_Area_Brightness(unsigned char d)
{
  oled_Command(0x82);			// Set Brightness for Area Color Banks
  oled_Command(d);			//   Default => 0x80
}
//--------------------------------------------------------------------------

void OLED_Set_Segment_Remap(unsigned char d)
{
  oled_Command(0xA0|d);			// Set Segment Re-Map
                                          //   Default => 0xA0
                                          //     0xA0 (0x00) => Column Address 0 Mapped to SEG0
                                          //     0xA1 (0x01) => Column Address 0 Mapped to SEG131
}
//--------------------------------------------------------------------------

void OLED_Set_Entire_Display(unsigned char d)
{
  oled_Command(0xA4|d);			// Set Entire Display On / Off
                                          //   Default => 0xA4
                                          //     0xA4 (0x00) => Normal Display
                                          //     0xA5 (0x01) => Entire Display On
}
//--------------------------------------------------------------------------

void OLED_Set_Inverse_Display(unsigned char d)
{
  oled_Command(0xA6|d);			// Set Inverse Display On/Off
                                          //   Default => 0xA6
                                          //     0xA6 (0x00) => Normal Display
                                          //     0xA7 (0x01) => Inverse Display On
}
//--------------------------------------------------------------------------

void OLED_Set_Multiplex_Ratio(unsigned char d)
{
  oled_Command(0xA8);			// Set Multiplex Ratio
  oled_Command(d);			//   Default => 0x3F (1/64 Duty)
}
//--------------------------------------------------------------------------

void OLED_Set_Dim_Mode(unsigned char a, unsigned char b)
{
  oled_Command(0xAB);			// Set Dim Mode Configuration
  oled_Command(0X00);			//           => (Dummy Write for First Parameter)
  oled_Command(a);			//   Default => 0x80 (Contrast Control for Bank 0)
  oled_Command(b);			//   Default => 0x80 (Brightness for Area Color Banks)
  oled_Command(0xAC);			// Set Display On in Dim Mode
}
//--------------------------------------------------------------------------

void OLED_Set_Master_Config(unsigned char d)
{
  oled_Command(0xAD);			// Set Master Configuration
  oled_Command(0x8E|d);			//   Default => 0x8E
                                          //     0x8E (0x00) => Select External VCC Supply
                                          //     0x8F (0x01) => Select Internal DC/DC Voltage Converter
}
//--------------------------------------------------------------------------

void OLED_Set_Display_On_Off(unsigned char d)	
{
  oled_Command(0xAE|d);			// Set Display On/Off
                                          //   Default => 0xAE
                                          //     0xAE (0x00) => Display Off
                                          //     0xAF (0x01) => Display On
}
//--------------------------------------------------------------------------

void OLED_Set_Start_Page(unsigned char d)
{
  oled_Command(0xB0|d);			// Set Page Start Address for Page Addressing Mode
                                          //   Default => 0xB0 (0x00)
}
//--------------------------------------------------------------------------

void OLED_Set_Common_Remap(unsigned char d)
{
  oled_Command(0xC0|d);			// Set COM Output Scan Direction
                                          //   Default => 0xC0
                                          //     0xC0 (0x00) => Scan from COM0 to 63
                                          //     0xC8 (0x08) => Scan from COM63 to 0
}
//--------------------------------------------------------------------------

void OLED_Set_Display_Offset(unsigned char d)
{
  oled_Command(0xD3);			// Set Display Offset
  oled_Command(d);			//   Default => 0x00
}
//--------------------------------------------------------------------------

void OLED_Set_Display_Clock(unsigned char d)
{
  oled_Command(0xD5);			// Set Display Clock Divide Ratio / Oscillator Frequency
  oled_Command(d);			//   Default => 0x70
                                          //     D[3:0] => Display Clock Divider
                                          //     D[7:4] => Oscillator Frequency
}
//--------------------------------------------------------------------------

void OLED_Set_Area_Color(unsigned char d)
{
  oled_Command(0xD8);			// Set Area Color Mode On/Off & Low Power Display Mode
  oled_Command(d);			//   Default => 0x00 (Monochrome Mode & Normal Power Display Mode)
}
//--------------------------------------------------------------------------

void OLED_Set_Precharge_Period(unsigned char d)
{
  oled_Command(0xD9);			// Set Pre-Charge Period
  oled_Command(d);			//   Default => 0x22 (2 Display Clocks [Phase 2] / 2 Display Clocks [Phase 1])
                                          //     D[3:0] => Phase 1 Period in 1~15 Display Clocks
                                          //     D[7:4] => Phase 2 Period in 1~15 Display Clocks
}
//--------------------------------------------------------------------------

void OLED_Set_Common_Config(unsigned char d)
{
  oled_Command(0xDA);			// Set COM Pins Hardware Configuration
  oled_Command(0x02|d);			//   Default => 0x12 (0x10)
                                          //     Alternative COM Pin Configuration
                                          //     Disable COM Left/Right Re-Map
}
//--------------------------------------------------------------------------

void OLED_Set_VCOMH(unsigned char d)
{
	oled_Command(0xDB);			// Set VCOMH Deselect Level
	oled_Command(d);			//   Default => 0x34 (0.77*VCC)
}
//--------------------------------------------------------------------------

void OLED_Set_Read_Modify_Write(unsigned char d)
{
  oled_Command(0xE0|d);			// Set Read Modify Write Mode
                                          //   Default => 0xE0
                                          //     0xE0 (0x00) => Enter Read Modify Write
                                          //     0xEE (0x0E) => Exit Read Modify Write
}
//--------------------------------------------------------------------------

void OLED_Set_NOP()
{
  oled_Command(0xE3);			// Command for No Operation
}

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//  Bank Color & Look Up Table Setting (Partial Screen)
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
void OLED_Set_LUT(unsigned char a, unsigned char b, unsigned char c, unsigned char d)
{
  oled_Command(0x91);			// Define Look Up Table of Area Color
  oled_Command(a);			//   Define Bank 0 Pulse Width
  oled_Command(b);			//   Define Color A Pulse Width
  oled_Command(c);			//   Define Color B Pulse Width
  oled_Command(d);			//   Define Color C Pulse Width
}

void OLED_Set_Bank_Color()
{
  oled_Command(0x92);			// Define Area Color for Bank 1~16 (Page 0)
  oled_Command(0x00);			//   Define Bank 1~4 as Color A
  oled_Command(0x55);			//   Define Bank 5~8 as Color B
  oled_Command(0xAA);			//   Define Bank 9~12 as Color C
  oled_Command(0xFF);			//   Define Bank 13~16 as Color D

  oled_Command(0x93);			// Define Area Color for Bank 17~32 (Page 1)
  oled_Command(0xFF);			//   Define Bank 17~32 as Color D
  oled_Command(0xFF);
  oled_Command(0xFF);
  oled_Command(0xFF);
}

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//  Show Regular Pattern (Full Screen)
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
void OLED_Fill_RAM(unsigned char Data)
{
  unsigned char i,j;

  for(i=PAGE_START; i< PAGE_END+1;i++)
  {
    OLED_Set_Start_Page(i);
    OLED_Set_Start_Column(0x00);

    for(j=COLUMN_START;j<COLUMN_END+1;j++)
    {
      oled_Data(Data);
    }
  }
}

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//  Show Checkboard (Full Screen)
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
void OLED_Checkerboard(){
  unsigned char i,j;
	
  for(i=PAGE_START; i<PAGE_END+1;i++){
    OLED_Set_Start_Page(i);
    OLED_Set_Start_Column(COLUMN_START);

    for(j=0;j<66;j++){
      oled_Data(0x55);
      oled_Data(0xaa);
    }
  }
}

char BLOCKS[21] = {4,10,16,22,28,34,40,46,52,58,64,70,
                   76,82,88,94,100,106,112,118,124};
char PAGES[4] = {0,8,16,24};
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//  Show Frame (Full Screen)
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
unsigned char test1[] = "15",test2[] = "2",test3[] = "3";
void OLED_Frame(unsigned char s[][21], unsigned char Print[][3]){
     char i,j,k;

     for(i=PAGE_START;i<MAX_PAGE;i++){
         OLED_Set_Start_Page(i);
       for(j=0;j < 22;j++){
           OLED_Set_Start_Column(BLOCKS[j]);
           switch (s[i][j]){
             case ARROW:
                  for(k = 0; k < 6;k++){
                      oled_Data(game_pieces[ARROW-1][k]);
                  }
                  break;
             case CANNON:
                  for(k = 0; k < 6;k++){
                      oled_Data(game_pieces[CANNON-1][k]);
                  }
                  break;
             case CASTLE:
                  for(k = 0; k < 6;k++){
                      oled_Data(game_pieces[CASTLE-1][k]);
                  }
                  break;
             case NUM:
               if(j < 19){
                 if(s[i][j+1] == NUM){
                   if(i == 0){
                     OLED_Write_String(Print[0],i,BLOCKS[j]);
                   }
                   if(i == 2){
                      OLED_Write_String(Print[1],i,BLOCKS[j]);
                   }
                   if(i == 3){
                      OLED_Write_String(Print[2],i,BLOCKS[j]);
                   }
                 }
               }
                    /*for(k=0;k<5;k++){
                        oled_Data(Ascii_1['5'-FONT_OFFSET][k]);
                    }*/
                  break;
             case SHIELD:
                  for(k = 0; k < 6;k++){
                      oled_Data(game_pieces[SHIELD-1][k]);
                  }
                  break;
             case CLEAR:
                  for(k = 0; k < 6;k++){
                      oled_Data(0x00);
                  }
                  s[i][j] = 0;
                  break;                  
              default:
 //                 OLED_udelay(100);
                  break;
           }
       }
     }
}

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//  Show Regular Pattern (Partial or Full Screen)
//
//    a: Start Page
//    b: End Page
//    c: Start Column
//    d: Total Columns
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
void OLED_Fill_Block(unsigned char Data, unsigned char a, unsigned char b, unsigned char c, unsigned char d)
{
}

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//  Show Pattern (Partial or Full Screen)
//
//    a: Start Page
//    b: End Page
//    c: Start Column
//    d: Total Blocks
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
void OLED_Show_Pattern(unsigned char *Data_Pointer, unsigned char a, unsigned char b, unsigned char c, unsigned char d){
    char i,j,k;
    for(i = a; i < b ;i++){
      OLED_Set_Start_Page(i);   
      for(j = c;j < (c+(d*8));){
          OLED_Set_Start_Column(j);
          for(k = 0;k<7;k++){
            oled_Data(game_pieces[1][k]);
          }
          oled_Data(0x00);
          j += 8;
        }
    }
}

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//  Initialization
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
void OLED_Init(){
  unsigned char i;	
  Brightness = 0xBF;

  DPORT_DIR |= 0xFF;
  OLED_SET_CMD();                                // Sets the CTL_PORT to sensible values
  CTL_DIR |= (DC | RW | E | nRES);              // Enable DC, RW, E, and nRES pins to be outputs
  OLED_RESET_nRES();
  for(i=0;i<200;i++){
    OLED_udelay(200);
  }
  OLED_SET_nRES();  

  OLED_Set_Display_On_Off(0x00);		// Display Off (0x00/0x01)
  OLED_Set_Display_Clock(0x20);		// Set Clock as 160 Frames/Sec
  OLED_Set_Multiplex_Ratio(0x1F);		// 1/32 Duty (0x0F~0x3F)
  OLED_Set_Display_Offset(0x00);		// Shift Mapping RAM Counter (0x00~0x3F)
  OLED_Set_Start_Line(0x00);			// Set Mapping RAM Display Start Line (0x00~0x3F)
  OLED_Set_Master_Config(0x00);		// Disable Embedded DC/DC Converter (0x00/0x01)
  OLED_Set_Area_Color(0x05);			// Set Monochrome & Low Power Save Mode
  OLED_Set_Addressing_Mode(0x02);		// Set Page Addressing Mode (0x00/0x01/0x02)
  OLED_Set_Segment_Remap(0x01);		// Set SEG/Column Mapping (0x00/0x01)
  OLED_Set_Common_Remap(0x08);			// Set COM/Row Scan Direction (0x00/0x08)
  OLED_Set_Common_Config(0x10);		// Set Alternative Configuration (0x00/0x10)
  OLED_Set_LUT(0x3F,0x3F,0x3F,0x3F);		// Define All Banks Pulse Width as 64 Clocks
  OLED_Set_Contrast_Control(Brightness);	// Set SEG Output Current
  OLED_Set_Area_Brightness(Brightness);	// Set Brightness for Area Color Banks
  OLED_Set_Precharge_Period(0xD2);		// Set Pre-Charge as 13 Clocks & Discharge as 2 Clock
  OLED_Set_VCOMH(0x08);			// Set VCOM Deselect Level
  OLED_Set_Entire_Display(0x00);		// Disable Entire Display On (0x00/0x01)
  OLED_Set_Inverse_Display(0x00);		// Disable Inverse Display On (0x00/0x01)

  OLED_Fill_RAM(0x00);				// Clear Screen

  OLED_Set_Display_On_Off(0x01);		// Display On (0x00/0x01)
}

void OLED_Clear(){
  unsigned char i,j;
  
  for(i=PAGE_START; i<PAGE_END+1;i++){
    OLED_Set_Start_Page(i);
    OLED_Set_Start_Column(COLUMN_START);
    for(j=0;j<132;j++){
      oled_Data(0x00);
    }
  }
}
