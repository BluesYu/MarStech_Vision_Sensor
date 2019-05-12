#include <stdio.h>  
#include <unistd.h>  
#include <sys/mman.h>  
#include <sys/types.h>  
#include <sys/stat.h>  
#include <fcntl.h> 
#include <linux/gpio.h> //里面声明io口的操作函数 
#include "gpio_pi.h" 
#include "led_pi.h" 

/******************************************************************************************************
* 函数(Function)：       led_Init()
* 功能(Description)：    初始化led
* 调用函数（Calls）： 
* 输入参数（parameter）：
* 返回数值(return)：    return = -1：失败
                        return = 0：成功
* 其他(Others):     配置GPIO，用于输入模式
********************************************************************************************************/ 
int led_Init()
{
	if(GPIO_Init() < 0)
	  return -1;
	GPIO_ConfigPin(PA,3,OUT);
	GPIO_ConfigPin(PC,0,OUT);
	GPIO_ConfigPin(PC,1,OUT);
	GPIO_ConfigPin(PA,1,OUT);
	unsigned int led=0;
	GPIO_SetPin(PA,3,led);
	led=1;
    GPIO_SetPin(PC,0,led);
	GPIO_SetPin(PC,1,led);//全灭灯
	GPIO_SetPin(PA,1,led);	
	return 0;
}

/******************************************************************************************************
* 函数(Function)：           camera_led_Set(unsigned int led)
* 功能(Description)：          控制补光led
* 调用函数（Calls）： 
* 输入参数（parameter）： unsigned int led：开关（0,1）
* 返回数值(return)：   return -1:失败
                       其他，成功。  
* 其他(Others):    
********************************************************************************************************/ 
int camera_led_Set(unsigned int led)
{
	if(led>1) 
		return -1;
	else if(led==1)
			{
			  //led = ~led;
			  GPIO_SetPin(PA,3,led);
			  return 1;
			}
			else
			{
			  //led = ~led;
			  GPIO_SetPin(PA,3,led);
			  return 0;
			}
	return -1;		
}


/******************************************************************************************************
* 函数(Function)：            led_Show(unsigned int &Mode)
* 功能(Description)：          显示led
* 调用函数（Calls）： 
* 输入参数（parameter）： unsigned int &Mode：系统模式
* 返回数值(return)：   无
* 其他(Others):    led可以调节
********************************************************************************************************/ 
void led_Show(unsigned int Mode)
{
	unsigned int led=0,led1=0,led2=0,led3=0;
	switch(Mode)
	{
		case 0://休眠，黄灯亮起，等待输入
				led1 = 1;
		        GPIO_SetPin(PC,0,led1);
				led2 =  0;
		        GPIO_SetPin(PC,1,led2);//黄色亮起
				led3 = 1;
		        GPIO_SetPin(PA,1,led3);		
		        break;
		case 1://识别颜色模式，全灭灯,蓝灯闪烁五次
		        led1 = 1;
		        GPIO_SetPin(PC,0,led1);
				led2 =  1;
		        GPIO_SetPin(PC,1,led2);
				led3 = 1;
		        GPIO_SetPin(PA,1,led3);
			    // for(int i=0;i<5;i++)
				// {
				// led3 = 1;
		        // GPIO_SetPin(PA,1,led3);
				// usleep(30000);//30ms
				// led3 = 0;
		        // GPIO_SetPin(PA,1,led3);
				// usleep(30000);//30ms
				// }				
		        break;
		case 2://跟踪模式，绿灯亮起
		       	led1 = 0;
		        GPIO_SetPin(PC,0,led1);//GN
				led2 =  1;
		        GPIO_SetPin(PC,1,led2);
				led3 = 1;
		        GPIO_SetPin(PA,1,led3);		
		        break;
		case 0x66://采集目标，绿灯闪烁
		        led1 = 0;
		        GPIO_SetPin(PC,0,led1);//GN
				usleep(100000);//30ms
				led1 = 1;
		        GPIO_SetPin(PC,0,led1);//GN
				usleep(100000);//30ms
				led2 =  1;
		        GPIO_SetPin(PC,1,led2);
				led3 = 1;
		        GPIO_SetPin(PA,1,led3);		
		        break;
		case 3://识别条形码模式，全灭灯
		       	led1 = 1;
		        GPIO_SetPin(PC,0,led1);
				led2 =  1;
		        GPIO_SetPin(PC,1,led2);
				led3 = 1;
		        GPIO_SetPin(PA,1,led3);		
		        break;
		case 4://识别数字模式，蓝灯亮起
		       	led1 = 1;
		        GPIO_SetPin(PC,0,led1);
				led2 =  1;
		        GPIO_SetPin(PC,1,led2);
				led3 = 0;
		        GPIO_SetPin(PA,1,led3);	//BU	
		        break;
		case 5://千分类模式，蓝灯亮起
		       	led1 = 1;
		        GPIO_SetPin(PC,0,led1);
				led2 =  1;
		        GPIO_SetPin(PC,1,led2);
				led3 = 0;
		        GPIO_SetPin(PA,1,led3);	//BU	
		        break;
		case 0x44://系统错误,黄色闪烁
		        led1 = 1;
		        GPIO_SetPin(PC,0,led1);
				led2 = 1;
		        GPIO_SetPin(PC,1,led2);//黄色闪烁，系统错误，失败
				usleep(100000);//100ms
				led2 = 0;
		        GPIO_SetPin(PC,1,led2);//黄色闪烁，系统错误，失败
				usleep(100000);//100ms
				led3 = 1;
		        GPIO_SetPin(PA,1,led3);	
                break;		
        default://系统故障，全闪烁
		        led1 = 0;
		        GPIO_SetPin(PC,0,led1);
				usleep(30000);//100ms
				 led1 = 1;
		        GPIO_SetPin(PC,0,led1);
				usleep(30000);//100ms
				led2 = 0;
		        GPIO_SetPin(PC,1,led2);//黄色闪烁，系统错误，失败
				usleep(30000);//100ms
				led2 = 1;
		        GPIO_SetPin(PC,1,led2);//黄色闪烁，系统错误，失败
				usleep(30000);//100ms
				led3 = 0;
		        GPIO_SetPin(PA,1,led3);	
		        usleep(30000);//30ms
				led3 = 1;
		        GPIO_SetPin(PA,1,led3);	
		        usleep(30000);//30ms
                break;		           		
	}
}

