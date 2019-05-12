/*****************************************************************************
*     UART    Communication                                              *
*  Copyright (C) 2019 Blues.Yu  2549721818@qq.com.                           *
*                                                                            *
*  This file is part of Color_Mode Code.                                     *
*                                                                            *
*  This program is free software; you can redistribute it and/or modify      *
*  it under the terms of the GNU General Public License version 3 as         *
*  published by the Free Software Foundation.                                *
*                                                                            *
*  You should have received a copy of the GNU General Public License         *
*  along with OST. If not, see <http://www.gnu.org/licenses/>.               *
*                                                                            *
*  Unless required by applicable law or agreed to in writing, software       *
*  distributed under the License is distributed on an "AS IS" BASIS,         *
*  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.  *
*  See the License for the specific language governing permissions and       *
*  limitations under the License.                                            *
*                                                                            *
*  @file          uart_io.h                                                  *
*  @brief         串口通信                                                   *
*  Details.                                                                  *
*                                                                            *
*  @author   Blues.Yu                                                        *
*  @email   2549721818@qq.com                                                *
*  @version  1.0.0.1(版本号)                                                 *
*  @date     2019.5.5                                                        *
*  @license  GNU General Public License (GPL)                                *
*                                                                            *
*----------------------------------------------------------------------------*
*  Remark         : Description                                              *
*                     串口通信                                               *
*----------------------------------------------------------------------------*
*  Change History :                                                          *
*  <2019.5.5>     | 1.0.0.1> | <Blues.Yu>  | <Initializer>                   *
*                                                                            *
*----------------------------------------------------------------------------*
*  2019/05/05 | 1.0.0.1   | Blues.Yu      | Create file         调整参数     *
*----------------------------------------------------------------------------*
*                                                                            *
*****************************************************************************/
#include "uart_io.h"

/******************************************************************************************************
* 函数(Function)：        hex2char_array(unsigned char *buf, unsigned char *dst_buf)
* 功能(Description)：           数组转换
* 调用函数（Calls）：
* 输入参数（parameter）：unsigned char *buf：输入数组
                         unsigned char *dst_buf：数组字符数组
* 返回数值(return)：return = -1：失败
                    return = 0：成功
* 其他(Others): 将0~255数字转换为字符。
********************************************************************************************************/
int hex2char_array(unsigned char *buf, unsigned char *dst_buf)
{
	char decade, Bit;
	if (*buf != ' ')//空格
	{
		//printf("hex:%x\n", *buf);
		/*if (*buf >= 49 && *buf <= 57)
		{*/
			decade = (*buf >> 4) & 0x0f;
			//printf("decade:%x\n", decade);
			if(decade>=0&& decade<=9)
			  decade += 48;
			else
			{
				decade=(decade-10)+97;
			}

			Bit = *buf & 0x0f;
			if (Bit >= 0 && Bit <= 9)
			    Bit += 48;
			else
			{
				Bit = (Bit - 10) + 97;
			}
			*dst_buf = decade;
			dst_buf++;
			*dst_buf = Bit;
			dst_buf++;
			*dst_buf = '\0';
			return 1;
		//}
	}
	return -1;
}

/******************************************************************************************************
* 函数(Function)：        int2char_num(int &num, unsigned char *dst_buf)
* 功能(Description)：           数字转换为字母
* 调用函数（Calls）：
* 输入参数（parameter）：int &num：数字
                         unsigned char *dst_buf：字母数组
* 返回数值(return)：return = -1：失败
                    return = 0：成功
* 其他(Others): 范围：0~999，三位数组
********************************************************************************************************/
int int2char_num(int &num,unsigned char *dst_buf)
{
	char kil,decade, Bit;
	if(num<0||num>999)
		 return -1;
	
	kil= num/100;
	decade=(num%100)/10;
	Bit=(num%100)%10;
	//printf("%d, %d, %d ",kil,decade,Bit);
	if (*dst_buf != '\0')//空格
	{
		kil += 48;
	    *dst_buf = kil;
		dst_buf++;
		decade += 48;
		*dst_buf = decade;
		dst_buf++;
		Bit += 48;
		*dst_buf = Bit;
		dst_buf++;
		*dst_buf = ' ';
		dst_buf++;
		*dst_buf = '\0';
		return 1;
	}
	return -1;
}

/******************************************************************************************************
* 函数(Function)：        UART_Init()
* 功能(Description)：     串口初始化
* 调用函数（Calls）：open（）、tcflush()、tcsetattr（）
* 输入参数（parameter）：无  
* 返回数值(return)：fd = -1：失败
                    fd = 其他参数
* 其他(Others): 打开的串口可以调节，波特率可以调节。
********************************************************************************************************/
int UART_Init()
{
	int fd = -1;
    fd = open("/dev/ttyUSB0", O_RDWR | O_NOCTTY | O_NDELAY);
    if (fd == -1)
    {
        perror("Open Serial Port Error!\n");
        return -1;
    }
    struct termios options;
    tcgetattr(fd, &options);
    //115200, 8N1
    options.c_cflag = B115200 | CS8 | CLOCAL | CREAD;
    options.c_iflag = IGNPAR;
    options.c_oflag = 0;
    options.c_lflag = 0;
    options.c_cc[VTIME]=0;
    options.c_cc[VMIN]=1;
    tcflush(fd, TCIFLUSH);
    tcsetattr(fd, TCSANOW, &options);
	return fd;
}

/******************************************************************************************************
* 函数(Function)：    UART_Send(int fd,  unsigned char *send_buf,int data_len)
* 功能(Description)：      串口数据发送
* 调用函数（Calls）：     write（）
* 输入参数（parameter）：int fd：设备
                         unsigned char *send_buf：发送的数组
						 int data_len：发送的长度
* 返回数值(return)：fd = -1：失败
                    fd = 正数：数组长度
* 其他(Others): 
********************************************************************************************************/
int UART_Send(int fd,  unsigned char *send_buf,int data_len)
{
    int len = 0;
    len = write(fd,send_buf,data_len);
    if (len == data_len )
	{
	return len;
	}     
	else   
	{
	tcflush(fd,TCOFLUSH);
	return -1;
	}
}

/******************************************************************************************************
* 函数(Function)：    UART_Read(int fd,  unsigned char *read_buffer)
* 功能(Description)：      串口数据读取
* 调用函数（Calls）：    read（）
* 输入参数（parameter）：int fd：设备
                         unsigned char *send_buf：读取的数组
* 返回数值(return)：fd = -1：失败
                    fd = 正数：数组长度
* 其他(Others): 
********************************************************************************************************/
int UART_Read(int fd,  unsigned char *read_buffer)
{
	unsigned char rx_buffer[256];//设置的bffer最大数目
	int rx_length = read(fd, (void*)rx_buffer, 255);
	if (rx_length > 0)
      {
		 for(int i=0;i<rx_length;i++)
		 {
		   *read_buffer=rx_buffer[i];
		  // printf("%c ",*read_buffer);
		   read_buffer++; 
		 }
        *read_buffer = '\0';
		rx_buffer[rx_length] = '\0';
		
        printf("%i rx_buffer bytes read : %s\n", rx_length, rx_buffer);
        }
	else
	{
		tcflush(fd,TCOFLUSH);
		return -1;
	}
  return rx_length;
}

/*******************************************************************
* 名称：    set_station
* 功能：    确定位置
* 入口参数：坐标参数（int x, int y）
* 出口参数：对应的位置
*******************************************************************/
int set_station(int x, int y)//根据中心点，判断位置。
{
	if (x < 0 || y < 0 || x>640 || y>480) return 0xff;//错误代码
	if (x < 170)
	{
		if (y < 140)
			return 0;//left_up		
		else if (y < 340)
			return 1;//left
		else
			return 2;//left
	}
	else if (x < 470)
	{
		if (y < 140)
			return 3;//up		
		else if (y < 340)
			return 4;//middle
		else
			return 5;//down
	}
	else if (x <= 640)
	{
		if (y < 140)
			return 6;//right_up		
		else if (y < 340)
			return 7;//right
		else
			return 8;//right_down
	}
}