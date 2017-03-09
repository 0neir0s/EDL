#include "at89c5131.h"
#include "stdio.h"

#define LCD_data  P2	    					// LCD Data port


void SPI_Init();
void LCD_Init();
void Timer_Init(int b);
void LCD_DataWrite(char dat);
void LCD_CmdWrite(char cmd);
void LCD_StringWrite(char * str, unsigned char len);
void LCD_Ready();
void check_column();
void check_row();
void sdelay(int delay);
void keys_init();
void send_char(char a);
void send_bit(int b);

int column = 0;
int row = 0;
char msg[1];
char count_str[4];
char keys[4][4];

int success;
int prev_success;
int s_count = 0;
sbit Data_Out = P0^4;

sbit LCD_rs = P0^0;  								// LCD Register Select
sbit LCD_rw = P0^1;  								// LCD Read/Write
sbit LCD_en = P0^2;  								// LCD Enable
sbit LCD_busy = P2^7;								// LCD Busy Flag

void main(void)
{
	Data_Out = 0;
	LCD_Init();
	keys_init();
	while (1)
		{
			LCD_CmdWrite(0x80);			// Cursor to First line First Position
			check_row();
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

void check_row()
{
		//row = 0;
		success = 1;
		P3 = 0x7F;
		if(P3 != 0x7F)
		{
			row = 1;
			check_column();
		}
		else
		{
			P3 = 0xBF;
			if(P3 != 0xBF)
			{
				row = 2;
				check_column();
			}
			else
			{
				P3 = 0xDF;
				if(P3 != 0xDF)
				{
					row = 3;
					check_column();
				}
				else
				{
					P3 = 0xEF;
					if(P3 != 0xEF)
					{
						row = 4;
						check_column();
					}
					else
					{
						success = 0;
					}
				}
			}
		}
	if(success == 1 && prev_success == 0)
	{
		s_count++;
		
		msg[0] = keys[row-1][column-1];	
		send_char(msg[0]); 
		count_str[3] = (char) ((s_count)%10 + 48);
		count_str[2] = (char) ((s_count/10)%10 + 48);
		count_str[1] = (char) ((s_count/100)%10 + 48);
		count_str[0] = '_';	
		LCD_StringWrite(msg,1);
		LCD_StringWrite(count_str,4);	
	}
//	rc[1] = (char) (column + 48);
//	rc[0] = (char) (row + 48);	
	
	
	prev_success = success;
} 

void check_column()
{
		//column = 0;
		P3 = 0xF7;
		if(P3 != 0xF7)
		{
			column = 1;
		}
		else
		{
			P3 = 0xFB;
			if(P3 != 0xFB)
			{
				column = 2;
			}
			else
			{
				P3 = 0xFD;
				if(P3 != 0xFD)
				{
					column = 3;
				}
				else
				{
					P3 = 0xFE;
					if(P3 != 0xFE)
					{
						column = 4;
					}
				}
			}
		}
	}

	void keys_init(void)
	{
		keys[0][0] = '1';
		keys[0][1] = '2';
		keys[0][2] = '3';
		keys[0][3] = 'A';
		
		keys[1][0] = '4';
		keys[1][1] = '5';
		keys[1][2] = '6';
		keys[1][3] = 'B';
		
		keys[2][0] = '7';
		keys[2][1] = '8';
		keys[2][2] = '9';
		keys[2][3] = 'C';
		
		keys[3][0] = '*';
		keys[3][1] = '0';
		keys[3][2] = '#';
		keys[3][3] = 'D';
	}
	
	void send_char( char a )
	{
		int a_int = (int) a;
		int i;
		int a_bits[9];
		for(i=0;i<8;i++)
		{
			a_bits[i] = a_int%2;
			a_int = a_int/2;
		}
		a_bits[8] = 1;
		for(i=0;i<8;i++)
		{
			send_bit(a_bits[i]);
		}
	}
	void Timer_Init(int b)
	{
		TMOD = 0x10;
		if(b)
		{
			TH0 = 0xFE;
			TL0 = 0x0C;
		}
		else
		{
			TH0 = 0xFC;
			TL0 = 0x18;
		}
		TR0 = 1;
	}
	void send_bit( int b )
	{
		Timer_Init(b);
		while(TF0 == 0);
		TF0 = 0;
		//invert here;
		Data_Out = 1;
		Timer_Init(b);
		while(TF0 == 0);
		TF0 = 0;
		//invert again
		Data_Out = 0;
	}