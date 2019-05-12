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
	#ifndef __KEY_PI_H__
	#define __KEY_PI_H__
/******************************************************************************************************
* 函数(Function)：      key_Init()
* 功能(Description)：       按键GPIO初始化设置
* 调用函数（Calls）： 
* 输入参数（parameter）：
* 返回数值(return)：    return = -1：失败
                        return = 0：成功
* 其他(Others):     配置GPIO，用于输入模式
********************************************************************************************************/ 
int key_Init();

/******************************************************************************************************
* 函数(Function)：            key_Press(unsigned int &Mode)
* 功能(Description)：          根据按键改变系统模式
* 调用函数（Calls）： 
* 输入参数（parameter）： unsigned int &Mode：系统模式
* 返回数值(return)：   无
* 其他(Others):     配置GPIO，用于输入模式
********************************************************************************************************/ 
void key_Press(unsigned int &Mode);

#endif