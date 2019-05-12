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

#ifndef __sqz_mode_H
#define __sqz_mode_H 

#include "opencv2/opencv.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "tengine_c_api.h"
#include "common_util.hpp"
#include "common_util.hpp"

using namespace cv;
using namespace TEngine;

extern const char * sqz_text_file;   //里面定义了模型的网络结构
extern const char * sqz_model_file; //网络模型
extern const char * sqz_image_file;//原始图片
extern const char * sqz_label_file; //ImageNet标签文件,存放分类名称
extern const float sqz_channel_mean[3];

/*******************************************************************
* 名称：                  LoadLabelFile
* 功能：                   导入图像模型
* 入口参数：  std::vector<std::string> &result:     const char *fname:
* 出口参数：        空
*******************************************************************/
void LoadLabelFile_sqz(std::vector<std::string> &result, const char *fname);//导入图片标记

/*******************************************************************
* 名称：                  PrintTopLabels_sqz
* 功能：                   打印标记函数（千分类）
* 入口参数：        const char *sqz_label_file：文件参数     float *data：输入的数据 
                    string &name_labels:名称数组             int &rate_num：比率，保留三位小数
* 出口参数：        空
*******************************************************************/
void PrintTopLabels_sqz(const char *sqz_label_file, float *data,string &name_labels,int &rate_num);//打印图片标记

/*******************************************************************
* 名称：                  get_input_data_sqz
* 功能：                   获取输入数据
* 入口参数：const char* sqz_image_file：输入文件 float *input_data:输出
            int img_h:图片高度               int img_w:图片宽度    
			const float* mean:RGB取值阈值    float scale:缩放因子
* 出口参数：        空
*******************************************************************/
void get_input_data_sqz(cv::Mat img, float *input_data, int img_h, int img_w,const float* mean, float scale);

#endif