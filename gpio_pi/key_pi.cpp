/*****************************************************************************
*       Control Key (Input)                                       *
*   Copyright (C) 2019 Blues.Yu  2549721818@qq.com.                          *
*                                                                            *
*  This file is part of key .                                                *
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
*  @file                key_pi.h                                            *
*  @brief               nanopi的key                                         *
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
#include <stdlib.h>  //包含系统函数
#include <unistd.h>  
#include <sys/mman.h>  
#include <sys/types.h>  
#include <sys/stat.h>  
#include <fcntl.h> 
#include <linux/gpio.h> //里面声明io口的操作函数 
#include "gpio_pi.h" 
#include "key_pi.h" 

/******************************************************************************************************
* 函数(Function)：      key_Init()
* 功能(Description)：       按键GPIO初始化设置
* 调用函数（Calls）： 
* 输入参数（parameter）：
* 返回数值(return)：    return = -1：失败
                        return = 0：成功
* 其他(Others):     配置GPIO，用于输入模式
********************************************************************************************************/ 
int key_Init()
{
	if(GPIO_Init() < 0)
	   return -1;
    GPIO_ConfigPin(PA,0,IN);
	GPIO_ConfigPin(PA,6,IN);
	GPIO_ConfigPin(PA,2,IN);
	return 0;
}

/******************************************************************************************************
* 函数(Function)：            key_Press(unsigned int &Mode)
* 功能(Description)：          根据按键改变系统模式
* 调用函数（Calls）： 
* 输入参数（parameter）： unsigned int &Mode：系统模式
* 返回数值(return)：   无
* 其他(Others):     配置GPIO，用于输入模式
********************************************************************************************************/ 
void key_Press(unsigned int &Mode)
  {
	unsigned int key1=0,key2=0,key3=0;//高电平无效
	unsigned int key1_time=0,key2_time=0,key3_time=0;
	unsigned int temp_time=0;
	unsigned int key_flag=0;
	unsigned int key_ready=0;
	
	while(true)
	{

	GPIO_GetPin(PA,0,key1);
	if ((key1==0)&&((key_flag==0)||(key_flag==1)))
	{
		key_flag=1;
		key1_time++;
		usleep(30000);//30ms
		//printf("key1_time=%d \n",key1_time);	
	}
	else
	{
		if(key_flag==1) 
		{
		   key_ready = 1;
		   temp_time=key1_time;
		   key_flag = 0;
		}
		key1_time = 0;
	}
	
	GPIO_GetPin(PA,6,key2);
	if ((key2==0)&&((key_flag==0)||(key_flag==2)))
	{
		key_flag=2;
		key2_time++;
		usleep(30000);//30ms
		//printf("key2_time=%d \n",key2_time);	
	}
	else
	{
		if(key_flag==2) 
		{
		   key_ready = 2;
		   temp_time=key2_time;
		   key_flag = 0;
		}
		key2_time = 0;
	}
	
	GPIO_GetPin(PA,2,key3);
	if ((key3==0)&&((key_flag==0)||(key_flag==3)))
	{
		key_flag=3;
		key3_time++;
		usleep(30000);//30ms
		//printf("key3_time=%d \n",key3_time);	
	}
	else
	{
		if(key_flag==3) 
		{
		   key_ready = 3;
		   temp_time=key3_time;
		   key_flag = 0;
		}
		key3_time = 0;
	}
   usleep(30000);//100ms
	
   if(key_ready>0)  
   {
       printf ("key_ready=%d\n", key_ready);
		switch(key_ready)
		{
			case 1:
					if(temp_time>1&&temp_time<=20)//短按30ms以上~500ms
					   {
						 printf("***Start Sensor*** \n");
						 Mode = 1;//颜色识别模式开启
					   }
				   else  if(temp_time>20)//长按，2s以上
					   {
						  printf("***Close Sensor*** \n");
						 // system("halt");//关机
						 system("poweroff");//关机
					   }
						key_flag = 0;
						key_ready = 0;
					   break;
			case 2:
					if(temp_time>1&&temp_time<=20)//短按30ms以上~500ms
					   {
						 printf("***Start Color Track Mode*** \n");
						 Mode = 2;//颜色追踪模式开启
					   }
				   else  if(temp_time>20)//长按，2s以上
					   {
						 printf("***Start Color Track to Get Target*** \n");
						 Mode = 0x66;//颜色追踪模式采集特征
					   }
						key_flag = 0;
						key_ready = 0;
					   break;
			case 3:
					if(temp_time>1&&temp_time<20)//短按30ms以上~500ms
					   {
						 if(Mode>0&&Mode<6)  //一共有5种模式（分别实现五种功能）
						   Mode++;//颜色追踪模式开启
						   else 
							Mode = 1;
						 if(Mode==6) Mode = 1;//加到末尾，重新开始
						 printf("***Switch Swnsor Mode to：%d*** \n",Mode);
					   }
				   else  if(temp_time>20)//长按，2s以上
					   {
						 Mode = 5;//千分类识别模式
						 printf("***Start 1000-Classification Mode*** \n");
					   }
						key_flag = 0;
						key_ready = 0;
					   break;
			default :  
			        printf("***Key Error , Reboot Key*** \n");
					key_flag = 0;
					key_ready = 0;
					key1_time = 0;
					key2_time = 0;
					key3_time = 0;
		}
	}
	//printf("***Sensor  Mode = %d*** \n", Mode);
  }  
 }
