/*****************************************************************************
*  Bar Code Recognition Algorithms                                                   *
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
*  @file        bar_mode.h                                                 *
*  @brief    识别条形码，可以旋转图像                                    *
*  Details.                                                                  *
*                                                                            *
*  @author   Blues.Yu                                                        *
*  @email   2549721818@qq.com                                                *
*  @version  1.0.0.1(版本号)                                                 *
*  @date     2019.5.1                                                        *
*  @license  GNU General Public License (GPL)                                *
*                                                                            *
*----------------------------------------------------------------------------*
*  Remark         : Description                                              *
*     通过滤波、形态学处理等方法进行图像预处理，然后对图像进行随机旋转校正   *
* 然后对条形码进行识别。                                                     *
*----------------------------------------------------------------------------*
*  Change History :                                                          *
*  <2019.5.1>     | 1.0.0.1> | <Blues.Yu>  | <Initializer>                   *
*                                                                            *
*----------------------------------------------------------------------------*
*  2019/05/05 | 1.0.0.1   | Blues.Yu      | Create file  get_bar()           *
*----------------------------------------------------------------------------*
*                                                                            *
*****************************************************************************/
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <string.h>
#include <iostream>
#include <unordered_map>
#include "bar_mode.h"

using namespace std;
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
string get_bar_num(Mat &srcImg, string &result)
{
	Mat gray, binary, dis_binary, mat_temp, final_result;
	string codeDigit = ""; //条形码参数
	float bar_rate[5] = { 0.2,0.35,0.5,0.65,0.8 };//条形码检测位置
	//Mat final_result;
	cvtColor(srcImg, gray, CV_BGR2GRAY);  //转换为灰度图，要引用头文件opencv/opencv.hpp
	threshold(gray, binary, 100, 255, CV_THRESH_BINARY);//转换为二值图
	bitwise_not(binary, dis_binary);//颜色反转

	expand_dis_binary_image(dis_binary, mat_temp);//形态学扩大图像
	vector<Point2f>  vertices;   //生成的范围
	if(get_bar_contours(mat_temp, vertices)==0)//生成的范围
		return "ERROR";
	if (get_bar(gray, vertices, final_result, 1280) == -1)
		return "ERROR";
	//IplImage* img;
	IplImage temp = (IplImage)final_result;
	IplImage* img = &temp;
	//cout << "img->height=" << img->height << " img->width=" << img->width << endl;
	for (int i = 0; i < 5; i++)//检测五次
	{
		codeDigit = getDigitsEAN13(img, (img->height)*bar_rate[i]);
		if (codeDigit.size() < 50)//参数过小，为错
			continue;
		cout << "codeDigit:" << codeDigit << "  " << "size:" << codeDigit.size() << endl;
		if (codeDigit.length() < 95 && codeDigit.length() > 92)//添加“101”分作为结尾
		{
			while (95 - codeDigit.length() > 0)
			{
				codeDigit.push_back('0');
			}
			codeDigit[93] = '0';
			codeDigit[94] = '1';
			codeDigit[95] = '0';
		}

		result = parseEAN13(codeDigit);
		//cout << "result:" << result << endl;
		if (result[0] >= 48 && result[0] <= 57)//数字参数,不是ERROR
		{
			//cout << "codeDigit:" << codeDigit << "  " << "size:" << codeDigit.size() << endl;
			//cout <<"result:" << result << endl;
			return result;
		}
	}
	return "ERROR";
}

/******************************************************************************************************
* 函数(Function)：expand_dis_binary_image(Mat &srcImg, Mat &dstImg)
* 功能(Description)： 将目标图像扩大，检测区域
* 调用函数（Calls）：erode()、dilate（）
* 输入参数（parameter）：Mat &srcImg：输入图像（反转后的二值图像）
                         Mat &dstImg：输出图像
* 返回数值(return)：     无
* 其他(Others):      可以修改模板，形态学模板越大计算越多。
********************************************************************************************************/
void expand_dis_binary_image(Mat &srcImg, Mat &dstImg)
{
	Mat element1 = getStructuringElement(MORPH_RECT, Size(3, 3));//腐蚀
	erode(srcImg, srcImg, element1);
	Mat element2 = getStructuringElement(MORPH_RECT, Size(13, 13));//膨胀
	dilate(srcImg, dstImg, element2);
}

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
int get_bar_contours(Mat &mat_temp, vector<Point2f>  &vertices) 
{
	vector<vector<Point>> contours;//有多少轮廓，向量contours就有多少元素
	vector<Vec4i> hierarchy;//每一个元素的4个int型变量——hierarchy[i][0] ~hierarchy[i][3]，分别表示第i个轮廓的后一个轮廓、前一个轮廓、父轮廓、内嵌轮廓的索引编号
	//CV_RETR_EXTERNAL  只检测最外围轮廓
	//CV_CHAIN_APPROX_NONE 保存物体边界上所有连续的轮廓点到contours向量内   CV_CHAIN_APPROX_SIMPLE  只有拐点信息
	findContours(mat_temp, contours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE, Point());
	RotatedRect minRect;         //旋转矩形
	Point2f fourPoint2f[4];      //定义4个点的数组
	double area=0;
	double max_area=0;
	for (int i = 0; i < contours.size(); i++)
	{
		//cout << "contours[i].size()=" << contours[i].size() << endl;
		area = contourArea(contours[i]);
		if (area > 100)//大于100大小
		{
			minRect = minAreaRect(contours[i]);
			Size2f rect_size = minRect.size;
			int rect_height = minRect.size.height;
			int rect_width = minRect.size.width;
			float rate = (float)rect_height / (float)rect_width;
			//cout << rect_height << " " << rect_width << " " << rate << endl;
			if (rect_height < 150 || rect_width < 150 || rate > 2 || rate < 0.05)
				continue;
			if (area > max_area)
			{
				max_area = area;
				minRect.points(fourPoint2f);   //将四个点存储到vertices数组中
			    //cout << fourPoint2f[0] << fourPoint2f[1] << fourPoint2f[2] << fourPoint2f[3] << endl;
				//vector<Point2f> tempPoint;//临时的文件
				for (int j = 0; j < 4; j++)
					if (vertices.size() < 4)//初始化为空
					    vertices.push_back(fourPoint2f[j]);
					    else if (vertices.size() == 4)
						     vertices[j] = fourPoint2f[j];
			}
		}
	}
	return vertices.size();
}

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
string getDigitsEAN13(IplImage* source, int lineHight) 
{
	int height = source->height;
	int width = source->width;
	int start = 0;
	int base = 0;
	int threshold = 0;
	//int lineHight = height/2;//默认采取图像中间的一条线来提取间接code 
	//lineHight = lineHight - 10;
	int sum_gray=0;
	int threshold_average = 0;
	for (int i = 0; i < (width / 3); i++)
	{
		CvScalar scalar = cvGet2D(source, lineHight, i);
		//int temp = 0.114 * scalar.val[0] + 0.587 * scalar.val[1] + 0.299 * scalar.val[2];//BGR转化为灰度图像公式
		int temp = (scalar.val[0] + scalar.val[1] + scalar.val[2]) / 3;//BGR转化为灰度图像公式
		sum_gray += temp;
	}
	threshold = sum_gray / (width / 3);
	//cout << "average =" << threshold<<endl;
	string code = "";
	if (getBaseAndStart(source, lineHight, threshold, &base, &start) < 0)
	{
		//cout << getBaseAndStart(source, lineHight, threshold, &base, &start) << endl;
		return "ERROR";
	}
	if(base<=1) return "ERROR";
	//cout << "base=" << base << " start=" << start << endl;
	int index = start;//起始点
	
	while (1)
	{
		//if (code.length() >= 95)//小于95参数
		if (code.length() >= 130)//大于130个参数
			break;
		if (index >= width - 5)//宽度-10
			break;

		CvScalar scalar = cvGet2D(source, lineHight, index);
		//int temp = 0.114 * scalar.val[0] + 0.587 * scalar.val[1] + 0.299 * scalar.val[2];//BGR转化为灰度图像公式
		int temp = (scalar.val[0] + scalar.val[1] + scalar.val[2]) / 3;//BGR转化为灰度图像公式
		int bias = temp - threshold;//用来判断为空白还是条纹
		int numOfPixel = getNumOfPixel(source, lineHight, threshold, &index);//统计相同类型（条/空白）的像素的个数			
		int numOfDigit = (int)((double)numOfPixel / base + 0.5);//在EAN13中代表宽条代表几个数字（0/1）
		if (numOfDigit >= 1)
		{
			for (int j = 0; j < numOfDigit; j++)
			{//采用循环来显示多个相同数字
				if (bias < 0)
					code = code + "1";
				else
					code = code + "0";
			}
		}
	}
	return code;
}

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
int getNumOfPixel(IplImage* source, int lineHight, int threshold, int* index) 
{
	int width = source->width;
	int numOfPixel = 0;
	CvScalar scalar = cvGet2D(source, lineHight, *index);
	//int temp = 0.114 * scalar.val[0] + 0.587 * scalar.val[1] + 0.299 * scalar.val[2];//BGR转化为灰度图像公式
	int temp = (scalar.val[0] + scalar.val[1] + scalar.val[2]) / 3;//BGR转化为灰度图像公式
	int bias = temp - threshold;
	do {
		if (*index >= width - 5)
			break;
		numOfPixel++;
		(*index)++;
		scalar = cvGet2D(source, lineHight, *index);
		/*temp = 0.114 * scalar.val[0] + 0.587 * scalar.val[1] + 0.299 * scalar.val[2];*/
		temp = (scalar.val[0] + scalar.val[1] + scalar.val[2]) / 3;//BGR转化为灰度图像公式
	} while (bias * (temp - threshold) > 0); //同负或同正相乘均为正，此处为优化算法，之前版本为将大于threshold或小于threshold作为两个分支，发现代码重复过大。
	return numOfPixel;
}

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
int getBaseAndStart(IplImage* source, int lineHight, int threshold, int* base, int* start) 
{
	int height = source->height;
	int width = source->width;
	int pixel_add[20] = {0};
	vector<vector<int>> array_int;
	vector<int> temp_int;
	int flag=0;//标记位
	//cout << "width:" << width << endl;
	for (int i = 0; i < (width/3); i++)
	{
		CvScalar scalar = cvGet2D(source, lineHight, i);
		//int temp = 0.114 * scalar.val[0] + 0.587 * scalar.val[1] + 0.299 * scalar.val[2];//BGR转化为灰度图像公式
		int temp = (scalar.val[0] + scalar.val[1] + scalar.val[2]) / 3;//BGR转化为灰度图像公式
		//threshold = 100;
		if (temp <threshold)
		{
			if (flag == 3)//突变清空数组
			{
				array_int.push_back(temp_int);
				vector<int>().swap(temp_int);
			}
			temp_int.push_back(2);//代表黑色,设置为2
			flag = 2;
		}
		else
		{
			if (flag == 2)//突变清空数组
			{
				array_int.push_back(temp_int);
				vector<int>().swap(temp_int);
			}
			temp_int.push_back(3);//代表白色,设置为3
			flag = 3;
		}
	}
	if (array_int.size() < 2)//为空
	{
		return -1;//失败
	}
	else
	{
		for (int j = 0; j < array_int.size()-2; j++)
		{
			if (array_int[j][0] == 3) continue;//白色
			//cout << "array_int.size："<< array_int[j].size() <<" "<< array_int[j + 1].size() <<" "<< array_int[j + 2].size() << endl;
			//if (array_int[j].size()== array_int[j + 1].size() && array_int[j + 1].size() == array_int[j + 2].size())//即101
			if (abs(array_int[j].size()- array_int[j + 1].size())<=1 &&(array_int[j + 1].size()-array_int[j + 2].size())<=1)//即101
			{
				int sum = 0;
				for (int k = 0; k < j; k++)
					sum += array_int[k].size();//求和，代表起始位置
				*start = sum;//起始位置
				*base = (array_int[j].size()+array_int[j + 1].size())/2;
				
				//cout << "参数：" << *start <<" "<< *base << endl;
				
				return 0;//成功
			}
		}
	}
	return -2;//失败
}
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
int get_bar(Mat &gray, vector<Point2f>  &vertices, Mat &final_result, const int &add_pixel)
{
	RotatedRect minRect;//最小矩形
	if (vertices.size() <4)
	{
		//cout << "NO BAR" << endl;
		return -1;
	}
	else
		if (vertices.size() == 4)
		{
			Rect rect_expand = Rect(add_pixel / 2, add_pixel / 2, gray.cols, gray.rows);//大图中裁剪的矩形
			Mat temp_expand = Mat::zeros(gray.rows + add_pixel, gray.cols + add_pixel, CV_8UC1);//灰度图，(行，列，参数)格式：宽*长
			gray.copyTo(temp_expand(rect_expand));	//图像加和
													//cout << "vertices.size:" << vertices.size() << endl;
			for (int i = 0; i < vertices.size(); i++)//求出最小矩形
			{
				vertices[i].x += add_pixel / 2;
				vertices[i].y += add_pixel / 2;
				//temp_contour.push_back(vertices[0][i]);
				//cout << "vertices:" << vertices[i] <<" "<< vertices[i] << endl;
			}
			minRect = minAreaRect(vertices);
			float rect_height = minRect.size.height;
			float rect_width = minRect.size.width;
			Point2f center_point = minRect.center;//外接矩形中心点坐标
			float temp_angle = minRect.angle;
			int x_length = 0;
			int y_length = 0;
			Mat rot_mat;//求旋转矩阵
			Mat rot_image;
			Size dst_sz(temp_expand.size());
			///*cout << "(" << center_point.x << ", " << center_point.y << ") " <<" ";
			//cout << rect_width << " " << rect_height << " ";
			//cout << " minRect.angle= " << temp_angle << endl;*/
			if (rect_width < rect_height)
			{
				temp_angle = minRect.angle + 90;
				x_length = rect_height;
				y_length = rect_width;
			}
			else
			{
				temp_angle = minRect.angle;
				x_length = rect_width;
				y_length = rect_height;
			}
			rot_mat = getRotationMatrix2D(center_point, temp_angle, 1.0);//求旋转矩形
			warpAffine(temp_expand, rot_image, rot_mat, dst_sz);//原图像旋转
			final_result = rot_image(Rect(center_point.x - (rect_width / 2) - 50, center_point.y - (rect_height / 2) - 50, rect_width + 100, rect_height + 100));//提取ROI
		   // imwrite("final.jpg",final_result);
		}
	return vertices.size();
}

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
string parseEAN13(string code) 
{
	unordered_map<string, string> numberMap;
	numberMap["0001101"] = "0A";
	numberMap["0100111"] = "0B";
	numberMap["1110010"] = "0C";
	numberMap["0011001"] = "1A";
	numberMap["0110011"] = "1B";
	numberMap["1100110"] = "1C";
	numberMap["0010011"] = "2A";
	numberMap["0011011"] = "2B";
	numberMap["1101100"] = "2C";
	numberMap["0111101"] = "3A";
	numberMap["0100001"] = "3B";
	numberMap["1000010"] = "3C";
	numberMap["0100011"] = "4A";
	numberMap["0011101"] = "4B";
	numberMap["1011100"] = "4C";
	numberMap["0110001"] = "5A";
	numberMap["0111001"] = "5B";
	numberMap["1001110"] = "5C";
	numberMap["0101111"] = "6A";
	numberMap["0000101"] = "6B";
	numberMap["1010000"] = "6C";
	numberMap["0111011"] = "7A";
	numberMap["0010001"] = "7B";
	numberMap["1000100"] = "7C";
	numberMap["0110111"] = "8A";
	numberMap["0001001"] = "8B";
	numberMap["1001000"] = "8C";
	numberMap["0001011"] = "9A";
	numberMap["0010111"] = "9B";
	numberMap["1110100"] = "9C";

	unordered_map<string, string> oddEvenMap;
	oddEvenMap["AAAAAA"] = "0";
	oddEvenMap["AABABB"] = "1";
	oddEvenMap["AABBAB"] = "2";
	oddEvenMap["AABBBA"] = "3";
	oddEvenMap["ABAABB"] = "4";
	oddEvenMap["ABBAAB"] = "5";
	oddEvenMap["ABBBAA"] = "6";
	oddEvenMap["ABABAB"] = "7";
	oddEvenMap["ABABBA"] = "8";
	oddEvenMap["ABBABA"] = "9";

	string result;
	if (code.length() < 95)
	{
		return "ERROR1";
	}
	//cout << "step1" << endl;
	string codeLeft[6] = {};
	string codeRight[6] = {};
	string oddEven = "";
	for (int i = 0; i < 6; i++) {
		codeLeft[i] = code.substr(3 + i * 7, 7);
		string codeTemp = numberMap[codeLeft[i]];
		if (codeTemp == "")
			return "ERROR2";
		result = result + codeTemp[0];
		if (codeTemp[1] == 'A') {
			oddEven += "A";
		}
		else {
			oddEven += "B";
		}
	}

	string before = oddEvenMap[oddEven];
	result = before + result;

	for (int i = 0; i < 6; i++) {
		codeRight[i] = code.substr(50 + i * 7, 7);
		string codeTemp = numberMap[codeRight[i]];
		if (codeTemp == "")
			return "ERROR3";
		result = result + codeTemp[0];
	}
	//验证
	int s1 = (result[0] - '0') + (result[2] - '0') + (result[4] - '0') + (result[6] - '0') + (result[8] - '0') + (result[10] - '0');
	int s2 = (result[1] - '0') + (result[3] - '0') + (result[5] - '0') + (result[7] - '0') + (result[9] - '0') + (result[11] - '0');
	//cout << (result[0] - '0') << (result[1] - '0') << (result[2] - '0') << (result[3] - '0') << (result[4] - '0') << (result[5] - '0');
	//cout << (result[6] - '0') << (result[7] - '0') << (result[8] - '0') << (result[9] - '0') << (result[10] - '0') << (result[11] - '0')<<endl;
	int s3 = 10 - ((s1 + s2 * 3) % 10);
	if (s3 == 10) s3 = 0;
	//cout << s3 << endl;
	if (s3 != result[12] - '0')
		return "ERROR4";
	else
		//cout << "Confirmative" << endl;
		cout << " ";

	return result;
}