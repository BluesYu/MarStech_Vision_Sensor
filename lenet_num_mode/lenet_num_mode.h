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

#ifndef __lenet_mode_H
#define __lenet_mode_H 

#include <opencv2/opencv.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "tengine_c_api.h"
#include "common_util.hpp"

using namespace TEngine;
using namespace cv;

extern const char* lenet_text_file;   //里面定义了模型的网络结构
extern const char* lenet_model_file; //网络模型
//const char * image_file="./lenet/num2/num5.jpg";   //原始图片
extern const char* lenet_label_file; //ImageNet标签文件,存放分类名称

/*******************************************************************
* 名称：                  LoadLabelFile
* 功能：                   导入图像模型
* 入口参数：  vector<string> &result:     const char *fname:
* 出口参数：        空
*******************************************************************/
void LoadLabelFile(vector<string> &result, const char *fname);//导入图片标记

/*******************************************************************
* 名称：                  PrintTopLabels
* 功能：                   打印标记函数（千分类）
* 入口参数：        const char *label_file：文件参数     float *data：输入的数据
* 出口参数：            返回数据（0-9），错误返回-1
*******************************************************************/
int PrintTopLabels(const char *lenet_label_file, float *data);//打印图片标记

/*******************************************************************
* 名称：                  get_input_data
* 功能：                   获取输入数据
* 入口参数：Mat img：灰度图                  float *input_data:输出
            int img_h:图片高度               int img_w:图片宽度    
			const float* mean:RGB取值阈值    float scale:缩放因子
* 出口参数：        空
*******************************************************************/
void get_input_data(Mat img, float *input_data, int img_h, int img_w);//获取输入数据 float scale：缩放

/*******************************************************************
* 名称：                get_num_Rect
* 功能：                获取图像矩阵
* 入口参数：graph_t graph：运行图句柄
            int node_idx：输出节点索引值
			int tensor_idx: 张量索引值
* 出口参数：        tensor_t:张量句柄，失败返回NULL
*******************************************************************/
int get_num_Rect(Mat &dis_binary, vector<Rect> &Rect_temp);//找到的矩阵

/*******************************************************************
* 名称：                get_div_Rect
* 功能：                  图像分割
* 入口参数：vector<Rect> &Rect_temp：分割得到的矩阵
* 出口参数：return= Rect_temp.size()：返回参数大小        
*******************************************************************/
int get_div_Rect(vector<Rect> &Rect_temp);//图像分割

/*******************************************************************
* 名称：                get_get_div_Image
* 功能：            根据矩矩形对图像进行图像分割，反转，得到最终图像
* 入口参数：Mat &grayImage： 原始图像
            vector<Rect> &Rect_temp：输入的参数矩阵数组
			vector<Mat> &Mat_temp：最终图像数组
* 出口参数：return= Mat_temp..size()：返回切割图形的数目        
*******************************************************************/
int get_get_div_Image(Mat &grayImage, vector<Rect> &Rect_temp, vector<Mat> &Mat_temp);//图像分割,反转

/*******************************************************************
* 名称：                get_expand_Image
* 功能：            根据矩矩形对图像进行图像分割，反转，得到最终图像
* 入口参数：vector<Mat> &Mat_temp:输入图像数组
            vector<Mat> &Mat_dst:最终得到图像数组
			unsigned int add_pixel:膨胀大小
* 出口参数：return= Mat_dst.size()：返回切割图形的数目    
*******************************************************************/
int get_expand_Image(vector<Mat> &Mat_temp, vector<Mat> &Mat_dst,unsigned int add_pixel);//图像扩大，膨胀

#endif