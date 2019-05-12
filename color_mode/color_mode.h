/*****************************************************************************
*  Color Recognition Mode                                                    *
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
*  @file        color_mode.h                                                 *
*  @brief    对图像进行处理，识别五种颜色                                    *
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
*                  通过滤波、形态学处理等方法进行图像预处理，然后采用HSV域的 *
* 参数进行图像颜色处理，最后间数据发送给串口。                               *
*     可以识别红色、黄色、绿色、蓝色、紫色五种颜色。                         *
*----------------------------------------------------------------------------*
*  Change History :                                                          *
*  <2019.5.5>     | 1.0.0.1> | <Blues.Yu>  | <Initializer>                   *
*                                                                            *
*----------------------------------------------------------------------------*
*  2019/05/05 | 1.0.0.1   | Blues.Yu      | Create file  expand_color_image()*
*----------------------------------------------------------------------------*
*                                                                            *
*****************************************************************************/
#ifndef __color_mode_H
#define __color_mode_H
#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
using namespace cv;
using namespace std;

/**********************************阈值参数**********************************************************/
//参数："RD", "RD", "YL", "GN", "BU", "PL"
unsigned int const h_min[6] = { 0,156,30,35,105,125 };
unsigned int const h_max[6] = { 8,180,34,77,115,155 };
unsigned int const s_min[6] = { 43,43,43,43,43,43 };
unsigned int const s_max[6] = { 255,255,255,255,255,255 };
unsigned int const v_min[6] = { 46,46,46,46,46,46 };
unsigned int const v_max[6] = { 255,255,255,255,255,255 };
//string const COLOR_NAME[6] = { "RD","RD","YL","GN","BU","PL" };

/******************************************************************************************************
* 函数(Function)：find_color(Mat &src, vector<string> &color_name,vector<Rect> &rect_dst, string color)
* 功能(Description)： 查找目标图片的轮廓，标记颜色
* 调用函数（Calls）：findContours()等
* 输入参数（parameter）：Mat &src：输入图像
                     vector<string> &color_name：保存轮廓的颜色数组
					 vector<Rect> &rect_dst：保存轮廓矩形数组
					 string color：颜色标记
* 返回数值(return)： -1：检测失败，即没有轮廓
                     正数：轮廓的个数
* 其他(Others):可以添加形态学处理算法
********************************************************************************************************/
int find_color(Mat &src, vector<string> &color_name, vector<Rect> &rect_dst, string color);

/******************************************************************************************************
* 函数(Function)：expand_color_image(Mat &srcImg, Mat &dstImg)
* 功能(Description)： 对图像进行形态学处理
* 调用函数（Calls）：erode()，dilate()等
* 输入参数（parameter）：Mat &srcImg：输入图像
					     Mat &dstImg：输出图像
* 返回数值(return)：     无
* 其他(Others):可以修改形态学处理算法模板大小
********************************************************************************************************/
void expand_color_image(Mat &srcImg, Mat &dstImg);

/******************************************************************************************************
* 函数(Function)get_color_pic(Mat &src, Mat &RD, Mat &YL, Mat &GN, Mat &BU, Mat &PL)
* 功能(Description)：获取五种颜色的模板
* 调用函数（Calls）：cvtColor()，get_color_num()等
* 输入参数（parameter）：Mat &src：输入图像   Mat &RD：红色
*					     Mat &YL：黄色        Mat &GN：绿色
*					     Mat &BU：蓝色        Mat &PL：紫色
* 返回数值(return)： return =-1：检测失败，参数范围溢出
*                    return = 0：检测成功
* 其他(Others):图像的参数：S(饱和度)和V(明度)，范围可以调节，主要用于调节背景颜色（黑、白、灰）
********************************************************************************************************/
int get_color_pic(Mat &src, Mat &RD, Mat &YL, Mat &GN, Mat &BU, Mat &PL);

/******************************************************************************************************
* 函数(Function)：  get_color_num(int &num)
* 功能(Description)：根据H（色调）数值，判断对应的颜色区间
* 调用函数（Calls）：无
* 输入参数（parameter）：int &num：输入H（色调）数值。
* 返回数值(return)：     return =-1：检测失败，五种颜色
*                        return = 1：红色
*                        return = 2：黄色
*                        return = 3：绿色
*                        return = 4：蓝色
*                        return = 5：紫色
* 其他(Others):默认的阈值是：h_min[6] = { 0,156,30,35,105,125 };
						     h_max[6] = { 8,180,34,77,124,155 };
			   阈值范围可以调节。
********************************************************************************************************/
int get_color_num(int &num);

/******************************************************************************************************
* 函数(Function)：get_color_mode(Mat &framem, vector<string> color_name, vector<Point> Rect_center, vector<Point> Rect_size)
* 功能(Description)：获取五种颜色的模板
* 调用函数（Calls）：get_pic()，find_color()等
* 输入参数（parameter）：Mat &frame:输入图像
*                        vector<string> &color_name:检测颜色数组
*					     vector<Point> &Rect_center:对应中心坐标
*					     vector<Point> &Rect_size:矩形尺寸（长和宽）
* 返回数值(return)：     return =-1：图像获取失败
*                        return =-2：未监测到轮廓
*                        return = 正数：检测得到参数
* 其他(Others):
********************************************************************************************************/
int get_color_mode(Mat &frame, vector<string> &color_name, vector<Point> &Rect_center, vector<Point> &Rect_size);

#endif