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
* ���ƣ�                  LoadLabelFile
* ���ܣ�                   ����ͼ��ģ��
* ��ڲ�����  std::vector<std::string> &result:     const char *fname:
* ���ڲ�����        ��
*******************************************************************/
void LoadLabelFile(std::vector<std::string> &result, const char *fname)//����ͼƬ���
{
    std::ifstream labels(fname);
    std::string line;
    while (std::getline(labels, line))
        result.push_back(line);
}

/*******************************************************************
* ���ƣ�                  PrintTopLabels
* ���ܣ�                   ��ӡ��Ǻ�����ǧ���ࣩ
* ��ڲ�����        const char *label_file���ļ�����     float *data�����������
* ���ڲ�����        ��
*******************************************************************/
void PrintTopLabels(const char *label_file, float *data)//��ӡͼƬ���
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
// setiosflags �ǰ����������ռ�iomanip �е�C++ ���������ò�������������ִ�����в���ָ�������ڵĶ�����
// iso::fixed �ǲ�����setiosflags �Ĳ���֮һ���ò���ָ���Ķ������Դ�С�������ʽ��ʾ������������������ľ��ȷ�Χ�ھ����ܵİ���������С�����Ҳࣻ
        if(result[idx]>0.2)
        std::cout << std::fixed << std::setprecision(4)
                  << result[idx] << " - \"" << labels[idx] << "\"\n";
    }
}
/*******************************************************************
* ���ƣ�                  get_input_data
* ���ܣ�                   ��ȡ��������
* ��ڲ�����const char* image_file�������ļ� float *input_data:���
            int img_h:ͼƬ�߶�               int img_w:ͼƬ���    
			const float* mean:RGBȡֵ��ֵ    float scale:��������
* ���ڲ�����        ��
*******************************************************************/
void get_input_data(cv::Mat img, float *input_data, int img_h, int img_w,const float* mean, float scale)//��ȡ�������� float scale������
{
#if 1
   // cv::Mat img = cv::imread(image_file, -1);   
   if (img.empty())
   {
         std::cerr << "failed to read image file " << "\n";
         return;
   }
   cv::resize(img, img, cv::Size(img_h, img_w));
   img.convertTo(img, CV_32FC3); // ������UINT8��Ϊ��FLOAT32λ
   float *img_data = (float *)img.data;//ͼ������
   int hw = img_h * img_w;
   for (int h = 0; h < img_h; h++)
       for (int w = 0; w < img_w; w++)
          for (int c = 0; c < 3; c++)
          {
              input_data[c * hw + h * img_w + w] = (*img_data - mean[c])*scale;//��ӦRGB��ֵ
              img_data++;
          }
#endif
}

   
int main(int argc, char * argv[])//./build/tests/bin/bench_sqz -r1
{  
   int img_h=227;//��һ��ͼ���С227*227
   int img_w=227;
   std::string model_name = "squeeze_net";//����
/*******************************************************************
* ���ƣ�                  init_tengine_library
* ���ܣ�                  ��ʼ��tengine�⺯��
* ��ڲ�����
* ���ڲ�����        0���ɹ�    -1��ʧ��
*******************************************************************/
   int init_tengine=init_tengine_library();//�Գ�ʼ��
   if(init_tengine==-1)
   {
	  std::cout<<"init tengine failed\n";
	  return -1;
   }
/*******************************************************************
* ���ƣ�                  request_tengine_version
* ���ܣ�                  �汾����
* ��ڲ�����
* ���ڲ�����        0����֧��    1��֧��
*******************************************************************/
   if(request_tengine_version("0.1")<0)
      return -2;
/*******************************************************************
* ���ƣ�                  load_model
* ���ܣ�                  ģ�ͼ���
* ��ڲ�����const char* model_name:ָ��ģ������
            const char* model_format:��caffe������tensorflow������onnx������tengine��
			const char* text_file��ģ�����ƣ�
			const char* model_file��ģ���ļ���
* ���ڲ�����        0���ɹ�    -1��ʧ��
*******************************************************************/
   if(load_model(model_name.c_str(),"caffe",text_file,model_file)<0)
      return 1; 
   std::cout<<"Load model successfully\n";
/*******************************************************************
* ���ƣ�                  create_runtime_graph
* ���ܣ�                  ��������ͼ
* ��ڲ�����const char* graph_name:ָ��ͼ����
            const char* model_name:ģ������
			workspace_t ws�����߿ռ���������NULL��ʹ��Ĭ�Ͽռ�
* ���ڲ�����        graph_t:����ͼ�����ʧ�ܷ���NULL
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
   tensor_t input_tensor;//����
   tensor_t  output_tensor;//���
   float  * input_data;//�������ݹ��
   cv::VideoCapture capture(0);//������ͷ
   cv::Mat srcImage;
   input_data=(float*) malloc (sizeof(float) * img_h *img_w *3);
/*******************************************************************
* ���ƣ�              get_graph_input_tensor
* ���ܣ�              ��ȡͼ������ڵ���������
* ��ڲ�����graph_t graph������ͼ���
            int node_idx������ڵ�����ֵ
			int tensor_idx:��������ֵ
* ���ڲ�����        tensor_t:���������ʧ�ܷ���NULL
*******************************************************************/
   input_tensor=get_graph_input_tensor(graph , node_idx , tensor_idx );   
   if(!check_tensor_valid(input_tensor))
   {
       std::printf("Cannot find input tensor,node_idx: %d,tensor_idx: %d\n",node_idx,tensor_idx);
       return -1;
   }
   int dims[]={1,3,img_h,img_w};   //����ά������
/*******************************************************************
* ���ƣ�              set_tensor_shape
* ���ܣ�              ����������״
* ��ڲ�����tensor_t input_tensor���������
            int dims��ά�ȵ�����
			int dims_num: ά�ȸ���  4ά
* ���ڲ�����       ��
*******************************************************************/
   set_tensor_shape(input_tensor,dims,4);
/*******************************************************************
* ���ƣ�              get_tensor_shape
* ���ܣ�              ��ȡ������״
* ��ڲ�����tensor_t input_tensor���������
            int dims��ά�ȵ�����
			int dims_num: ά�ȸ���  4ά
* ���ڲ�����       ��
*******************************************************************/
    int a=get_tensor_shape(input_tensor,dims,4);
	if(a!=-1)
	   std::cout << "ά����Ŀ��" <<a<< std::endl;

   while(1)
   {

    capture >> srcImage; //��ȡһ֡ͼ��
	 //Mat srcImage=imread("a.jpg");
	if (!srcImage.data)  //�������Ϣ����
	{
		std::cout << "Could not open or find the image" << std::endl;
		continue;
		//getchar();
		// return -1;
	}	
   /* prepare input data */
   get_input_data( srcImage, input_data, img_h, img_w, channel_mean,1);//��ȡ����
   
   /* setup input buffer */
/*******************************************************************
* ���ƣ�             set_tensor_buffer
* ���ܣ�             �����������ݻ�����
* ��ڲ�����tensor_t input_tensor���������
            void *buffer�����ݻ�����ָ�루��Ҫ�ͷţ�
            int size����������С��4ά��
* ���ڲ�����       0���ɹ�    -1��ʧ��
*******************************************************************/
   if(set_tensor_buffer(input_tensor,input_data,3*img_h*img_w*4)<0)
   {
       std::printf("Set buffer for tensor failed\n");
	   return -1;
   }

   /* run the graph */
/*******************************************************************
* ���ƣ�                  prerun_graph
* ���ܣ�                ͼԤ���У�׼������
* ��ڲ�����      graph_t graph:����ͼ���
* ���ڲ�����        0���ɹ�    -1��ʧ��
*******************************************************************/
   prerun_graph(graph); 
 /*******************************************************************
* ���ƣ�                  run_graph
* ���ܣ�                   ����ͼ
* ��ڲ�����      graph_t graph:����ͼ���
                  int block��0������wait_graph�ȴ����н�����   1���������У�
* ���ڲ�����        0���ɹ�    -1��ʧ��
*******************************************************************/ 
   run_graph(graph,1);

/*******************************************************************
* ���ƣ�              get_graph_output_tensor
* ���ܣ�              ��ȡͼ������ڵ���������
* ��ڲ�����graph_t graph������ͼ���
            int node_idx������ڵ�����ֵ
			int tensor_idx: ��������ֵ
* ���ڲ�����        tensor_t:���������ʧ�ܷ���NULL
*******************************************************************/
   output_tensor = get_graph_output_tensor(graph, 0, 0);
   
/*******************************************************************
* ���ƣ�              get_tensor_buffer
* ���ܣ�              ��ȡ�������ݻ�����
* ��ڲ�����   tensor_t:ͼ���������
* ���ڲ�����   void *�����ݻ�����ָ�루NULL��ʾ���ݻ�����δ���룩
*******************************************************************/
   float *data = (float *)get_tensor_buffer(output_tensor);

   PrintTopLabels(label_file, data);
   
    }   
   put_graph_tensor(output_tensor);//�ͷ�����������
   put_graph_tensor(input_tensor);//�ͷ������������
   postrun_graph(graph); //�ͷ�����ͼ��Դ
   destroy_runtime_graph(graph);//��������ͼ
   remove_model(model_name.c_str());//�Ƴ�ģ���ļ�
   std::cout<<"ALL TEST DONE\n";
   release_tengine_library();   //��Դ�ͷŲ���
   free(input_data);
   input_data=NULL;//free�ͷŶѿռ�󣬱������Чָ���Ϊ��
   return 0;
}
