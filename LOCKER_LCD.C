#include<lpc21xx.h>
#include "header.h"
#define lcd 0xff<<4
#define rs 1<<2
#define e 1<<3

void lcd_init(void);
void lcd_cmd(char);
void lcd_data(char);
void lcd_str(char*);
void lcd_title(void);

void lcd_init(void)
{
	PINSEL0|=0;
	IODIR0|=lcd|rs|e;
	lcd_cmd(0X01);
	lcd_cmd(0X02);
	lcd_cmd(0X0C);
	lcd_cmd(0X38);
}
void lcd_cmd(char c)
{
	IOCLR0|=lcd;
	IOSET0|=c<<4;
	IOCLR0|=rs;
	IOSET0|=e;
	delay_ms(2);
	IOCLR0|=e;
}
void lcd_data(char d)
{
	IOCLR0|=lcd;
	IOSET0|=d<<4;
	IOSET0|=rs;
	IOSET0|=e;
	delay_ms(2);
	IOCLR0|=e;
}

void lcd_str(char *a)
{
	int i;
		for(i = 0; a[i]; i++)
		{
			lcd_data(a[i]);
			if(i==15)
				lcd_cmd(0xc0);
		}	
}

void lcd_title(void)
{
	int i,j=0;
while(j<2)
{
	lcd_cmd(0x01);
	lcd_cmd(0x82);
	lcd_str("FORTILOCKER:");
	lcd_cmd(0xc0);
	lcd_str("2FA BANK LOCKER");
	
	delay_ms(500);
	for(i=0;i<16;i++)
	{
		lcd_cmd(0x1C);   // shift right
		delay_ms(250);
  }
	j++;
}
}
