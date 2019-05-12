#include <unistd.h>
#include <iostream> 
#include <functional>
#include <algorithm>
#include <fstream>
#include <iomanip>
#include <stdio.h>
#include <sys/time.h>
#include "lenet_num_mode.h"  
#include "uart_io/uart_io.h"

using namespace TEngine;
using namespace cv;
using namespace std;

VideoCapture capture(0);//打开摄像头

   
int main(int argc, char * argv[])//./build/tests/bin/bench_sqz -r1
{
   int fd = UART_Init();

   int img_h=28;//归一化图像大小28*28
   int img_w=28;
   string model_name = "lenet";//名称
/*******************************************************************
* 名称：                  init_tengine_library
* 功能：                  初始化tengine库函数
* 入口参数：
* 出口参数：        0：成功    -1：失败
*******************************************************************/
   int init_tengine=init_tengine_library();//对初始化
   if(init_tengine==-1)
   {
	  cout<<"init tengine failed\n";
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
			const char* lenet_text_file：模型名称；
			const char* lenet_model_file：模型文件；
* 出口参数：        0：成功    -1：失败
*******************************************************************/
   if(load_model(model_name.c_str(),"caffe",lenet_text_file,lenet_model_file)<0)
      return 1; 
   cout<<"Load model successfully\n";

   /* get input tensor */
   int node_idx = 0;//输入节点索引值
   int tensor_idx = 0;//张量索引值
   tensor_t input_tensor;//输入
   tensor_t output_tensor;//输出
   float  * input_data;//输入数据规格
   Mat srcImage,grayImage, binaryImage,dis_binary;//输入图像
   input_data=(float*) malloc (sizeof(float) * img_h *img_w *1);//分配资源
   int dims[]={1,1,img_h,img_w};   //设置维度数组
 
   Mat dis_temp;//输入图像
   string  pic_name[18] ={"0","1","2","3","4","5", "6","7","8", "9","10","11","12","13","14","15","16","17" };//保存图片
	
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
       cout<<"Create graph0 failed\n";
       return 1;
   }
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
       printf("Cannot find input tensor,node_idx: %d,tensor_idx: %d\n",node_idx,tensor_idx);
       return -1;
   }
   
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
    // int a=get_tensor_shape(input_tensor,dims,4);
	// if(a!=-1)
	   // cout << "shape：" <<a<< endl;
   
   while(1)
   {
    //capture >> srcImage; //获取一帧图像
   //srcImage = imread(image_file, 0); //取出灰度图
    vector<Rect> Rect_get;//矩阵数组参数
    vector<Mat> Mat_result;	//切割得到图像矩阵
    vector<Mat> Mat_dst;//归一化图像结果
	
    //srcImage = imread(image_file, -1); //原图
	capture >> srcImage; //获取一帧图像
	if (!srcImage.data)  //错误的信息处理
	{
		cout << "Could not open or find the image" << endl;
		continue;
	}
	// else
		// cout << "pic ok"<<endl;
	
	cvtColor(srcImage, grayImage, CV_BGR2GRAY);//灰度化
	//imwrite("./image/gray.jpg", grayImage);   //  将gray图像保存为bmp  
	
	threshold(grayImage, binaryImage, 100, 255, CV_THRESH_BINARY);//二值化
	//imwrite("./image/binary.jpg", binaryImage);   // 将gray图像保存为bmp  
	
	 //threshold(gray, dst, 0, 255, CV_THRESH_OTSU);//大津阈值法
	bitwise_not(binaryImage, dis_binary);//颜色反转
	//imwrite("./image/dis_binary.jpg", dis_binary);   //  将gray图像保存为bmp  
	
	//cout << "bitwise_not ok"<<endl;
	
	dis_temp=dis_binary.clone();//保存一份
	get_num_Rect(dis_binary, Rect_get);//获取轮廓
	//imwrite("./image/dis_binary1.jpg", dis_binary);  //  将gray图像保存为bmp  
	
	//cout << "get_num_Rect ok"<<endl;
	//if(Rect_get.size()>0)
	if (Rect_get.size() > 0&& Rect_get.size() < 25)//最多支持25个数字
	{
	get_div_Rect(Rect_get);//图像分割排序
	//imwrite("./image/dis_binary2.jpg", dis_binary); //  将gray图像保存为bmp  

	get_get_div_Image(dis_temp, Rect_get, Mat_result);//获取图片，切割图片
	
	for (unsigned int i = 0; i < Mat_result.size(); i++)
	{
	Mat gray_Image = Mat_result[i];//获取输入
	//imwrite("./image1/"+ pic_name[i]+".bmp", gray_Image);   //  将gray图像保存为bmp  
	//cout<<"time="<<i<<endl;
	}
	
	get_expand_Image(Mat_result, Mat_dst, 60);//图片扩大范围
	
	for (unsigned int i = 0; i < Mat_dst.size(); i++)
	{
	Mat gray_Image = Mat_dst[i];//获取输入
	//imwrite("./image/"+ pic_name[i]+".bmp", gray_Image);   //  将gray图像保存为bmp  
	//cout<<"time="<<i<<endl;
	
     /* prepare input data */
    get_input_data( gray_Image, input_data, img_h, img_w);//获取输入
   
     /* setup input buffer */
/*******************************************************************
* 名称：             set_tensor_buffer
* 功能：             设置张量数据缓冲区
* 入口参数：tensor_t input_tensor：张量句柄
            void *buffer：数据缓冲区指针（需要释放）
            int size：缓冲区大小（4维）
* 出口参数：       0：成功    -1：失败
*******************************************************************/
   if(set_tensor_buffer(input_tensor,input_data,1*img_h*img_w*4)<0)
   {
       printf("Set buffer for tensor failed\n");
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

   int num_temp = PrintTopLabels(lenet_label_file, data);
   int Rect_num_x = Rect_get[i].x+Rect_get[i].width;
   int Rect_num_y = Rect_get[i].y+Rect_get[i].height;
   
   if(num_temp!=-1)
   {
	 cout<<num_temp<<"("<<Rect_num_x<<","<<Rect_num_y<<")"<<" "; 
	 //cout<<num_temp<<"("<<(Rect_get[i].x+Rect_get[i].width)<<","<<(Rect_get[i].y+Rect_get[i].height)<<")"<<" "; 
	 if(num_temp<10)
	 {
	 //unsigned char num_temp =((char)num_temp&0x0f)+48;
	 unsigned char temp[5];
	 int len=0;
	 temp[0]=((unsigned char)num_temp&0x0f)+48;
	 temp[1] = ' ';
	 len=UART_Send(fd,(unsigned char*)temp,2);//数字
	 
	 int2char_num(Rect_num_x,temp);
	 len=UART_Send(fd,(unsigned char*)temp,4);//x
			
	 int2char_num(Rect_num_y,temp);
	 len=UART_Send(fd,(unsigned char*)temp,4);//y
	 } 
   }
   }
   unsigned char temp_next[2]={' ','\n'};
   UART_Send(fd,(unsigned char*)temp_next,2);//y
   cout<<endl;
   }
   }
   put_graph_tensor(output_tensor);//释放输出张量句柄
   put_graph_tensor(input_tensor);//释放输入张量句柄
   postrun_graph(graph); //释放运行图资源
   destroy_runtime_graph(graph);//销毁运行图
   remove_model(model_name.c_str());//移除模型文件
   cout<<"ALL TEST DONE\n";
   release_tengine_library();   //资源释放操作
   free(input_data);
   input_data=NULL;//free释放堆空间后，必须把无效指针变为空
   return 0;
}