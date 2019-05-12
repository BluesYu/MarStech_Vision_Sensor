#include <stdio.h>  
#include <unistd.h>  
#include <sys/mman.h>  
#include <sys/types.h>  
#include <sys/stat.h>  
#include <fcntl.h> 
#include "gpio_pi.h" 
#include <linux/gpio.h> //里面声明io口的操作函数 
 
int main()
{
	GPIO_Init();
	GPIO_ConfigPin(PA,0,IN);
	GPIO_ConfigPin(PA,6,IN);
	GPIO_ConfigPin(PA,2,IN);
	
	// GPIO_ConfigPin(PA,3,OUT);
	// GPIO_ConfigPin(PC,0,OUT);
	// GPIO_ConfigPin(PC,1,OUT);
	// GPIO_ConfigPin(PA,1,OUT);
	
	unsigned int a=0,b=0,c=0;
	unsigned int led=0,led1=0,led2=0,led3=0;
	while(1)
	{
		//a = ~a;
		GPIO_GetPin(PA,0,a);
	    GPIO_GetPin(PA,6,b);
		GPIO_GetPin(PA,2,c);
		printf ("a=%d,b=%d,c=%d\n", a,b,c);
		
		if(a==0)
		{
		 led = ~led;
		 GPIO_SetPin(PA,3,led);
		}
		if(a==0)
		{
		 led1 = ~led1;
		 GPIO_SetPin(PC,0,led1);
		}
		if(b==0)
		{
		 led2 = ~led2;
		 GPIO_SetPin(PC,1,led2);
		}
		if(c==0)
		{
		 led3 = ~led3;
		 GPIO_SetPin(PA,1,led3);
		}
		usleep(100);
	}
}