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
*                      串口通信                                              *
*----------------------------------------------------------------------------*
*  Change History :                                                          *
*  <2019.5.5>     | 1.0.0.1> | <Blues.Yu>  | <Initializer>                   *
*                                                                            *
*----------------------------------------------------------------------------*
*  2019/05/05 | 1.0.0.1   | Blues.Yu      | Create file         调整参数     *
*----------------------------------------------------------------------------*
*                                                                            *
*****************************************************************************/
#ifndef __uart_io_H
#define __uart_io_H 

#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>

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
int hex2char_array(unsigned char *buf, unsigned char *dst_buf);

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
int int2char_num(int &num,unsigned char *dst_buf);

/******************************************************************************************************
* 函数(Function)：        UART_Init()
* 功能(Description)：     串口初始化
* 调用函数（Calls）：open（）、tcflush()、tcsetattr（）
* 输入参数（parameter）：无  
* 返回数值(return)：fd = -1：失败
                    fd = 其他参数
* 其他(Others): 打开的串口可以调节，波特率可以调节。
********************************************************************************************************/
int UART_Init();

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
int UART_Send(int fd,  unsigned char *send_buf,int data_len);

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
int UART_Read(int fd,  unsigned char *read_buffer);


#endif