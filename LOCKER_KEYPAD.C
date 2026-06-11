#include<lpc21xx.h>
#include "header.h"
#define c0 (IOPIN1 & (1<<20))
#define c1 (IOPIN1 & (1<<21))
#define c2 (IOPIN1 & (1<<22))
#define c3 (IOPIN1 & (1<<23))

#define r0 (1<<16)
#define r1 (1<<17)
#define r2 (1<<18)
#define r3 (1<<19)

unsigned int lut[4][4]={
						 {'1','2','3','A'},
						 {'4','5','6','B'},
						 {'7','8','9','C'},
						 {'*','0','#','D'}
						};

unsigned int keyscan(void)
{
	char row,col;
	IODIR1=r0|r1|r2|r3;
	while(1)
	{
		IOCLR1=r0|r1|r2|r3;
		IOSET1=c0|c1|c2|c3;
		while((c0&&c1&&c2&&c3)==1);
		
		IOCLR1=r0;
		IOSET1=r1|r2|r3;
		if((c0&&c1&&c2&&c3)==0)
		{
			row=0;
			break;
		}

		IOCLR1=r1;
		IOSET1=r0|r2|r3;
		if((c0&&c1&&c2&&c3)==0)
		{
			row=1;
			break;
		}
		
		IOCLR1=r2;
		IOSET1=r1|r0|r3;
		if((c0&&c1&&c2&&c3)==0)
		{
			row=2;
			break;
		}
		
		IOCLR1=r3;
		IOSET1=r1|r2|r0;
		if((c0&&c1&&c2&&c3)==0)
		{
			row=3;
			break;
		}
	}
	
	if(c0==0)
		col=0;
	else if(c1==0)
		col=1;
	else if(c2==0)
		col=2;
	else
		col=3;
	
	delay_ms(100);//avoid switch bounce
	while((c0&&c1&&c2&&c3)==0);
	return lut[row][col];
}
