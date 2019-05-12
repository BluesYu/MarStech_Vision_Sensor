/*
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * License); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * AS IS BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
 */
/*
 * Copyright (c) 2017, Open AI Lab
 * Author: haitao@openailab.com
 */
#include <unistd.h>
#include <iostream> 
#include <functional>
#include <algorithm>
#include <fstream>
#include <iomanip>
#include <time.h>
#include "opencv2/opencv.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "tengine_c_api.h"
#include <sys/time.h>
#include "common_util.hpp"

using namespace std;
using namespace cv;
using namespace TEngine;
/*******************************************************************
* 名称：                  LoadLabelFile
* 功能：                   导入图像模型
* 入口参数：  std::vector<std::string> &result:     const char *fname:
* 出口参数：        空
*******************************************************************/
void LoadLabelFile(std::vector<std::string> &result, const char *fname)//导入图片标记
{
    std::ifstream labels(fname);
    std::string line;
    while (std::getline(labels, line))
        result.push_back(line);
}

/*******************************************************************
* 名称：                  PrintTopLabels
* 功能：                   打印标记函数（千分类）
* 入口参数：        const char *label_file：文件参数     float *data：输入的数据
* 出口参数：        空
*******************************************************************/
void PrintTopLabels(const char *label_file, float *data)//打印图片标记
{
    // load labels
    std::vector<std::string> labels;
    LoadLabelFile(labels, label_file);

    float *end = data + 1000;
    std::vector<float> result(data, end);
    std::vector<int> top_N = Argmax(result, 5);

    for (unsigned int i = 0; i < top_N.size(); i++)
    {
        int idx = top_N[i];
// setiosflags 是包含在命名空间iomanip 中的C++ 操作符，该操作符的作用是执行由有参数指定区域内的动作；
// iso::fixed 是操作符setiosflags 的参数之一，该参数指定的动作是以带小数点的形式表示浮点数，并且在允许的精度范围内尽可能的把数字移向小数点右侧；
        if(result[idx]>0.2)
        std::cout << std::fixed << std::setprecision(4)
                  << result[idx] << " - \"" << labels[idx] << "\"\n";
    }
}
/*******************************************************************
* 名称：                  get_input_data
* 功能：                   获取输入数据
* 入口参数：const char* image_file：输入文件 float *input_data:输出
            int img_h:图片高度               int img_w:图片宽度    
			const float* mean:RGB取值阈值    float scale:缩放因子
* 出口参数：        空
*******************************************************************/
void get_input_data(cv::Mat img, float *input_data, int img_h, int img_w,const float* mean, float scale)//获取输入数据 float scale：缩放
{
#if 1
   // cv::Mat img = cv::imread(image_file, -1);   
   if (img.empty())
   {
         std::cerr << "failed to read image file " << "\n";
         return;
   }
   cv::resize(img, img, cv::Size(img_h, img_w));
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
#endif
}

   
int main(int argc, char * argv[])//./build/tests/bin/bench_sqz -r1
{  
   int img_h=227;//归一化图像大小227*227
   int img_w=227;
   std::string model_name = "squeeze_net";//名称
/*******************************************************************
* 名称：                  init_tengine_library
* 功能：                  初始化tengine库函数
* 入口参数：
* 出口参数：        0：成功    -1：失败
*******************************************************************/
   int init_tengine=init_tengine_library();//对初始化
   if(init_tengine==-1)
   {
	  std::cout<<"init tengine failed\n";
	  return -1;
   }
/*******************************************************************
* 名称：                  request_tengine_version
* 功能：                  版本设置
* 入口参数：
* 出口参数：        0：不支持    1：支持
*******************************************************************/
   if(request_tengine_version("0.1")<0)
      return -2;
/*******************************************************************
* 名称：                  load_model
* 功能：                  模型加载
* 入口参数：const char* model_name:指定模型名称
            const char* model_format:“caffe”，“tensorflow”，“onnx”，“tengine”
			const char* text_file：模型名称；
			const char* model_file：模型文件；
* 出口参数：        0：成功    -1：失败
*******************************************************************/
   if(load_model(model_name.c_str(),"caffe",text_file,model_file)<0)
      return 1; 
   std::cout<<"Load model successfully\n";
/*******************************************************************
* 名称：                  create_runtime_graph
* 功能：                  创建运行图
* 入口参数：const char* graph_name:指定图名称
            const char* model_name:模型名称
			workspace_t ws：工具空间句柄，传入NULL，使用默认空间
* 出口参数：        graph_t:运行图句柄，失败返回NULL
*******************************************************************/
   graph_t graph=create_runtime_graph("graph0",model_name.c_str(),NULL);
   if(!check_graph_valid(graph))
   {
       std::cout<<"Create graph0 failed\n";
       return 1;
   }

   /* get input tensor */
   int node_idx = 0;
   int tensor_idx = 0;
   tensor_t input_tensor;//输入
   tensor_t  output_tensor;//输出
   float  * input_data;//输入数据规格
   cv::VideoCapture capture(0);//打开摄像头
   cv::Mat srcImage;
   input_data=(float*) malloc (sizeof(float) * img_h *img_w *3);
/*******************************************************************
* 名称：              get_graph_input_tensor
* 功能：              获取图的输入节点的张量句柄
* 入口参数：graph_t graph：运行图句柄
            int node_idx：输入节点索引值
			int tensor_idx:张量索引值
* 出口参数：        tensor_t:张量句柄，失败返回NULL
*******************************************************************/
   input_tensor=get_graph_input_tensor(graph , node_idx , tensor_idx );   
   if(!check_tensor_valid(input_tensor))
   {
       std::printf("Cannot find input tensor,node_idx: %d,tensor_idx: %d\n",node_idx,tensor_idx);
       return -1;
   }
   int dims[]={1,3,img_h,img_w};   //设置维度数组
/*******************************************************************
* 名称：              set_tensor_shape
* 功能：              设置张量形状
* 入口参数：tensor_t input_tensor：张量句柄
            int dims：维度的数组
			int dims_num: 维度个数  4维
* 出口参数：       无
*******************************************************************/
   set_tensor_shape(input_tensor,dims,4);
/*******************************************************************
* 名称：              get_tensor_shape
* 功能：              获取张量形状
* 入口参数：tensor_t input_tensor：张量句柄
            int dims：维度的数组
			int dims_num: 维度个数  4维
* 出口参数：       无
*******************************************************************/
    int a=get_tensor_shape(input_tensor,dims,4);
	if(a!=-1)
	   std::cout << "维度数目：" <<a<< std::endl;

   while(1)
   {

    capture >> srcImage; //获取一帧图像
	 //Mat srcImage=imread("a.jpg");
	if (!srcImage.data)  //错误的信息处理
	{
		std::cout << "Could not open or find the image" << std::endl;
		continue;
		//getchar();
		// return -1;
	}	
   /* prepare input data */
   get_input_data( srcImage, input_data, img_h, img_w, channel_mean,1);//获取输入
   
   /* setup input buffer */
/*******************************************************************
* 名称：             set_tensor_buffer
* 功能：             设置张量数据缓冲区
* 入口参数：tensor_t input_tensor：张量句柄
            void *buffer：数据缓冲区指针（需要释放）
            int size：缓冲区大小（4维）
* 出口参数：       0：成功    -1：失败
*******************************************************************/
   if(set_tensor_buffer(input_tensor,input_data,3*img_h*img_w*4)<0)
   {
       std::printf("Set buffer for tensor failed\n");
	   return -1;
   }

   /* run the graph */
/*******************************************************************
* 名称：                  prerun_graph
* 功能：                图预运行，准备参数
* 入口参数：      graph_t graph:运行图句柄
* 出口参数：        0：成功    -1：失败
*******************************************************************/
   prerun_graph(graph); 
 /*******************************************************************
* 名称：                  run_graph
* 功能：                   运行图
* 入口参数：      graph_t graph:运行图句柄
                  int block（0：调用wait_graph等待运行结束；   1：阻塞运行）
* 出口参数：        0：成功    -1：失败
*******************************************************************/ 
   run_graph(graph,1);

/*******************************************************************
* 名称：              get_graph_output_tensor
* 功能：              获取图的输出节点的张量句柄
* 入口参数：graph_t graph：运行图句柄
            int node_idx：输出节点索引值
			int tensor_idx: 张量索引值
* 出口参数：        tensor_t:张量句柄，失败返回NULL
*******************************************************************/
   output_tensor = get_graph_output_tensor(graph, 0, 0);
   
/*******************************************************************
* 名称：              get_tensor_buffer
* 功能：              获取张量数据缓冲区
* 入口参数：   tensor_t:图像张量句柄
* 出口参数：   void *：数据缓存区指针（NULL表示数据缓冲区未申请）
*******************************************************************/
   float *data = (float *)get_tensor_buffer(output_tensor);

   PrintTopLabels(label_file, data);
   
    }   
   put_graph_tensor(output_tensor);//释放输出张量句柄
   put_graph_tensor(input_tensor);//释放输入张量句柄
   postrun_graph(graph); //释放运行图资源
   destroy_runtime_graph(graph);//销毁运行图
   remove_model(model_name.c_str());//移除模型文件
   std::cout<<"ALL TEST DONE\n";
   release_tengine_library();   //资源释放操作
   free(input_data);
   input_data=NULL;//free释放堆空间后，必须把无效指针变为空
   return 0;
}
