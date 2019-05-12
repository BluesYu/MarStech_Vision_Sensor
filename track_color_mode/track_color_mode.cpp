/*****************************************************************************
*  Tracking objects by color                                                   *
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

#include <iostream>
#include <vector>
#include <string>
#include "track_color_mode.h"

using namespace std;
using namespace cv;

bool track_have = false;   // 是否进行了学习

struct ColorSignature
{
	ColorSignature()
	{
		m_uMin = m_uMax = m_uMean = m_vMin = m_vMax = m_vMean = m_type = 0;
	}

	int m_uMin;//u最大
	int m_uMax;//u最小
	int m_uMean;//u平均
	int m_vMin;//v最小
	int m_vMax;//v最大
	int m_vMean;//v平均
	int m_rgb;  //rgb数值
	int m_type;//type数值
}sig;

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
int track_result(Mat &frame, Point &center, Point &w_h)
{
	Mat out = HSVUpdate_track(frame);
	vector<vector<Point>> contours;//有多少轮廓，向量contours就有多少元素
	vector<vector<Point>> contours_temp;//有多少轮廓，向量contours就有多少元素
	vector<Vec4i> hierarchy;//每一个元素的4个int型变量——hierarchy[i][0] ~hierarchy[i][3]，分别表示第i个轮廓的后一个轮廓、前一个轮廓、父轮廓、内嵌轮廓的索引编号
	//CV_RETR_EXTERNAL  只检测最外围轮廓
	//CV_CHAIN_APPROX_NONE 保存物体边界上所有连续的轮廓点到contours向量内   CV_CHAIN_APPROX_SIMPLE  只有拐点信息
	findContours(out, contours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE, Point());
	//cout << "contours.size=" << contours.size() << endl;
	if (contours.size() < 1) return -1;//没有轮廓
	RotatedRect minRect, maxRotatedRect;
	Rect maxRect;//最大的参数
	int max_area = 0;
	//RotatedRect maxRect;
	for (int i = 0; i < contours.size(); i++)
	{
		double area = contourArea(contours[i]);
		if (area > 800)
		{
			minRect = minAreaRect(contours[i]);
			Size2f rect_size = minRect.size;
			int rect_height = minRect.size.height;
			int rect_width = minRect.size.width;
			Point2f center = minRect.center;//外接矩形中心点坐标
			float rate = (float)rect_height / (float)rect_width;
			//cout << rect_height << " " << rect_width << " " << rate << endl;
			if (rect_height < 50 || rect_width < 50 || rate > 2 || rate < 0.05)
				continue;
			if (rect_height*rect_width >= max_area)
			{
				maxRotatedRect = minRect;
				max_area = rect_height*rect_width;
				maxRect = boundingRect(contours[i]);
			}
		}
	}
	if (max_area==0) return -2;//检测不到数据
	center.x = maxRect.x;
	center.y = maxRect.y;
	w_h.x = maxRect.width;
	w_h.y = maxRect.height;

	Point2f vertices[4];      //定义4个点的数组
	maxRotatedRect.points(vertices);   //将四个点存储到vertices数组中
	for (int i = 0; i < 4; i++)//画矩形
		line(frame, vertices[i], vertices[(i + 1) % 4], Scalar(255, 0, 0));

	return max_area;//返回矩形面积
}
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
void calcRatios8(Mat& uu, Mat& vv, struct ColorSignature *sig, float ratios[])
{
	int n = 0, counts[4];
	counts[0] = counts[1] = counts[2] = counts[3] = 0;
	for (int i = 0; i<uu.rows; i++)
	{
		for (int j = 0; j<uu.cols; j++)
		{
			uchar m_u = uu.ptr<uchar>(i)[j];
			uchar m_v = vv.ptr<uchar>(i)[j];
			if (m_u>sig->m_uMin)
				counts[0]++;

			if (m_u<sig->m_uMax)
				counts[1]++;

			if (m_v>sig->m_vMin)
				counts[2]++;

			if (m_v<sig->m_vMax)
				counts[3]++;
			n++;
		}
	}
	// calc ratios
	ratios[0] = (float)counts[0] / n;
	ratios[1] = (float)counts[1] / n;
	ratios[2] = (float)counts[2] / n;
	ratios[3] = (float)counts[3] / n;
	// calc mean (because it's cheap to do it here)
	sig->m_uMean = (sig->m_uMin + sig->m_uMax) / 2;
	sig->m_vMean = (sig->m_vMin + sig->m_vMax) / 2;
}
/******************************************************************************************************
* 函数(Function)：   HSVGet_track(Mat& rgb)
* 功能(Description)：   获取结构体其他参数m_uMin、m_uMax、m_vMin、m_vMax等
* 调用函数（Calls）：cvtColor()、split（）、calcRatios8（）
* 输入参数（parameter）：Mat& rgb：RGB三色矩阵
* 返回数值(return)：     Mat()：处理的矩阵
* 其他(Others):       扩大范围参数range可以调节。
                      track_have=true;//代表完成参数采集
********************************************************************************************************/
Mat HSVGet_track(Mat& rgb)
{
	Mat m_hsv;
	// 先转换为HSV格式
	cvtColor(rgb, m_hsv, cv::COLOR_BGR2HSV);
	// 通道分离
	vector<Mat> splits;
	split(m_hsv, splits);
	//// 对每个通道进行处理
	int scale = 0;
	float ratios[4];
	// 取得hs的上下限
	for (scale = 1 << 7, sig.m_uMin = sig.m_uMax = sig.m_vMin = sig.m_vMax = 0; scale != 0; scale >>= 1)
	{
		calcRatios8(splits[0], splits[1], &sig, ratios);
		if (ratios[0]>track_m_ratio)
			sig.m_uMin += scale;
		else
			sig.m_uMin -= scale;

		if (ratios[1]>track_m_ratio)
			sig.m_uMax -= scale;
		else
			sig.m_uMax += scale;

		if (ratios[2]>track_m_ratio)
			sig.m_vMin += scale;
		else
			sig.m_vMin -= scale;

		if (ratios[3]>track_m_ratio)
			sig.m_vMax -= scale;
		else
			sig.m_vMax += scale;
	}
	int range = 2.5;
	sig.m_uMin = sig.m_uMean + (sig.m_uMin - sig.m_uMean)*range; //扩大范围
	sig.m_uMax = sig.m_uMean + (sig.m_uMax - sig.m_uMean)*range;
	range = 3.5;
	sig.m_vMin = sig.m_vMean + (sig.m_vMin - sig.m_vMean)*range;
	sig.m_vMax = sig.m_vMean + (sig.m_vMax - sig.m_vMean)*range;
	track_have = true;
	return Mat();
}

/******************************************************************************************************
* 函数(Function)：   HSVUpdate_track(Mat& rgb)
* 功能(Description)：   查找在对应范围内的参数
* 调用函数（Calls）：cvtColor()、split（）
* 输入参数（parameter）：Mat& rgb：RGB三色矩阵
* 返回数值(return)：     m_ret：反转后的图像
* 其他(Others):        
********************************************************************************************************/
Mat HSVUpdate_track(Mat& rgb)
{
	Mat m_yuv;
	Mat m_ret;
	// 先转换为YUV格式
	cvtColor(rgb, m_yuv, cv::COLOR_BGR2HSV);
	// 通道分离
	vector<Mat> splits;
	split(m_yuv, splits);
	Mat imgh, imgs;
	inRange(splits[0], Scalar::all(sig.m_uMin), Scalar::all(sig.m_uMax), imgh);
	inRange(splits[1], Scalar::all(sig.m_vMin), Scalar::all(sig.m_vMax), imgs);
	cv::bitwise_and(imgh, imgs, m_ret);
	return m_ret;
}

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
void onMouse_track(Mat &rgb,int event, int x, int y, int flags, void *ustc)
{
	static Point pre_pt(-1, -1);
	static Point cur_pt(-1, -1);
	Rect rect_temp;
	if (event == CV_EVENT_LBUTTONDOWN)
	{
		pre_pt = Point(x, y);
	}
	else
		if (event == CV_EVENT_MOUSEMOVE && (flags & CV_EVENT_FLAG_LBUTTON))
		{
			cur_pt = Point(x, y);
			// rectangle(rgb, pre_pt, cur_pt, Scalar::all(255));
		}
		else if (event == CV_EVENT_LBUTTONUP)
		{
			rect_temp = Rect(pre_pt, cur_pt);
			Mat imgroi = rgb(rect_temp);
			HSVGet_track(imgroi);
		}
}

/******************************************************************************************************
* 函数(Function)：     track_pic_get()
* 功能(Description)：    用于采集参数
* 调用函数（Calls）：cvtColor()、threshold（）、bitwise_not（）
* 输入参数（parameter）：Mat& rgb：RGB三色矩阵
* 返回数值(return)：   return -1:失败
                       return 0:成功
* 其他(Others):
********************************************************************************************************/
int track_pic_get(Mat &pic)
{
	Mat gray, binary, dis_binary,final_result;
	Rect  Rect_temp;
	RotatedRect Rect_dst;

	cvtColor(pic, gray, CV_BGR2GRAY);  //转换为灰度图，要引用头文件opencv/opencv.hpp
	threshold(gray, binary, 128, 255, CV_THRESH_BINARY);//转换为二值图
	//threshold(gray, binary, 0, 255, CV_THRESH_OTSU);//大津法OTSU
	bitwise_not(binary, dis_binary);//颜色反转
	if (track_get_pic(dis_binary, Rect_dst) > 800)//采集的目标足够大
		return -1;

	//final_result = pic(Rect(0, 0, pic.cols, pic.rows));//提取ROI
	Point  center_track;
	Point  w_h_track;
	center_track.x = (int)Rect_dst.center.x;
	center_track.y = (int)Rect_dst.center.y;
	w_h_track.x = (int)Rect_dst.size.width;
	w_h_track.y = (int)Rect_dst.size.height;

	cout << "Rect_dst:" << center_track << " " << w_h_track << endl;

	//Rect_temp = Rect(Rect_dst.x+ Rect_dst.width*0.25, Rect_dst.y+ Rect_dst.height*0.25, Rect_dst.width/2, Rect_dst.height/2);
	if (w_h_track.x > 100 && w_h_track.y > 100)
	{
		/********************防止越界******************************/
		Point point_temp, w_h_temp;
		if (center_track.x - 50 < 0) 
			point_temp.x = 0;
		else
			point_temp.x = center_track.x - 50;
		if (center_track.y - 50 < 0)
			point_temp.y = 0;
		else
			point_temp.y = center_track.y - 50;
		if (center_track.x + 50 > 640)
			w_h_temp.x = (640 - point_temp.x);
		else
			w_h_temp.x = 100;
		if (center_track.y + 50 > 480)
			w_h_temp.y = (480 - point_temp.y);
		else
			w_h_temp.y = 100;
		Rect_temp = Rect(point_temp.x, point_temp.y, w_h_temp.x, w_h_temp.y);
	}
	else
	{
		/********************防止越界******************************/
		int temp_min;
		(w_h_track.x > w_h_track.y) ? (temp_min = w_h_track.y) : (temp_min = w_h_track.x);
		cout << "temp_min" << temp_min << endl;
		Point point_temp, w_h_temp;
		if (center_track.x - temp_min / 2 < 0)
			point_temp.x = 0;
		else
			point_temp.x = center_track.x - temp_min / 2;
		if (center_track.y - temp_min / 2 < 0)
			point_temp.y = 0;
		else
			point_temp.y = center_track.y - temp_min / 2;

		if (point_temp.x + temp_min > 640)
			w_h_temp.x = 640 - point_temp.x;
		else
			w_h_temp.x = temp_min;
		if (point_temp.y + temp_min > 480)
			w_h_temp.y = 480 - point_temp.y;
		else
			w_h_temp.y = temp_min;
		Rect_temp = Rect(point_temp.x, point_temp.y, w_h_temp.x, w_h_temp.y);
	}
	  

	final_result = pic(Rect_temp);//提取ROI
	//imshow("final_result1", final_result);
	HSVGet_track(final_result);
	return 0;
}

/******************************************************************************************************
* 函数(Function)：    track_get_pic(Mat &mat_temp, Rect &Rect_dst)
* 功能(Description)：      用于获取目标图像（切割出目标）
* 调用函数（Calls）：findContours()、boundingRect（）
* 输入参数（parameter）：Mat &mat_temp：原始图像
                         RotatedRect &Rect_dst：目标图像(旋转矩形)
* 返回数值(return)：return=(int)Rect_dst.area()：目标矩形面积
* 其他(Others):
********************************************************************************************************/
int track_get_pic(Mat &mat_temp, RotatedRect &Rect_dst) //生成的范围
{
	vector<vector<Point>> contours;//有多少轮廓，向量contours就有多少元素
	//vector<vector<Point>> contours_temp;//有多少轮廓，向量contours就有多少元素
	vector<Vec4i> hierarchy;//每一个元素的4个int型变量——hierarchy[i][0] ~hierarchy[i][3]，分别表示第i个轮廓的后一个轮廓、前一个轮廓、父轮廓、内嵌轮廓的索引编号
	//CV_RETR_EXTERNAL  只检测最外围轮廓
	//CV_CHAIN_APPROX_NONE 保存物体边界上所有连续的轮廓点到contours向量内   CV_CHAIN_APPROX_SIMPLE  只有拐点信息
	findContours(mat_temp, contours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE, Point());
	int Rect_max_point = 0;
	double area = 0;
	double area_max = 0;
	for (int i = 0; i < contours.size(); i++)
	{
		area = contourArea(contours[i]);
		if (area > 100)
		{
			if (area > area_max)
				Rect_max_point = i;
		}
	}
	Rect_dst = minAreaRect(contours[Rect_max_point]);
	return (int)area;
}
