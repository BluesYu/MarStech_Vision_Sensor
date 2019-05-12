/*****************************************************************************
*  Tracking objects by color                                                 *
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
*  @file        track_color_mode.h                                           *
*  @brief      根据颜色对物体进行跟踪                                        *
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
*                  根据颜色实现对物体的跟踪                                  *
*----------------------------------------------------------------------------*
*  Change History :                                                          *
*  <2019.5.5>     | 1.0.0.1> | <Blues.Yu>  | <Initializer>                   *
*                                                                            *
*----------------------------------------------------------------------------*
*  2019/05/05 | 1.0.0.1   | Blues.Yu      | Create file  track_get_pic()     *
*----------------------------------------------------------------------------*
*                                                                            *
*****************************************************************************/

#ifndef __track_color_mode_H
#define __track_color_mode_H 

#include <opencv2/opencv.hpp>

using namespace cv;

float const track_m_ratio = 0.8; // 选中区域的hs通道大于百分之多少算好

extern bool track_have;   // 是否进行了学习

/******************************************************************************************************
* 函数(Function)：    track_result(Mat &mat_temp, Point &center,Point &w_h)
* 功能(Description)：      获取参数
* 调用函数（Calls）：HSVUpdate_track（）、findContours()、boundingRect（）、minAreaRect（）
* 输入参数（parameter）：Mat &frame：输入图像
                         Point &center;矩形中心点
						 Point &w_h:矩形长和宽
* 返回数值(return)：return=-1：没有检测轮廓
                    return=-2：检测到轮廓过小，滤除
					return=正数：反馈矩形面积，成功
* 其他(Others): 参数可以调。
********************************************************************************************************/
int track_result(Mat &frame, Point &center, Point &w_h);

/******************************************************************************************************
* 函数(Function)：calcRatios8(Mat& uu, Mat& vv, struct ColorSignature *sig, float ratios[])
* 功能(Description)：   获取参数m_uMean（u平均）、m_vMean（v平均）
* 调用函数（Calls）：
* 输入参数（parameter）：Mat& uu：图像uu
                         Mat& vv：图像vv
						 struct ColorSignature *sig：参数结构体
						 float ratios[]：4维矩阵
* 返回数值(return)：    无
* 其他(Others):
********************************************************************************************************/
void calcRatios8(Mat& uu, Mat& vv, struct ColorSignature *sig, float ratios[]);

/******************************************************************************************************
* 函数(Function)：   HSVGet_track(Mat& rgb)
* 功能(Description)：   获取结构体其他参数m_uMin、m_uMax、m_vMin、m_vMax等
* 调用函数（Calls）：cvtColor()、split（）、calcRatios8（）
* 输入参数（parameter）：Mat& rgb：RGB三色矩阵
* 返回数值(return)：     Mat()：处理的矩阵
* 其他(Others):       扩大范围参数range可以调节。
                      track_have=true;//代表完成参数采集
********************************************************************************************************/
Mat HSVGet_track(Mat& rgb);

/******************************************************************************************************
* 函数(Function)：   HSVUpdate_track(Mat& rgb)
* 功能(Description)：   查找在对应范围内的参数
* 调用函数（Calls）：cvtColor()、split（）
* 输入参数（parameter）：Mat& rgb：RGB三色矩阵
* 返回数值(return)：     m_ret：反转后的图像
* 其他(Others):        
********************************************************************************************************/
Mat HSVUpdate_track(Mat& rgb);

/******************************************************************************************************
* 函数(Function)：   onMouse(Mat &rgb,int event, int x, int y, int flags, void *ustc, Rect &rect_temp )
* 功能(Description)：  获取目标图像的矩形的大小，捕捉鼠标的参数
* 调用函数（Calls）：
* 输入参数（parameter）：Mat &rgb：输入图像
                         int event：鼠标点击事件
						 int x：鼠标x坐标
						 int y：鼠标y坐标
						 int flags：标记位
						 void *ustc：标记位
						 Rect &rect_temp：矩形
* 返回数值(return)：         无
* 其他(Others):
********************************************************************************************************/
void onMouse_track(Mat &rgb,int event, int x, int y, int flags, void *ustc);

/******************************************************************************************************
* 函数(Function)：     track_pic_get()
* 功能(Description)：    用于采集参数
* 调用函数（Calls）：cvtColor()、threshold（）、bitwise_not（）
* 输入参数（parameter）：Mat& rgb：RGB三色矩阵
* 返回数值(return)：   return -1:失败
                       return 0:成功
* 其他(Others):
********************************************************************************************************/
int track_pic_get(Mat &pic);

/******************************************************************************************************
* 函数(Function)：    track_get_pic(Mat &mat_temp, Rect &Rect_dst)
* 功能(Description)：      用于获取目标图像（切割出目标）
* 调用函数（Calls）：findContours()、boundingRect（）
* 输入参数（parameter）：Mat &mat_temp：原始图像
                         RotatedRect &Rect_dst：目标图像(旋转矩形)
* 返回数值(return)：return=(int)Rect_dst.area()：目标矩形面积
* 其他(Others):
********************************************************************************************************/
int track_get_pic(Mat &mat_temp, RotatedRect &Rect_dst); //生成的范围

#endif