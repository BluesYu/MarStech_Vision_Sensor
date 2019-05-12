#include <stdio.h>  
#include <unistd.h>  
#include <sys/mman.h>  
#include <sys/types.h>  
#include <sys/stat.h>  
#include <fcntl.h> 
#include <linux/gpio.h> //里面声明io口的操作函数 
#include "gpio_pi.h" 
#include "led_pi.h" 

int main()
{
led_Init();
while(1)
{
camera_led_Set(1);
led_Show(0);
// led_Show(1);
// led_Show(2);
// led_Show(3);
// led_Show(4);
// led_Show(5);
// led_Show(0x66);
// led_Show(0x44);
// led_Show(6);
}
return 0;
}