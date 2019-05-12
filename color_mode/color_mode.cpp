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
#include <iostream>
#include <string>
#include <vector>
#include "color_mode.h"

using namespace cv;
using namespace std;

/******************************************************************************************************
* 函数(Function)：expand_color_image(Mat &srcImg, Mat &dstImg)
* 功能(Description)： 对图像进行形态学处理
* 调用函数（Calls）：erode()，dilate()等
* 输入参数（parameter）：Mat &srcImg：输入图像
					     Mat &dstImg：输出图像
* 返回数值(return)：     无
* 其他(Others):可以修改形态学处理算法模板大小
********************************************************************************************************/
void expand_color_image(Mat &srcImg, Mat &dstImg)
{
	Mat element1 = getStructuringElement(MORPH_RECT, Size(3, 3));//腐蚀
	erode(srcImg, srcImg, element1);
	Mat element2 = getStructuringElement(MORPH_RECT, Size(8, 8));//膨胀
	dilate(srcImg, dstImg, element2);
}

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
int find_color(Mat &src, vector<string> &color_name,vector<Rect> &rect_dst, string color)
{
	expand_color_image(src, src);      //形态学处理
	vector<vector<Point>> contours;    //定义二维（每一维长度可变容器）浮点型向量，存放找到的边界坐标（x，y）
	vector<Vec4i> hirerachy;           //定义的层级，在找边界findContours时自动生成，这里定义只开辟一个空间
	Point2f vertices[4];               //定义4个点的数组
	//Mat get_pic = getStructuringElement(MORPH_RECT, Size(5, 5), Point(-1, -1));   //返回指定形状（矩形）和尺寸（5x5）的结构元素
	//morphologyEx(src, src, MORPH_OPEN, get_pic);  //形态学变换函数，滤波，开运算：先腐蚀后膨胀，消除小的干扰块
	findContours(src, contours, hirerachy, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);  //寻找轮廓边界，存储于contours中
	if (contours.size() < 1) return -1;
	int contours_time=0;
	for (size_t t = 0; t < contours.size(); t++)
	{
		double area = contourArea(contours[t]);
		if (area < 2500) continue;//面积小于2500,滤波处理
		Rect rect_temp = boundingRect(contours[t]);
		rect_dst.push_back(rect_temp);
		color_name.push_back(color);
		contours_time++;
	}
	return contours_time;
}

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
int get_color_num(int &num)
{
	if (num >h_min[0] && num < h_max[0] || num>h_min[1] && num <  h_max[1])
		return 1;//红色
	 else if (num >h_min[2] && num <  h_max[2])
		 return 2;//黄色
	     else if (num > h_min[3] && num < h_max[3])
		      return 3;//绿色
			 else if (num >h_min[4] && num <  h_max[4])
				 return 4;//蓝色
				 else if (num > h_min[5] && num <  h_max[5])
					 return 5;//紫色
	return -1;//失败
}

/******************************************************************************************************
* 函数(Function)：get_color_pic(Mat &src, Mat &RD, Mat &YL, Mat &GN, Mat &BU, Mat &PL)
* 功能(Description)：获取五种颜色的模板
* 调用函数（Calls）：cvtColor()，get_color_num()等
* 输入参数（parameter）：Mat &src：输入图像   Mat &RD：红色
*					     Mat &YL：黄色        Mat &GN：绿色
*					     Mat &BU：蓝色        Mat &PL：紫色
* 返回数值(return)： return =-1：检测失败，参数范围溢出
*                    return = 0：检测成功
* 其他(Others):图像的参数：S(饱和度)和V(明度)，范围可以调节，主要用于调节背景颜色（黑、白、灰）
********************************************************************************************************/
int get_color_pic(Mat &src, Mat &RD, Mat &YL, Mat &GN, Mat &BU, Mat &PL)//"RD", "RD", "YL", "GN", "BU", "PL"
{
	Mat hsv;
	//Mat img_h,img_s,img_v;
   cvtColor(src, hsv, COLOR_BGR2HSV);   //从RGB色彩空间转换到HSV色彩空间
   //imshow("hsv", hsv);
   for (int j = 0; j < src.rows; j++)//行(height)
	   for (int i = 0; i<src.cols; i++) //列(width)
	   {
		   if (hsv.at<Vec3b>(j, i)[1]<= s_min[0] || hsv.at<Vec3b>(j, i)[1] >= s_max[0] || hsv.at<Vec3b>(j, i)[2] <= v_min[0] || hsv.at<Vec3b>(j, i)[2] >= v_max[0])
			  continue;
		   //cout <<" h:" << (int)hsv.at<Vec3b>(j, i)[0] << " ";
		   int num = (int)hsv.at<Vec3b>(j, i)[0];
		   int h_temp = get_color_num(num);
		   //cout <<"num:"<<(int) h_temp << " ";
		   if (h_temp != -1)
			   switch (h_temp)
			   {
			   case 1:  RD.at<uchar>(j, i) = 255; break;
			   case 2:  YL.at<uchar>(j, i) = 255; break;
			   case 3:  GN.at<uchar>(j, i) = 255; break;
			   case 4:  BU.at<uchar>(j, i) = 255; break;
			   case 5:  PL.at<uchar>(j, i) = 255; break;
			   default:return -1;//错误
			   }
	   }
   return 0;
}

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
int get_color_mode(Mat &frame, vector<string> &color_name, vector<Point> &Rect_center, vector<Point> &Rect_size)
{
	Mat RD = Mat::zeros(frame.rows, frame.cols, CV_8UC1);
	Mat YL = Mat::zeros(frame.rows, frame.cols, CV_8UC1);
	Mat GN = Mat::zeros(frame.rows, frame.cols, CV_8UC1);
	Mat BU = Mat::zeros(frame.rows, frame.cols, CV_8UC1);
	Mat PL = Mat::zeros(frame.rows, frame.cols, CV_8UC1);
	vector<Rect> rect_dst;
	//cout << "size:" << RD.cols << " " << RD.rows << endl;//(640,480)
	if (get_color_pic(frame, RD, YL, GN, BU, PL) == -1)
		return -1;//图像获取失败
	find_color(RD, color_name, rect_dst, "RD");
	find_color(YL, color_name, rect_dst, "YL");
	find_color(GN, color_name, rect_dst, "GN");
	find_color(BU, color_name, rect_dst, "BU");
	find_color(PL, color_name, rect_dst, "PL");

	if(color_name.size()<1) return -2;//无轮廓
	Point Rect_center_temp, Rect_size_temp;
	for (int j = 0; j < color_name.size(); j++)
	{
		Rect_center_temp.x = rect_dst[j].x + rect_dst[j].width / 2;
		Rect_center_temp.y = rect_dst[j].y + rect_dst[j].height / 2;
		Rect_size_temp.x = rect_dst[j].width;
		Rect_size_temp.y = rect_dst[j].height;
		Rect_center.push_back(Rect_center_temp);
		Rect_size.push_back(Rect_size_temp);
		//cout << color_name[j] << " " << Rect_center_temp.x << " " << Rect_center_temp.y << " " \
		//	<< Rect_size_temp.x << " " << Rect_size_temp.y << endl;
		//rectangle(frame, rect_dst[j].tl(), rect_dst[j].br(), Scalar(0, 255, 0));//随机颜色 
		//putText(frame, color_name[j], Rect_center_temp, FONT_HERSHEY_SIMPLEX, 2, Scalar(0, 0, 255), 4, 8);
	}
	RD.release();
	YL.release();
	GN.release();
	BU.release();
	PL.release();
	return color_name.size();
}


// VideoCapture capture(0);   // 打开摄像头

// int main(int argc, char** argv) {
	// if (!capture.isOpened())
	// {
		// printf("could not load camera...\n");
		// return -1;
	// }
	// Mat frame;
	// while (true) {
		// bool ret = capture.read(frame); //读取摄像头的一帧图像
		// if (!ret) continue;
		//imshow("SRC", frame);
		//waitKey(10);
		// vector<string> color_name;
		// vector<Point> Rect_center;
		// vector<Point> Rect_size;
		// if (get_color_mode(frame, color_name, Rect_center, Rect_size) > 0)//get_color_mode(frame, color_name, Rect_center, Rect_size);
		// {
			// for(int i=0;i<color_name.size();i++)
			// cout << color_name[i] << " " << Rect_center[i].x << " " << Rect_center[i].y << " " \
				// << Rect_size[i].x << " " << Rect_size[i].y << endl;
			/*imshow("output", frame);
			waitKey(10);*/
		// }
		// }
	// return 0;
// }