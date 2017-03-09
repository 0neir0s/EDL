#include "at89c5131.h"
#include "stdio.h"
#include "math.h"

#define LCD_data  P2	    					// LCD Data port


void SPI_Init();
void LCD_Init();
void LCD_DataWrite(char dat);
void LCD_CmdWrite(char cmd);
void LCD_StringWrite(char * str, unsigned char len);
void LCD_Ready();
void sdelay(int delay);
void Meas_init();
void Timer_init();
void Timer_reset();

sbit LCD_rs = P0^0;  								// LCD Register Select
sbit LCD_rw = P0^1;  								// LCD Read/Write
sbit LCD_en = P0^2;  								// LCD Enable
sbit LCD_busy = P2^7;								// LCD Busy Flag
sbit RX_pin = P3^1;

int time_meas = 0;
int thresh = 768;
char RX_char[10] = {'-','-','-','-','-','-','-','-','-','-'};
int bit_array[8];
int fresh_flag = 1;
int i_count = 0;
int b_count = -1;
int char_count = 0;
int RX_value = 0;

void ex0_isr (void) interrupt 0
{
	time_meas = TH0*256+TL0;
	if(i_count == 0)
	{
		TR0 = 1;
	}
	else if(i_count == 1)
	{
		TR0 = 0;
		Timer_reset();
		if(time_meas < 350)
		{
			b_count = 0;
		}
		else
		{
			i_count = -1;
		}
		TR0 = 1;
	}
	if(b_count >= 0)
	{
		TR0 = 0;
		Timer_reset();
		if(time_meas>1200)
		{
			RX_char[b_count] = '0';
			bit_array[b_count] = 0;
			TR0 = 1;
		}
		else
		{
			RX_char[b_count] = '1';
			bit_array[b_count] = 1;
			TR0 = 1;
		}
		b_count++ ;
		if(b_count == 8)
		{
			TR0 = 0;
			b_count = -1;
			i_count = -1;
			Timer_reset();
			//EA = 0;
			//RX_char[char_count] = 128*bit_array[7]+64*bit_array[6]+32*bit_array[5]+16*bit_array[4]+8*bit_array[3]+4*bit_array[2]+2*bit_array[1]+bit_array[0];
			RX_char[8] = '-';
			RX_char[9] = 128*bit_array[7]+64*bit_array[6]+32*bit_array[5]+16*bit_array[4]+8*bit_array[3]+4*bit_array[2]+2*bit_array[1]+bit_array[0];
			
			char_count = (char_count+1)%10 ;
		}
		
	}
	i_count++ ;
}

void main(void)
{
	LCD_Init();
	Timer_init();
	Meas_init();
	
	while(1)
	{
		LCD_CmdWrite(0x80);			// Cursor to First line First Position
		LCD_StringWrite(RX_char,10);
		//LCD_StringWrite(bit_array,8);
	}
}
 // FUNCTION_PURPOSE:LCD Initialization
 // FUNCTION_INPUTS: void
 // FUNCTION_OUTPUTS: none
 
void LCD_Init()
{
  sdelay(100);
  LCD_CmdWrite(0x38);   	// LCD 2lines, 5*7 matrix
  LCD_CmdWrite(0x0E);			// Display ON cursor ON  Blinking off
  LCD_CmdWrite(0x01);			// Clear the LCD
  LCD_CmdWrite(0x80);			// Cursor to First line First Position
}

/**
 * FUNCTION_PURPOSE: Write Command to LCD
 * FUNCTION_INPUTS: cmd- command to be written
 * FUNCTION_OUTPUTS: none
 */
void LCD_CmdWrite(char cmd)
{
	LCD_Ready();
	LCD_data=cmd;     			// Send the command to LCD
	LCD_rs=0;         	 		// Select the Command Register by pulling LCD_rs LOW
  LCD_rw=0;          			// Select the Write Operation  by pulling RW LOW
  LCD_en=1;          			// Send a High-to-Low Pusle at Enable Pin
  sdelay(5);
  LCD_en=0;
	sdelay(5);
}

/**
 * FUNCTION_PURPOSE: Write Command to LCD
 * FUNCTION_INPUTS: dat- data to be written
 * FUNCTION_OUTPUTS: none
 */
void LCD_DataWrite( char dat)
{
	LCD_Ready();
  LCD_data=dat;	   				// Send the data to LCD
  LCD_rs=1;	   						// Select the Data Register by pulling LCD_rs HIGH
  LCD_rw=0;    	     			// Select the Write Operation by pulling RW LOW
  LCD_en=1;	   						// Send a High-to-Low Pusle at Enable Pin
  sdelay(5);
  LCD_en=0;
	sdelay(5);
}
/**
 * FUNCTION_PURPOSE: Write a string on the LCD Screen
 * FUNCTION_INPUTS: 1. str - pointer to the string to be written, 
										2. length - length of the array
 * FUNCTION_OUTPUTS: none
 */
void LCD_StringWrite( char * str, unsigned char length)
{
    while(length>0)
    {
        LCD_DataWrite(*str);
        str++;
        length--;
    }
}

/**
 * FUNCTION_PURPOSE: To check if the LCD is ready to communicate
 * FUNCTION_INPUTS: void
 * FUNCTION_OUTPUTS: none
 */
void LCD_Ready()
{
	LCD_data = 0xFF;
	LCD_rs = 0;
	LCD_rw = 1;
	LCD_en = 0;
	sdelay(5);
	LCD_en = 1;
	while(LCD_busy == 1)
	{
		LCD_en = 0;
		LCD_en = 1;
	}
	LCD_en = 0;
}

/**
 * FUNCTION_PURPOSE: A delay of 15us for a 24 MHz crystal
 * FUNCTION_INPUTS: void
 * FUNCTION_OUTPUTS: none
 */
void sdelay(int delay)
{
	char d=0;
	while(delay>0)
	{
		for(d=0;d<5;d++);
		delay--;
	}
}
void Timer_init()
{
	TMOD = 0x01;
	TH0 = 0x00;
	TL0 = 0x00;
}
void Timer_reset()
{
	TH0 = 0;
	TL0 = 0;
}
void Meas_init()
{
	EA = 1;
	EX0 = 1;
	IT0 = 1;
}