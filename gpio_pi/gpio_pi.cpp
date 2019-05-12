/*****************************************************************************
*       Control Gpio (Input && Output)                                       *
*   Copyright (C) 2019 Blues.Yu  2549721818@qq.com.                          *
*                                                                            *
*  This file is part of Multiple Number Recognition .                        *
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
*  @file                gpio_pi.h                                            *
*  @brief               nanopi的GPIO                                         *
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
*                   读取gpio数据                                             *
*----------------------------------------------------------------------------*
*  Change History :                                                          *
*  <2019.5.5>     | 1.0.0.1> | <Blues.Yu>  | <Initializer>                   *
*                                                                            *
*----------------------------------------------------------------------------*
*  2019/05/05 | 1.0.0.1   | Blues.Yu      | Create file    调整了基本地址    *
*----------------------------------------------------------------------------*
*                                                                            *
*****************************************************************************/
#include <stdio.h>  
#include <unistd.h>  
#include <sys/mman.h>  
#include <sys/types.h>  
#include <sys/stat.h>  
#include <fcntl.h> 
#include "gpio_pi.h"  
 
PIO_Map *PIO = NULL;
unsigned int *gpio_map;

/******************************************************************************************************
* 函数(Function)：          GPIO_Init()
* 功能(Description)：       GPIO初始化
* 调用函数（Calls）：  open（）、sysconf（）、mmap（）
* 输入参数（parameter）：
* 返回数值(return)：return = -1：失败
                    return = -2：失败
                    return = 0：成功
* 其他(Others): 注意调节基地址参数
********************************************************************************************************/
int GPIO_Init(void)
{
	unsigned int fd;
	unsigned int addr_start, addr_offset;
	unsigned int PageSize, PageMask;
	
	if((fd = open("/dev/mem",O_RDWR)) == -1)
	{
		printf("open error\r\n");
		return -1;
	}
    printf("open ok\r\n");
	PageSize = sysconf(_SC_PAGESIZE);	//页大小
	PageMask = (~(PageSize-1));			//页掩码
	printf("PageSize:%d,PageMask:%.8X\r\n",PageSize,PageMask);//PageSize:4096,   PageMask:FFFFF000
	printf("PageMask:%d,~PageMask:%.8X\r\n",PageMask,~PageMask);//
 
	addr_start = PIO_BASE_ADDRESS & PageMask;
	addr_offset = PIO_BASE_ADDRESS & ~PageMask;
	printf("addr_start:%.8X,addr_offset:%.8X\r\n",addr_start,addr_offset);//addr_start:01F02000,addr_offset:00000C00
 
	if((gpio_map = (unsigned int *)mmap(NULL,PageSize*2,PROT_READ|PROT_WRITE, MAP_SHARED,fd,addr_start)) == NULL)
	{
		printf("mmap error\r\n");
		close(fd);
		return -2;
	}
	printf("gpio_map:%.8X\r\n",gpio_map);//gpio_map:834F5000

	PIO = (PIO_Map *)((unsigned long)gpio_map + addr_offset);
	printf("PIO:%.8X\r\n",PIO);//PIO:834F5C00
	
	close(fd);
	return 0;
}

/******************************************************************************************************
* 函数(Function)：      GPIO_ConfigPin(PORT port,unsigned int pin,PIN_MODE mode)
* 功能(Description)：       GPIO配置
* 调用函数（Calls）： 
* 输入参数（parameter）：PORT port：端口（PA,PB,PC,PD,PE,PF,PG）
                         unsigned int pin:端口引脚（0~16）
						 PIN_MODE mode:设置模式（IN= 0x00,OUT= 0x01,AUX= 0x02,INT= 0x06,DISABLE= 0x07）
* 返回数值(return)：  无
* 其他(Others):     配置GPIO，主要用到IN= 0x00,OUT= 0x01（输入字母、数字都可以）
********************************************************************************************************/ 
void GPIO_ConfigPin(PORT port,unsigned int pin,PIN_MODE mode)
{
    if (gpio_map == NULL)
        return;
	PIO->Pn[port].CFG[pin / 8] &= ~((unsigned int)0x07 << pin % 8 * 4);
	PIO->Pn[port].CFG[pin / 8] |=  ((unsigned int)mode << pin % 8 * 4);
}

/******************************************************************************************************
* 函数(Function)：      GPIO_SetPin(PORT port,unsigned int pin,unsigned int level)
* 功能(Description)：       GPIO设置（输出时候使用）
* 调用函数（Calls）： 
* 输入参数（parameter）：PORT port：端口（PA,PB,PC,PD,PE,PF,PG）
                         unsigned int pin:端口引脚（0~16）
						 unsigned int level:设置电平高低（HIGH= 0x01,LOW= 0x00）
* 返回数值(return)：  无
* 其他(Others):     配置GPIO，用于输出模式
********************************************************************************************************/ 
void GPIO_SetPin(PORT port,unsigned int pin,unsigned int level)
{
    if (gpio_map == NULL)
        return;
	if(level)
		PIO->Pn[port].DAT |= (1 << pin);
	else
		PIO->Pn[port].DAT &= ~(1 << pin);
}

/******************************************************************************************************
* 函数(Function)：      GPIO_GetPin(PORT port,unsigned int pin, unsigned int &(value))
* 功能(Description)：       GPIO设置（输入时候使用）
* 调用函数（Calls）： 
* 输入参数（parameter）：PORT port：端口（PA,PB,PC,PD,PE,PF,PG）
                         unsigned int pin:端口引脚（0~16）
						 unsigned int &(value):反馈参数（HIGH= 0x01,LOW= 0x00）
* 返回数值(return)：  return = -1：失败
                      return = value：成功
* 其他(Others):     配置GPIO，用于输入模式
********************************************************************************************************/ 
unsigned int GPIO_GetPin(PORT port,unsigned int pin, unsigned int &(value))
{
    if (gpio_map == NULL)
        return -1;
	
	value=((PIO->Pn[port].DAT)>>pin);
	value=value%2;//取出最后一位
	//return __raw_readl(base + 0x04) & (1<< offset);
	return value;
}