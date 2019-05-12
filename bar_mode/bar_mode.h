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
#ifndef __bar_mode_H
#define __bar_mode_H

#include <opencv2/opencv.hpp>
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"

using namespace cv;
/******************************************************************************************************
* 函数(Function)：get_bar_num(Mat &srcImg, string &result)
* 功能(Description)： 获取参数
* 调用函数（Calls）：cvtColor()、threshold（）、bitwise_not（）、expand_dis_binary_image（）、
                     get_bar_contours（）、get_bar（）、getDigitsEAN13（）、parseEAN13（）
* 输入参数（parameter）：Mat &srcImg：输入图像（反转后的二值图像）
                         string &result：输出结果
* 返回数值(return)：     return="ERROR"：错误
                         return=result ：条形码数据
* 其他(Others):     bar_rate可以调节，代表取值的参数，每张图一共扫描5次。
********************************************************************************************************/
string get_bar_num(Mat &srcImg, string &result);

/******************************************************************************************************
* 函数(Function)：expand_dis_binary_image(Mat &srcImg, Mat &dstImg)
* 功能(Description)： 将目标图像扩大，检测区域
* 调用函数（Calls）：erode()、dilate（）
* 输入参数（parameter）：Mat &srcImg：输入图像（反转后的二值图像）
                         Mat &dstImg：输出图像
* 返回数值(return)：     无
* 其他(Others):      可以修改模板，形态学模板越大计算越多。
********************************************************************************************************/
void expand_dis_binary_image(Mat &srcImg, Mat &dstImg);

/******************************************************************************************************
* 函数(Function)：get_bar_contours(Mat &mat_temp, vector<vector<Point2f>>  &vertices)
* 功能(Description)： 将目标图像扩大，检测区域
* 调用函数（Calls）：findContours()、contourArea（）
* 输入参数（parameter）：Mat &mat_temp：输入图像（反转后的二值图像）
                         vector<Point2f>  &vertices)：输出旋转矩形参数点
* 返回数值(return)：     返回0：无轮廓
                         返回4：代表4个点                    
* 其他(Others):      
********************************************************************************************************/
int get_bar_contours(Mat &mat_temp, vector<Point2f>  &vertices) ;

/******************************************************************************************************
* 函数(Function)：getDigitsEAN13(IplImage* source, int lineHight)
* 功能(Description)： 获取矩形代码（0或者1）
* 调用函数（Calls）：getBaseAndStart()、cvGet2D（）、getNumOfPixel()
* 输入参数（parameter）：IplImage* source:输入图像（灰度图）
                         int lineHight:获取的Y轴图像
* 返回数值(return)：     return="ERROR"：失败
                          return=code（0、1代码）：成功   
* 其他(Others):          调整参数lineHight
********************************************************************************************************/
string getDigitsEAN13(IplImage* source, int lineHight);

/******************************************************************************************************
* 函数(Function)： getNumOfPixel(IplImage* source, int lineHight, int threshold, int* index)
* 功能(Description)： 统计相同类型（条/空白）的像素的个数	
* 调用函数（Calls）：
* 输入参数（parameter）：IplImage* source:输入图像（灰度图）
                         int lineHight:获取的Y轴图像
                         int threshold：二值化阈值
						 int* index：起始点（x轴）
* 返回数值(return)：     return="ERROR1"：失败
return=code（0、1代码）：成功
* 其他(Others):          调整参数lineHight
********************************************************************************************************/
int getNumOfPixel(IplImage* source, int lineHight, int threshold, int* index) ;

/******************************************************************************************************
* 函数(Function)： getBaseAndStart(IplImage* source, int lineHight, int threshold, int* base, int* start)
* 功能(Description)：获取条形码起始的位置和宽度
* 调用函数（Calls）：cvGet2D()
* 输入参数（parameter）：IplImage* source:输入图像（灰度图）
						 int lineHight:获取的Y轴图像
						 int threshold：二值化阈值
						 int* base:基础宽度
						 int* start：起始的位置
* 返回数值(return)：     return=-2：失败
                         return=-1：失败
						 return= 0：成功，返回参数
* 其他(Others):          判断宽度的算法还可以改进
********************************************************************************************************/
int getBaseAndStart(IplImage* source, int lineHight, int threshold, int* base, int* start) ;

/******************************************************************************************************
* 函数(Function)：get_bar(Mat &gray, vector<Point2f>  &vertices,Mat &final_result,const int &add_pixel)
* 功能(Description)： 获取目标图像
* 调用函数（Calls）：temp_expand()、warpAffine（）
* 输入参数（parameter）：Mat &gray：输入灰度图像（二值图像也可）
vector<Point2f>  &vertices：输入矩形点
Mat &final_result：输出的目标图像
const int &add_pixel：扩张图像（用于旋转）
* 返回数值(return)：     return=-1：失败，无图像
                         return= 4：正常
* 其他(Others):          add_pixel参数可以修改，根据图像进行调整。
********************************************************************************************************/
int get_bar(Mat &gray, vector<Point2f>  &vertices, Mat &final_result, const int &add_pixel);

/******************************************************************************************************
* 函数(Function)： parseEAN13(string code) 
* 功能(Description)：解析EAN13的间接code
* 调用函数（Calls）：
* 输入参数（parameter）：string code:根据得到的参数，进行解析
* 返回数值(return)：     return=code（数字）：成功
					     return= "ERROR1"：参数范围错误
						 return= "ERROR2"：参数解析错误
						 return= "ERROR3"：第一个参数解析错误
						 return= "ERROR4"：校验失败
* 其他(Others):          基本参数不需要修改
********************************************************************************************************/
string parseEAN13(string code) ;

#endif