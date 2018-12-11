#include "lcd_display.h"


extern LCD_HandleTypeDef hlcd;

#define ASCII_CHAR_0                  0x30  /* 0 */
#define ASCII_CHAR_AT_SYMBOL          0x40  /* @ */
#define ASCII_CHAR_LEFT_OPEN_BRACKET  0x5B  /* [ */
#define ASCII_CHAR_APOSTROPHE         0x60  /* ` */
#define ASCII_CHAR_LEFT_OPEN_BRACE    0x7B  /* ( */

const uint16_t CapLetterMap[26]=
    {
        /* A      B      C      D      E      F      G      H      I  */
        0xFE00, 0x6714, 0x1D00, 0x4714, 0x9D00, 0x9C00, 0x3F00, 0xFA00, 0x0014,
        /* J      K      L      M      N      O      P      Q      R  */
        0x5300, 0x9841, 0x1900, 0x5A48, 0x5A09, 0x5F00, 0xFC00, 0x5F01, 0xFC01,
        /* S      T      U      V      W      X      Y      Z  */
        0xAF00, 0x0414, 0x5b00, 0x18C0, 0x5A81, 0x00C9, 0x0058, 0x05C0
    };

const uint16_t NumberMap[10]=
    {
        /* 0      1      2      3      4      5      6      7      8      9  */
        0x5F00,0x4200,0xF500,0x6700,0xEa00,0xAF00,0xBF00,0x04600,0xFF00,0xEF00
    };

uint32_t Digit[4];     /* Digit frame buffer */

static void Convert(uint8_t* Char, Point_Typedef Point, DoublePoint_Typedef Colon);
static void WriteChar(uint8_t* ch, Point_Typedef Point, DoublePoint_Typedef Colon, DigitPosition_Typedef Position);

static void Convert(uint8_t* Char, Point_Typedef Point, DoublePoint_Typedef Colon)
{
  uint16_t ch = 0 ;
  uint8_t loop = 0, index = 0;
  
  switch (*Char)
    {
    case ' ' :
      ch = 0x00;
      break;

    case '*':
      ch = C_STAR;
      break;

    case '(' :
      ch = C_OPENPARMAP;
      break;

    case ')' :
      ch = C_CLOSEPARMAP;
      break;
      
    case 'd' :
      ch = C_DMAP;
      break;
    
    case 'm' :
      ch = C_MMAP;
      break;
    
    case 'n' :
      ch = C_NMAP;
      break;

    case '-' :
      ch = C_MINUS;
      break;

    case '+' :
      ch = C_PLUS;
      break;

    case '/' :
      ch = C_SLATCH;
      break;  
      
    case '%' :
      ch = C_PERCENT_2; 
      break;
    case 255 :
      ch = C_FULL;
      break ;
    
    case '0':
    case '1':
    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
    case '7':
    case '8':
    case '9':      
      ch = NumberMap[*Char - ASCII_CHAR_0];    
      break;
          
    default:
      /* The character Char is one letter in upper case*/
      if ( (*Char < ASCII_CHAR_LEFT_OPEN_BRACKET) && (*Char > ASCII_CHAR_AT_SYMBOL) )
      {
        ch = CapLetterMap[*Char - 'A'];
      }
      /* The character Char is one letter in lower case*/
      if ( (*Char < ASCII_CHAR_LEFT_OPEN_BRACE) && ( *Char > ASCII_CHAR_APOSTROPHE) )
      {
        ch = CapLetterMap[*Char - 'a'];
      }
      break;
  }
       
  /* Set the digital point can be displayed if the point is on */
  if (Point == POINT_ON)
  {
    ch |= 0x0002;
  }

  /* Set the "COL" segment in the character that can be displayed if the colon is on */
  if (Colon == DOUBLEPOINT_ON)
  {
    ch |= 0x0020;
  }    

  for (loop = 12,index=0 ;index < 4; loop -= 4,index++)
  {
    Digit[index] = (ch >> loop) & 0x0f; /*To isolate the less significant digit */
  }
}

static void WriteChar(uint8_t* ch, Point_Typedef Point, DoublePoint_Typedef Colon, DigitPosition_Typedef Position)
{
  uint32_t data =0x00;
  /* To convert displayed character in segment in array digit */
  Convert(ch, (Point_Typedef)Point, (DoublePoint_Typedef)Colon);

  switch (Position)
  {
    /* Position 1 on LCD (Digit1)*/
    case LCD_DIGIT_POSITION_1:
      data = ((Digit[0] & 0x1) << LCD_SEG0_SHIFT) | (((Digit[0] & 0x2) >> 1) << LCD_SEG1_SHIFT)
          | (((Digit[0] & 0x4) >> 2) << LCD_SEG22_SHIFT) | (((Digit[0] & 0x8) >> 3) << LCD_SEG23_SHIFT);
      HAL_LCD_Write(&hlcd, LCD_DIGIT1_COM0, LCD_DIGIT1_COM0_SEG_MASK, data); /* 1G 1B 1M 1E */
      
      data = ((Digit[1] & 0x1) << LCD_SEG0_SHIFT) | (((Digit[1] & 0x2) >> 1) << LCD_SEG1_SHIFT)
          | (((Digit[1] & 0x4) >> 2) << LCD_SEG22_SHIFT) | (((Digit[1] & 0x8) >> 3) << LCD_SEG23_SHIFT);
      HAL_LCD_Write(&hlcd, LCD_DIGIT1_COM1, LCD_DIGIT1_COM1_SEG_MASK, data) ; /* 1F 1A 1C 1D  */
      
      data = ((Digit[2] & 0x1) << LCD_SEG0_SHIFT) | (((Digit[2] & 0x2) >> 1) << LCD_SEG1_SHIFT)
          | (((Digit[2] & 0x4) >> 2) << LCD_SEG22_SHIFT) | (((Digit[2] & 0x8) >> 3) << LCD_SEG23_SHIFT);
      HAL_LCD_Write(&hlcd, LCD_DIGIT1_COM2, LCD_DIGIT1_COM2_SEG_MASK, data) ; /* 1Q 1K 1Col 1P  */
      
      data = ((Digit[3] & 0x1) << LCD_SEG0_SHIFT) | (((Digit[3] & 0x2) >> 1) << LCD_SEG1_SHIFT)
          | (((Digit[3] & 0x4) >> 2) << LCD_SEG22_SHIFT) | (((Digit[3] & 0x8) >> 3) << LCD_SEG23_SHIFT);
      HAL_LCD_Write(&hlcd, LCD_DIGIT1_COM3, LCD_DIGIT1_COM3_SEG_MASK, data) ; /* 1H 1J 1DP 1N  */
      break;

    /* Position 2 on LCD (Digit2)*/
    case LCD_DIGIT_POSITION_2:
      data = ((Digit[0] & 0x1) << LCD_SEG2_SHIFT) | (((Digit[0] & 0x2) >> 1) << LCD_SEG3_SHIFT)
          | (((Digit[0] & 0x4) >> 2) << LCD_SEG20_SHIFT) | (((Digit[0] & 0x8) >> 3) << LCD_SEG21_SHIFT);
      HAL_LCD_Write(&hlcd, LCD_DIGIT2_COM0, LCD_DIGIT2_COM0_SEG_MASK, data); /* 1G 1B 1M 1E */
      
      data = ((Digit[1] & 0x1) << LCD_SEG2_SHIFT) | (((Digit[1] & 0x2) >> 1) << LCD_SEG3_SHIFT)
          | (((Digit[1] & 0x4) >> 2) << LCD_SEG20_SHIFT) | (((Digit[1] & 0x8) >> 3) << LCD_SEG21_SHIFT);
      HAL_LCD_Write(&hlcd, LCD_DIGIT2_COM1, LCD_DIGIT2_COM1_SEG_MASK, data) ; /* 1F 1A 1C 1D  */
      
      data = ((Digit[2] & 0x1) << LCD_SEG2_SHIFT) | (((Digit[2] & 0x2) >> 1) << LCD_SEG3_SHIFT)
          | (((Digit[2] & 0x4) >> 2) << LCD_SEG20_SHIFT) | (((Digit[2] & 0x8) >> 3) << LCD_SEG21_SHIFT);
      HAL_LCD_Write(&hlcd, LCD_DIGIT2_COM2, LCD_DIGIT2_COM2_SEG_MASK, data) ; /* 1Q 1K 1Col 1P  */
      
      data = ((Digit[3] & 0x1) << LCD_SEG2_SHIFT) | (((Digit[3] & 0x2) >> 1) << LCD_SEG3_SHIFT)
          | (((Digit[3] & 0x4) >> 2) << LCD_SEG20_SHIFT) | (((Digit[3] & 0x8) >> 3) << LCD_SEG21_SHIFT);
      HAL_LCD_Write(&hlcd, LCD_DIGIT2_COM3, LCD_DIGIT2_COM3_SEG_MASK, data) ; /* 1H 1J 1DP 1N  */
      break;
    
    /* Position 3 on LCD (Digit3)*/
    case LCD_DIGIT_POSITION_3:
      data = ((Digit[0] & 0x1) << LCD_SEG4_SHIFT) | (((Digit[0] & 0x2) >> 1) << LCD_SEG5_SHIFT)
          | (((Digit[0] & 0x4) >> 2) << LCD_SEG18_SHIFT) | (((Digit[0] & 0x8) >> 3) << LCD_SEG19_SHIFT);
      HAL_LCD_Write(&hlcd, LCD_DIGIT3_COM0, LCD_DIGIT3_COM0_SEG_MASK, data); /* 1G 1B 1M 1E */
      
      data = ((Digit[1] & 0x1) << LCD_SEG4_SHIFT) | (((Digit[1] & 0x2) >> 1) << LCD_SEG5_SHIFT)
          | (((Digit[1] & 0x4) >> 2) << LCD_SEG18_SHIFT) | (((Digit[1] & 0x8) >> 3) << LCD_SEG19_SHIFT);
      HAL_LCD_Write(&hlcd, LCD_DIGIT3_COM1, LCD_DIGIT3_COM1_SEG_MASK, data) ; /* 1F 1A 1C 1D  */
      
      data = ((Digit[2] & 0x1) << LCD_SEG4_SHIFT) | (((Digit[2] & 0x2) >> 1) << LCD_SEG5_SHIFT)
          | (((Digit[2] & 0x4) >> 2) << LCD_SEG18_SHIFT) | (((Digit[2] & 0x8) >> 3) << LCD_SEG19_SHIFT);
      HAL_LCD_Write(&hlcd, LCD_DIGIT3_COM2, LCD_DIGIT3_COM2_SEG_MASK, data) ; /* 1Q 1K 1Col 1P  */
      
      data = ((Digit[3] & 0x1) << LCD_SEG4_SHIFT) | (((Digit[3] & 0x2) >> 1) << LCD_SEG5_SHIFT)
          | (((Digit[3] & 0x4) >> 2) << LCD_SEG18_SHIFT) | (((Digit[3] & 0x8) >> 3) << LCD_SEG19_SHIFT);
      HAL_LCD_Write(&hlcd, LCD_DIGIT3_COM3, LCD_DIGIT3_COM3_SEG_MASK, data) ; /* 1H 1J 1DP 1N  */
      break;
    
    /* Position 4 on LCD (Digit4)*/
    case LCD_DIGIT_POSITION_4:
      data = ((Digit[0] & 0x1) << LCD_SEG6_SHIFT) | (((Digit[0] & 0x8) >> 3) << LCD_SEG17_SHIFT);
      HAL_LCD_Write(&hlcd, LCD_DIGIT4_COM0, LCD_DIGIT4_COM0_SEG_MASK, data); /* 1G 1B 1M 1E */
      
      data = (((Digit[0] & 0x2) >> 1) << LCD_SEG7_SHIFT) | (((Digit[0] & 0x4) >> 2) << LCD_SEG16_SHIFT);
      HAL_LCD_Write(&hlcd, LCD_DIGIT4_COM0_1, LCD_DIGIT4_COM0_1_SEG_MASK, data); /* 1G 1B 1M 1E */
      
      data = ((Digit[1] & 0x1) << LCD_SEG6_SHIFT) | (((Digit[1] & 0x8) >> 3) << LCD_SEG17_SHIFT);
      HAL_LCD_Write(&hlcd, LCD_DIGIT4_COM1, LCD_DIGIT4_COM1_SEG_MASK, data) ; /* 1F 1A 1C 1D  */
      
      data = (((Digit[1] & 0x2) >> 1) << LCD_SEG7_SHIFT) | (((Digit[1] & 0x4) >> 2) << LCD_SEG16_SHIFT);
      HAL_LCD_Write(&hlcd, LCD_DIGIT4_COM1_1, LCD_DIGIT4_COM1_1_SEG_MASK, data) ; /* 1F 1A 1C 1D  */
      
      data = ((Digit[2] & 0x1) << LCD_SEG6_SHIFT) | (((Digit[2] & 0x8) >> 3) << LCD_SEG17_SHIFT);
      HAL_LCD_Write(&hlcd, LCD_DIGIT4_COM2, LCD_DIGIT4_COM2_SEG_MASK, data) ; /* 1Q 1K 1Col 1P  */
      
      data = (((Digit[2] & 0x2) >> 1) << LCD_SEG7_SHIFT) | (((Digit[2] & 0x4) >> 2) << LCD_SEG16_SHIFT);
      HAL_LCD_Write(&hlcd, LCD_DIGIT4_COM2_1, LCD_DIGIT4_COM2_1_SEG_MASK, data) ; /* 1Q 1K 1Col 1P  */
      
      data = ((Digit[3] & 0x1) << LCD_SEG6_SHIFT) | (((Digit[3] & 0x8) >> 3) << LCD_SEG17_SHIFT);
      HAL_LCD_Write(&hlcd, LCD_DIGIT4_COM3, LCD_DIGIT4_COM3_SEG_MASK, data) ; /* 1H 1J 1DP 1N  */
      
      data = (((Digit[3] & 0x2) >> 1) << LCD_SEG7_SHIFT) | (((Digit[3] & 0x4) >> 2) << LCD_SEG16_SHIFT);
      HAL_LCD_Write(&hlcd, LCD_DIGIT4_COM3_1, LCD_DIGIT4_COM3_1_SEG_MASK, data) ; /* 1H 1J 1DP 1N  */
      break;
    
    /* Position 5 on LCD (Digit5)*/
    case LCD_DIGIT_POSITION_5:
       data = (((Digit[0] & 0x2) >> 1) << LCD_SEG9_SHIFT) | (((Digit[0] & 0x4) >> 2) << LCD_SEG14_SHIFT);
      HAL_LCD_Write(&hlcd, LCD_DIGIT5_COM0, LCD_DIGIT5_COM0_SEG_MASK, data); /* 1G 1B 1M 1E */
      
      data = ((Digit[0] & 0x1) << LCD_SEG8_SHIFT) | (((Digit[0] & 0x8) >> 3) << LCD_SEG15_SHIFT);
      HAL_LCD_Write(&hlcd, LCD_DIGIT5_COM0_1, LCD_DIGIT5_COM0_1_SEG_MASK, data); /* 1G 1B 1M 1E */
      
      data = (((Digit[1] & 0x2) >> 1) << LCD_SEG9_SHIFT) | (((Digit[1] & 0x4) >> 2) << LCD_SEG14_SHIFT);
      HAL_LCD_Write(&hlcd, LCD_DIGIT5_COM1, LCD_DIGIT5_COM1_SEG_MASK, data) ; /* 1F 1A 1C 1D  */
      
       data = ((Digit[1] & 0x1) << LCD_SEG8_SHIFT) | (((Digit[1] & 0x8) >> 3) << LCD_SEG15_SHIFT);
      HAL_LCD_Write(&hlcd, LCD_DIGIT5_COM1_1, LCD_DIGIT5_COM1_1_SEG_MASK, data) ; /* 1F 1A 1C 1D  */
      
      data = (((Digit[2] & 0x2) >> 1) << LCD_SEG9_SHIFT) | (((Digit[2] & 0x4) >> 2) << LCD_SEG14_SHIFT);
      HAL_LCD_Write(&hlcd, LCD_DIGIT5_COM2, LCD_DIGIT5_COM2_SEG_MASK, data) ; /* 1Q 1K 1Col 1P  */
      
      data = ((Digit[2] & 0x1) << LCD_SEG8_SHIFT) | (((Digit[2] & 0x8) >> 3) << LCD_SEG15_SHIFT);
      HAL_LCD_Write(&hlcd, LCD_DIGIT5_COM2_1, LCD_DIGIT5_COM2_1_SEG_MASK, data) ; /* 1Q 1K 1Col 1P  */
      
      data = (((Digit[3] & 0x2) >> 1) << LCD_SEG9_SHIFT) | (((Digit[3] & 0x4) >> 2) << LCD_SEG14_SHIFT);
      HAL_LCD_Write(&hlcd, LCD_DIGIT5_COM3, LCD_DIGIT5_COM3_SEG_MASK, data) ; /* 1H 1J 1DP 1N  */
      
      data = ((Digit[3] & 0x1) << LCD_SEG8_SHIFT) | (((Digit[3] & 0x8) >> 3) << LCD_SEG15_SHIFT);
      HAL_LCD_Write(&hlcd, LCD_DIGIT5_COM3_1, LCD_DIGIT5_COM3_1_SEG_MASK, data) ; /* 1H 1J 1DP 1N  */
      break;
    
    /* Position 6 on LCD (Digit6)*/
    case LCD_DIGIT_POSITION_6:
      data = ((Digit[0] & 0x1) << LCD_SEG10_SHIFT) | (((Digit[0] & 0x2) >> 1) << LCD_SEG11_SHIFT)
          | (((Digit[0] & 0x4) >> 2) << LCD_SEG12_SHIFT) | (((Digit[0] & 0x8) >> 3) << LCD_SEG13_SHIFT);
      HAL_LCD_Write(&hlcd, LCD_DIGIT6_COM0, LCD_DIGIT6_COM0_SEG_MASK, data); /* 1G 1B 1M 1E */
      
      data = ((Digit[1] & 0x1) << LCD_SEG10_SHIFT) | (((Digit[1] & 0x2) >> 1) << LCD_SEG11_SHIFT)
          | (((Digit[1] & 0x4) >> 2) << LCD_SEG12_SHIFT) | (((Digit[1] & 0x8) >> 3) << LCD_SEG13_SHIFT);
      HAL_LCD_Write(&hlcd, LCD_DIGIT6_COM1, LCD_DIGIT6_COM1_SEG_MASK, data) ; /* 1F 1A 1C 1D  */
      
      data = ((Digit[2] & 0x1) << LCD_SEG10_SHIFT) | (((Digit[2] & 0x2) >> 1) << LCD_SEG11_SHIFT)
          | (((Digit[2] & 0x4) >> 2) << LCD_SEG12_SHIFT) | (((Digit[2] & 0x8) >> 3) << LCD_SEG13_SHIFT);
      HAL_LCD_Write(&hlcd, LCD_DIGIT6_COM2, LCD_DIGIT6_COM2_SEG_MASK, data) ; /* 1Q 1K 1Col 1P  */
      
      data = ((Digit[3] & 0x1) << LCD_SEG10_SHIFT) | (((Digit[3] & 0x2) >> 1) << LCD_SEG11_SHIFT)
          | (((Digit[3] & 0x4) >> 2) << LCD_SEG12_SHIFT) | (((Digit[3] & 0x8) >> 3) << LCD_SEG13_SHIFT);
      HAL_LCD_Write(&hlcd, LCD_DIGIT6_COM3, LCD_DIGIT6_COM3_SEG_MASK, data) ; /* 1H 1J 1DP 1N  */
      break;
    
     default:
      break;
  }
}

void lcd_display_control(uint8_t status, uint8_t precent)
{
	HAL_LCD_Clear(&hlcd);
	uint8_t index_num;
	index_num = status + '0';
	WriteChar(&index_num, POINT_OFF, DOUBLEPOINT_OFF, LCD_DIGIT_POSITION_3);
	
	index_num = (precent / 100) + '0';
	WriteChar(&index_num, POINT_OFF, DOUBLEPOINT_OFF, LCD_DIGIT_POSITION_4);
	
	index_num = ((precent % 100)) / 10 + '0';
	WriteChar(&index_num, POINT_OFF, DOUBLEPOINT_OFF, LCD_DIGIT_POSITION_5);
	
	index_num = (precent % 10) + '0' ;
	WriteChar(&index_num, POINT_OFF, DOUBLEPOINT_OFF, LCD_DIGIT_POSITION_6);
	
	HAL_LCD_UpdateDisplayRequest(&hlcd);
	
}
void lcd_DisplayString(uint8_t* ptr)
{
  DigitPosition_Typedef position = LCD_DIGIT_POSITION_1;

  /* Send the string character by character on lCD */
  while ((*ptr != 0) & (position <= LCD_DIGIT_POSITION_6))
  {
    /* Write one character on LCD */
    WriteChar(ptr, POINT_OFF, DOUBLEPOINT_OFF, position);

    /* Point on the next character */
    ptr++;

    /* Increment the character counter */
    position++;
  }
  /* Update the LCD display */
  HAL_LCD_UpdateDisplayRequest(&hlcd);
}
