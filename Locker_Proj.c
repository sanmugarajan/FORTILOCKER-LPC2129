#include<lpc21xx.h>
#include<string.h>
#include<stdlib.h>
#include "LOCKER_KEYPAD.C"
#include "LOCKER_LCD.C"
#include "header.h"
#define buz 1<<12
#define m1 1<<13
#define m2 1<<14

void uart_conf(void);
void uart_tx(char);
void uart_str(char*);
void uart_inte(unsigned int);
void pin(void);
void otp(void);
void gsm(void);
void buzz(void);
void motor_open(void);
void motor_close(void);

int main()
{
	IODIR0 |= buz;
	IODIR0 |= m1|m2;
	IOCLR0 = buz;
  IOCLR0 = m1|m2; 
	lcd_init();
	uart_conf();
	T1PR=1000000-1;
	T1TCR=0X1;
	while(1)
	{
		lcd_title();
		pin();
		otp();
	}
}

void pin(void)
{
	char pin[10]="2574";
	char ch;
	char upin[10],count=4;
	int i;
	while(count!=0)
	{
		lcd_cmd(0x01);
		lcd_cmd(0x80);
		lcd_str("ENTER 4DIGIT PIN");
		lcd_cmd(0xc6);
		for(i=0;i<10;)
		{
			ch=keyscan();
			lcd_data('#');
			upin[i]=ch;
			delay_ms(100);
			i++;
			if(i==4)
			{
				upin[i]='\0';
				break;
			}
		}
		if(strcmp(upin,pin)!=0)
		{
			count--;
			lcd_cmd(0x01);
			if(count==3)
			{		
				lcd_str("PIN NOT MATCHED!");
				lcd_cmd(0xc0);
				lcd_str("3-ATTEMPTS LEFT!");
				buzz();
				delay_ms(500);
			}
			else if(count==2)
			{		
				lcd_str("PIN NOT MATCHED!");
				lcd_cmd(0xc0);
				lcd_str("2-ATTEMPTS LEFT!");
				buzz();
				delay_ms(500);
			}
			else if(count==1)
			{
				lcd_str("PIN NOT MATCHED!");
				lcd_cmd(0xc0);
				lcd_str("1-ATTEMPT LEFT!");
				buzz();
				delay_ms(500);
			}
			else
			{			
				lcd_str("MAXIMUM LIMIT");
				lcd_cmd(0xc3);
				lcd_str("REACHED!");
				delay_ms(500);
				lcd_cmd(0x01);
				lcd_cmd(0x82);
				lcd_str("TRY AGAIN!");
				lcd_cmd(0xc1);
				lcd_str("AFTER 24HRS...");
				buzz();
				delay_sec(1);
				lcd_cmd(0x01);
				while(1);
			}
		}
		else
		{
			lcd_cmd(0x01);
			lcd_str("PIN MATCHED!");
			delay_sec(1);
			break;
		}
	}
}

void otp(void)
{
	unsigned int OTP;
	char EOTP[10];
	char cnt=4,k,no;
	while(cnt!=0)
	{
		lcd_cmd(0x01);
		lcd_str("GENERATING OTP..");
		delay_ms(500);
		lcd_cmd(0x01);	
		OTP=T1PC;
		if(OTP>=100000)
		{
			//gsm();
			uart_str("LOCKER OTP: ");
			uart_inte(OTP);
			uart_tx('\r');//not for gsm
			uart_tx('\n');//not for gsm
			//uart_tx(0x1A);
			lcd_cmd(0x84);
			lcd_str("OTP SENT!");
			delay_ms(500);
			lcd_cmd(0x01);
		}
		lcd_str("ENTER 6DIGIT OTP");
		lcd_cmd(0xc5);
		for(k=0;k<10;)
		{
			no=keyscan();
			lcd_data('*');
			EOTP[k]=no;
			delay_ms(100);
			k++;
			if(k==6)
			{
				EOTP[k]='\0';
				break;
			}
		}
		if(atoi(EOTP)!=OTP)
		{
			cnt--;
			lcd_cmd(0x01);
			if(cnt==3)
			{		
				lcd_str("OTP NOT MATCHED!");
				uart_str("INVALID OTP ENTERED\r\n");//not for gsm
				//gsm();
				//uart_str("ENTERED OTP IS INVALID, 3-ATTEMPTS LEFT...");
				//uart_tx(0x1A);
				lcd_cmd(0xc0);
				lcd_str("3-ATTEMPTS LEFT!");
				buzz();
				delay_ms(500);
			}
			if(cnt==2)
			{		
				lcd_str("OTP NOT MATCHED!");
				uart_str("INVALID OTP ENTERED\r\n");//not for gsm
				//gsm();
				//uart_str("ENTERED OTP IS INVALID, 2-ATTEMPTS LEFT...");
				//uart_tx(0x1A);
				lcd_cmd(0xc0);
				lcd_str("2-ATTEMPTS LEFT!");
				buzz();
				delay_ms(500);
			}
			if(cnt==1)
			{
				lcd_str("OTP NOT MATCHED!");
				uart_str("INVALID OTP ENTERED\r\n");//not for gsm
				//gsm();
				//uart_str("ENTERED OTP IS INVALID, 1-ATTEMPTS LEFT...");
				//uart_tx(0x1A);
				lcd_cmd(0xc0);
				lcd_str("1-ATTEMPT LEFT!");
				buzz();
				delay_ms(500);
			}
			if(cnt==0)
			{
				lcd_str("MAXIMUM LIMIT");
				uart_str("MAXIMUM LOGIN LIMIT REACHED! TRY AGAIN AFTER 24HRS\r\n");//not for gsm
				//gsm();
				//uart_str("MAXIMUM LOGIN LIMIT REACHED! TRY AGAIN AFTER 24HRS");
				//uart_tx(0x1A);
				lcd_cmd(0xc3);
				lcd_str("REACHED!");
				delay_ms(500);
				lcd_cmd(0x01);
				lcd_cmd(0x82);
				lcd_str("TRY AGAIN!");
				lcd_cmd(0xc1);
				lcd_str("AFTER 24HRS...");
				buzz();
				delay_sec(1);
				lcd_cmd(0x01);
	
				while(1);
			}
		}
		else
		{
			//delay_sec(1);
			lcd_cmd(0x01);
			lcd_str("LOGIN SUCCESS!");
			delay_ms(500);
			lcd_cmd(0x01);
			lcd_str("LOCKER OPENED!");
			motor_open();
			delay_sec(2);
			lcd_cmd(0x01);
			lcd_str("LOCKER CLOSED!");
			motor_close();
			return;
		}
	}
}
void buzz(void)
{
	IOSET0 = buz;
	delay_sec(1);
	
	IOCLR0 = buz;
	delay_sec(1);
}
void motor_open(void) //clockwise
{
	IOSET0 = m1;
	IOCLR0 = m2;
	delay_sec(2);
	IOCLR0 = m1;
	IOCLR0 = m2;
}
void motor_close(void) //anti-clockwise
{
    IOCLR0 = m1;
    IOSET0 = m2;
    delay_sec(2);
    IOCLR0 = m1;
    IOCLR0 = m2;
}
void uart_conf(void)
{
	PINSEL0|=0X5;
	U0LCR=0X83;
	U0DLL=97;
	U0DLM=0;
	U0LCR=0X03;
}
void uart_tx(char t)
{
  while(((U0LSR>>5)&1)==0);
	U0THR=t;
}
void uart_str(char*s)
{
	while(*s)
	{
		while((U0LSR & (1<<5)) == 0);
		uart_tx(*s++);
	}
	
}
void uart_inte(unsigned int n)
{
	char a[50];
	int i=0;
	if(n==0)
	{
		uart_tx('0');
	}
	else
	{
		while(n>0)
		{
			a[i++]=n%10;
			n=n/10;
		}
		for(--i;i>=0;i--)
		{
			while((U0LSR & (1<<5)) == 0);
			U0THR=(a[i]+'0');
		}
	}
}
void gsm(void)
{
	uart_str("AT\r\n");
	uart_str("ATEO\r\n");
  delay_ms(500);

  uart_str("AT+CMGF=1\r\n");
  delay_ms(500);

	uart_str("AT+CMGS=\"+916383102675\"\r\n");
	delay_ms(500);	
	
}
