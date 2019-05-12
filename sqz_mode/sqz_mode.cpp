/*****************************************************************************
*       Thousand classification                                              *
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
*  @file              sqz_mode.h                                             *
*  @brief               千分类                                               *
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
#include <time.h>
#include <sys/time.h>
#include "sqz_mode.h"

using namespace std;
using namespace cv;
using namespace TEngine;

const char * sqz_text_file="./sqz_net/sqz.prototxt";   //里面定义了模型的网络结构
const char * sqz_model_file="./sqz_net/squeezenet_v1.1.caffemodel"; //网络模型
const char * sqz_image_file="./sqz_net/cat.jpg";//原始图片
const char * sqz_label_file="./sqz_net/synset_words.txt"; //ImageNet标签文件,存放分类名称
const float sqz_channel_mean[3]={104.007, 116.669, 122.679};

/*******************************************************************
* 名称：                  LoadLabelFile_sqz
* 功能：                   导入图像模型
* 入口参数：   vector< string> &result:     const char *fname:
* 出口参数：        空
*******************************************************************/
void LoadLabelFile_sqz( vector< string> &result, const char *fname)//导入图片标记
{
     ifstream labels(fname);
     string line;
    while ( getline(labels, line))
        result.push_back(line);
}

/*******************************************************************
* 名称：                  PrintTopLabels_sqz
* 功能：                   打印标记函数（千分类）
* 入口参数：        const char *sqz_label_file：文件参数     float *data：输入的数据 
                    string &name_labels:名称数组             int &rate_num：比率，保留三位小数
* 出口参数：        空
*******************************************************************/
void PrintTopLabels_sqz(const char *sqz_label_file, float *data,string &name_labels,int &rate_num)//打印图片标记
{
    // load labels
    vector< string> labels;
    LoadLabelFile_sqz(labels, sqz_label_file);

    float *end = data + 1000;
     vector<float> result(data, end);
     vector<int> top_N = Argmax(result, 5);

	//int fd = UART_Init();
	//char name_temp[128];
	int len_buff=labels.size();
	if(len_buff>0)
		for (unsigned int i = 0; i < top_N.size(); i++)
		{
			int idx = top_N[i];
	// setiosflags 是包含在命名空间iomanip 中的C++ 操作符，该操作符的作用是执行由有参数指定区域内的动作；
	// iso::fixed 是操作符setiosflags 的参数之一，该参数指定的动作是以带小数点的形式表示浮点数，并且在允许的精度范围内尽可能的把数字移向小数点右侧；
			if(result[idx]>0.3)
			{
			 cout <<  fixed <<  setprecision(4) << result[idx] << " - \"" << labels[idx] << "\"\n";
			 name_labels = labels[idx].c_str();
			 rate_num = (int)(result[idx]*1000);
			 
			 // strcpy(name_temp,labels.c_str());
			 // color_temp[len_buff]='\n';
			 // len=UART_Send(fd,(unsigned char*)color_temp,len_buff+1);//发送数
			}	
		}
}
/*******************************************************************
* 名称：                  get_input_data_sqz
* 功能：                   获取输入数据
* 入口参数：const char* sqz_image_file：输入文件 float *input_data:输出
            int img_h:图片高度               int img_w:图片宽度    
			const float* mean:RGB取值阈值    float scale:缩放因子
* 出口参数：        空
*******************************************************************/
// void get_input_data_sqz(const char* sqz_image_file, float *input_data, int img_h, int img_w,const float* mean, float scale)//获取输入数据 float scale：缩放
void get_input_data_sqz( Mat img, float *input_data, int img_h, int img_w,const float* mean, float scale)//获取输入数据 float scale：缩放
{
   //  Mat img =  imread(sqz_image_file, -1);   
   if (img.empty())
   {
          cerr << "failed to read image file " << "\n";
         return;
   }
    resize(img, img,  Size(img_h, img_w));
   img.convertTo(img, CV_32FC3); // 类型又UINT8变为了FLOAT32位
   float *img_data = (float *)img.data;//图像数据
   int hw = img_h * img_w;
   for (int h = 0; h < img_h; h++)
       for (int w = 0; w < img_w; w++)
          for (int c = 0; c < 3; c++)
          {
              input_data[c * hw + h * img_w + w] = (*img_data - mean[c])*scale;//对应RGB阈值
              img_data++;
          }
}
