/*****************************************************************************
*      Multiple Number Recognition                                           *
*   Copyright (c) 2017, Open AI Lab  Author: haitao@openailab.com            *
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
*  @file        lenet_num_mode.h                                             *
*  @brief         两行数字识别                                               *
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
*                  读取摄像头，运行caffe模型                                 *
*----------------------------------------------------------------------------*
*  Change History :                                                          *
*  <2019.5.5>     | 1.0.0.1> | <Blues.Yu>  | <Initializer>                   *
*                                                                            *
*----------------------------------------------------------------------------*
*  2019/05/05 | 1.0.0.1   | Blues.Yu      | Create file  opencv处理函数()    *
*----------------------------------------------------------------------------*
*                                                                            *
*****************************************************************************/
#include <unistd.h>
#include <iostream> 
#include <functional>
#include <algorithm>
#include <fstream>
#include <iomanip>
#include <stdio.h>
#include <sys/time.h>
#include "lenet_num_mode.h"  

using namespace TEngine;
using namespace cv;
using namespace std;

const char* lenet_text_file="./lenet/lenet.prototxt";   //里面定义了模型的网络结构
const char* lenet_model_file="./lenet/lenet_iter_20000.caffemodel"; //网络模型
//const char * image_file="./lenet/num2/num5.jpg";   //原始图片
const char* lenet_label_file="./lenet/synset_words.txt"; //ImageNet标签文件,存放分类名称

/*******************************************************************
* 名称：                  LoadLabelFile
* 功能：                   导入图像模型
* 入口参数：  vector<string> &result:     const char *fname:
* 出口参数：        空
*******************************************************************/
void LoadLabelFile(vector<string> &result, const char *fname)//导入图片标记
{
    ifstream labels(fname);
    string line;
    while (getline(labels, line))
        result.push_back(line);
}

/*******************************************************************
* 名称：                  PrintTopLabels
* 功能：                   打印标记函数（千分类）
* 入口参数：        const char *lenet_label_file：文件参数     float *data：输入的数据
* 出口参数：            返回数据（0-9），错误返回-1
*******************************************************************/
int PrintTopLabels(const char *lenet_label_file, float *data)//打印图片标记
{
    // load labels
    vector<string> labels;
    LoadLabelFile(labels, lenet_label_file);

    float *end = data + 10;
    vector<float> result(data, end);
    vector<int> top_N = Argmax(result,2);//最大的三个数值

    for (unsigned int i = 0; i < top_N.size(); i++)
    {
        unsigned int idx = top_N[i];//参数序号
// setiosflags 是包含在命名空间iomanip 中的C++ 操作符，该操作符的作用是执行由有参数指定区域内的动作；
// iso::fixed 是操作符setiosflags 的参数之一，该参数指定的动作是以带小数点的形式表示浮点数，并且在允许的精度范围内尽可能的把数字移向小数点右侧；
        if(result[idx]>0.5)
		  {
			// cout<<idx<<" ";
			 return idx;
			// cout<<labels[idx]<<endl;
			// cout << "("<<fixed << setprecision(3)<< result[idx] << ") "<<labels[idx] << " ";
		  }   
    } 
  return -1;
}

/*******************************************************************
* 名称：                  get_input_data
* 功能：                   获取输入数据
* 入口参数：Mat img：灰度图                  float *input_data:输出
            int img_h:图片高度               int img_w:图片宽度    
			const float* mean:RGB取值阈值    float scale:缩放因子
* 出口参数：        空
*******************************************************************/
void get_input_data(Mat img, float *input_data, int img_h, int img_w)//获取输入数据 float scale：缩放
{
   if (img.empty())
   {
         cerr << "failed to read image file " << "\n";
         return;
   }
	img.convertTo(img, CV_32FC3); // 类型又UINT8变为了FLOAT32位
	float *img_data = (float *)img.data;//图像数据
    for (int h = 0; h < img_h; h++)//y
       for (int w = 0; w < img_w; w++)//x
	      {
            input_data[h * img_w + w] = (*img_data)*0.00390625;//对应RGB阈值
            img_data++;
	       }
}
/*******************************************************************
* 名称：                get_num_Rect
* 功能：                获取图像矩阵
* 入口参数：graph_t graph：运行图句柄
            int node_idx：输出节点索引值
			int tensor_idx: 张量索引值
* 出口参数：        tensor_t:张量句柄，失败返回NULL
*******************************************************************/
int get_num_Rect(Mat &dis_binary, vector<Rect> &Rect_temp)//找到的矩阵
{
	vector<vector<Point>> contours;//有多少轮廓，向量contours就有多少元素
	vector<Vec4i> hierarchy;//每一个元素的4个int型变量——hierarchy[i][0] ~hierarchy[i][3]，分别表示第i个轮廓的后一个轮廓、前一个轮廓、父轮廓、内嵌轮廓的索引编号
							//CV_RETR_EXTERNAL  只检测最外围轮廓
							//CV_CHAIN_APPROX_NONE 保存物体边界上所有连续的轮廓点到contours向量内   CV_CHAIN_APPROX_SIMPLE  只有拐点信息
	findContours(dis_binary, contours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE, Point());//查找轮廓
	vector<vector<Point>>::const_iterator itc = contours.begin();//起始轮廓
																 // while(itc!=contours.end())
	for (int i = 0; itc != contours.end(); i++)
	{
		Rect ret = boundingRect(Mat(*itc));//计算右上点集的边界矩形
		int rect_height = ret.height;
		int rect_width = ret.width;

		if (itc->size() <100 || itc->size() > 1000) {// 删除当前连通域轮廓 200为最小轮廓长，3000为最大轮廓长
			itc = contours.erase(itc);
		}
		else
			if (((double)rect_height) / ((double)rect_width)>100 ||//图像的比例参数
				((double)rect_height / ((double)rect_width)<0.2))
			{
				itc = contours.erase(itc);
			}
			else
			{//满足要求，保存矩形参数。
				itc++;
				ret.x = (ret.x > 0 ? ret.x : 0);//防止越界
				ret.y = (ret.y > 0 ? ret.y : 0);
				Rect_temp.push_back(ret);//存储矩形信息
			}
	}
	return Rect_temp.size();
}

/*******************************************************************
* 名称：                get_div_Rect
* 功能：                  图像分割
* 入口参数：vector<Rect> &Rect_temp：分割得到的矩阵
* 出口参数：return= Rect_temp.size()：返回参数大小        
*******************************************************************/
int get_div_Rect(vector<Rect> &Rect_temp)//图像分割
{
	int max_y = 0;
	int min_y = 480;
	int mean_y;
	int isSorted = 1;//冒泡排序的标记
	for (unsigned int i = 0; i < Rect_temp.size() - 1; i++) //求切割阈值
	{
		if (max_y < Rect_temp[i].y) max_y = Rect_temp[i].y;
		if (min_y > Rect_temp[i].y) min_y = Rect_temp[i].y;
	}
	mean_y = (max_y + min_y)/2;//切割的阈值，默认切割2排
								 //cout<< "max_y=" << max_y << " " << "min_y=" << min_y << "mean_y=" << mean_y << endl;
	if (max_y - min_y > 30)
	{
		vector<Rect> Rect_div1;//切割的矩阵
		vector<Rect> Rect_div2;//切割的矩阵

		for (unsigned int i = 0; i < Rect_temp.size(); i++) { //	切割两排
			if (Rect_temp[i].y < mean_y)//第一行
				Rect_div1.push_back(Rect_temp[i]);
			else
				Rect_div2.push_back(Rect_temp[i]);
		}

		for (unsigned int i = 0; i<Rect_div1.size() - 1; i++) { //div1排序
			isSorted = 1; //假设剩下的元素已经排序好了
			for (unsigned int j = 0; j<Rect_div1.size() - 1 - i; j++) {
				if (Rect_div1[j].x > Rect_div1[j + 1].x)//交换顺序
				{
					swap(Rect_div1[j + 1], Rect_div1[j]);
					isSorted = 0; //一旦需要交换数组元素，就说明剩下的元素没有排序好
				}
			}
			if (isSorted) break; //如果没有发生交换，说明剩下的元素已经排序好了
		}

		/*for (int i = 0; i < Rect_div1.size(); i++)
		cout << Rect_div1[i].x << " ";
		cout << endl;*/
		for (unsigned int i = 0; i<Rect_div2.size() - 1; i++) { //div2排序
			isSorted = 1; //假设剩下的元素已经排序好了
			for (unsigned int j = 0; j<Rect_div2.size() - 1 - i; j++) {
				if (Rect_div2[j].x > Rect_div2[j + 1].x)//交换顺序
				{
					swap(Rect_div2[j + 1], Rect_div2[j]);
					isSorted = 0; //一旦需要交换数组元素，就说明剩下的元素没有排序好
				}
			}
			if (isSorted) break; //如果没有发生交换，说明剩下的元素已经排序好了
		}

		/*for (int i = 0; i < Rect_div2.size(); i++)
		cout << Rect_div2[i].x << " ";
		cout << endl;*/

		for (unsigned int i = 0; i < Rect_div1.size(); i++)//合并数组
			Rect_temp[i] = Rect_div1[i];
		for (unsigned int i = 0; i < Rect_div2.size(); i++)
			Rect_temp[Rect_div1.size() + i] = Rect_div2[i];
	}
	else
	{
		for (unsigned int i = 0; i<Rect_temp.size() - 1; i++) {//y轴方向
			isSorted = 1; //假设剩下的元素已经排序好了
			for (unsigned int j = 0; j<Rect_temp.size() - 1 - i; j++) {
				if (Rect_temp[j].x > Rect_temp[j + 1].x)//交换顺序
				{
					swap(Rect_temp[j + 1], Rect_temp[j]);
					isSorted = 0; //一旦需要交换数组元素，就说明剩下的元素没有排序好
				}
			}
			if (isSorted) break; //如果没有发生交换，说明剩下的元素已经排序好了
		}
	}
	return Rect_temp.size();
	// for (int i = 0; i < Rect_temp.size(); i++)
	//cout << Rect_temp[i].x << " ";
	// cout << endl;
}

/*******************************************************************
* 名称：                get_get_div_Image
* 功能：            根据矩矩形对图像进行图像分割，反转，得到最终图像
* 入口参数：Mat &grayImage： 原始图像
            vector<Rect> &Rect_temp：输入的参数矩阵数组
			vector<Mat> &Mat_temp：最终图像数组
* 出口参数：return= Mat_temp..size()：返回切割图形的数目        
*******************************************************************/
int get_get_div_Image(Mat &grayImage, vector<Rect> &Rect_temp, vector<Mat> &Mat_temp)//图像分割,反转
{

	for (unsigned int i = 0; i <Rect_temp.size(); i++) {
		Mat rot_image, final_result;
		rot_image = grayImage;
		Rect ret = Rect_temp[i];
		ret.x = (ret.x > 0 ? ret.x : 0);//防止越界
		ret.y = (ret.y > 0 ? ret.y : 0);
		final_result = rot_image(ret);//提取ROI
		Mat_temp.push_back(final_result);
	}
	return Mat_temp.size();
}

/*******************************************************************
* 名称：                get_expand_Image
* 功能：            根据矩矩形对图像进行图像分割，反转，得到最终图像
* 入口参数：vector<Mat> &Mat_temp:输入图像数组
            vector<Mat> &Mat_dst:最终得到图像数组
			unsigned int add_pixel:膨胀大小
* 出口参数：return= Mat_dst.size()：返回切割图形的数目    
*******************************************************************/
int get_expand_Image(vector<Mat> &Mat_temp, vector<Mat> &Mat_dst,unsigned int add_pixel)//图像扩大，膨胀
{
	if (add_pixel < 5) return -1;//参数过小
	if (Mat_temp.size()<1) return -2;//范围为空
	//Mat temp;//空白参量
	Rect roi_rect;//切割矩形
	for (unsigned int i = 0; i < Mat_temp.size(); i++)//图像膨胀
	{
		//cout <<"Mat="<<Mat_temp[i].cols << "  " << Mat_temp[i].rows << endl;
	    Mat temp = Mat::zeros(Mat_temp[i].rows + add_pixel, Mat_temp[i].cols+ add_pixel, CV_8UC1);//灰度图，(行，列，参数)格式：宽*长
		//cout << "temp=" << temp.cols <<"  "<< temp.rows << endl;//(w和h)
        roi_rect = Rect(add_pixel/2, add_pixel/2, Mat_temp[i].cols, Mat_temp[i].rows);
		//cout << "Rect=" << add_pixel / 2 <<"  "<<Mat_temp[i].cols << "  " << Mat_temp[i].rows << endl;//(w和h)
		Mat_temp[i].copyTo(temp(roi_rect));	//图像加和
		Mat_dst.push_back(temp);
	}

	for (unsigned int i = 0; i < Mat_dst.size(); i++)//对目标图像进行膨胀
	{
		Mat element1 = getStructuringElement(MORPH_RECT, Size(3,3));//原图膨胀
		dilate(Mat_dst[i], Mat_dst[i], element1);

		resize(Mat_dst[i], Mat_dst[i], Size(28, 28), CV_INTER_LINEAR);//设置大小  双线性差值
		Mat element = getStructuringElement(MORPH_RECT, Size(2, 2));//膨胀
		dilate(Mat_dst[i], Mat_dst[i], element);

		for(int j=0;j<Mat_dst[i].cols;j++)
			for (int k = 0; k< Mat_dst[i].rows; k++)
			{
				unsigned int pixy_temp=Mat_dst[i].at<uchar>(k,j);
				if (pixy_temp != 0)
					Mat_dst[i].at<uchar>(k,j) = 255;//白色点
			}
		//imwrite("C:\\Users\\yxx\\Desktop\\fsdownload\\num2\\temp3\\" + pic_name[i] + ".bmp", Mat_dst[i]);   //  将gray图像保存为bmp
	}
	return Mat_dst.size();
}