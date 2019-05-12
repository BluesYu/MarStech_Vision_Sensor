/******************************************************************************************************
* 函数(Function)：        main()
* 功能(Description)：    测试串口功能
* 调用函数（Calls）：open（）、tcflush()、tcsetattr（）
* 其他(Others): 打开的串口可以调节，波特率可以调节，使用USB转TTL，发送。
* 作者(Author)：Blues Yu
* 联系方式（E-mail）：2549721818@qq.com
********************************************************************************************************/
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include "uart_io.h"

int main()
{
	int fd = UART_Init();
	int tx_length=3;
	unsigned char Read_buffer[256];
	unsigned char tx_buffer[3]={'H','X','R'};
    while(1)
	{
       int temp_len=UART_Read(fd, Read_buffer);
	   if(temp_len>0)
	     printf("%i Read_buffer bytes read : %s\n", temp_len, Read_buffer);
		   
	  
	   int len=UART_Send(fd,(unsigned char*)tx_buffer,tx_length);//int fd, char *send_buf,int data_len
		   printf("len: %d\n", len);
			
	   sleep(1);
    }
    close(fd);
    return 0;
}