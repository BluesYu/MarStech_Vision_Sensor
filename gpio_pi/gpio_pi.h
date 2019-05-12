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
	#ifndef __GPIO_H__
	#define __GPIO_H__

/*****************************基本参数和结构体********************************************************/	
	//#define PIO_BASE_ADDRESS	0x01C20800   //全志H3
	#define PIO_BASE_ADDRESS	0x01C20800   //全志H5 
	typedef struct
	{
		unsigned int CFG[4];
		unsigned int DAT ;
		unsigned int DRV0;
		unsigned int DRV1;
		unsigned int PUL0;
		unsigned int PUL1;
	}PIO_Struct;
 
	typedef struct
	{
	PIO_Struct Pn[7];
	}PIO_Map;
 
	typedef enum
	{
		PA = 0,
		PB = 1,
		PC = 2,
		PD = 3,
		PE = 4,
		PF = 5,
		PG = 6,
	}PORT;
 
	typedef enum
	{
		IN			= 0x00,
		OUT			= 0x01,
		AUX			= 0x02,
		INT			= 0x06,
		DISABLE		= 0x07,
	}PIN_MODE;
 
	extern PIO_Map *PIO;

	/******************************************************************************************************
	* 函数(Function)：          GPIO_Init()
	* 功能(Description)：       GPIO初始化
	* 调用函数（Calls）：  open（）、sysconf（）、mmap（）
	* 输入参数（parameter）：
	* 返回数值(return)：return = -1：失败
						return = -1：失败
						return = 0：成功
	* 其他(Others): 注意调节基地址参数
	********************************************************************************************************/
	int GPIO_Init(void);

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
	void GPIO_ConfigPin(PORT port,unsigned int pin,PIN_MODE mode);

	/******************************************************************************************************
	* 函数(Function)：      GPIO_SetPin(PORT port,unsigned int pin,unsigned int level)
	* 功能(Description)：       GPIO设置（输出时候使用）
	* 调用函数（Calls）： 
	* 输入参数（parameter）：PORT port：端口（PA,PB,PC,PD,PE,PF,PG）
							 unsigned int pin:端口引脚（0~16）
							 PIN_MODE mode:设置模式（HIGH= 0x00,LOW= 0x01）
	* 返回数值(return)：  无
	* 其他(Others):     配置GPIO，用于输出模式
	********************************************************************************************************/ 
	void GPIO_SetPin(PORT port,unsigned int pin,unsigned int level);

	/******************************************************************************************************
	* 函数(Function)：      GPIO_GetPin(PORT port,unsigned int pin, unsigned int &(value))
	* 功能(Description)：       GPIO设置（输入时候使用）
	* 调用函数（Calls）： 
	* 输入参数（parameter）：PORT port：端口（PA,PB,PC,PD,PE,PF,PG）
							 unsigned int pin:端口引脚（0~16）
							 unsigned int &(value):反馈参数（HIGH= 0x01,LOW= 0x00）
	* 返回数值(return)：  无
	* 其他(Others):     配置GPIO，用于输入模式
	********************************************************************************************************/ 
	unsigned int GPIO_GetPin(PORT port,unsigned int pin, unsigned int &(value));
	//void GPIO_Free(void);
#endif