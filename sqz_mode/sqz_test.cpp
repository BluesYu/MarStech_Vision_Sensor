#include <unistd.h>
#include <iostream> 
#include <functional>
#include <algorithm>
#include <fstream>
#include <iomanip>
#include <time.h>
#include <sys/time.h>
#include "sqz_mode.h"
#include "uart_io/uart_io.h"

using namespace std;
using namespace cv;
using namespace TEngine;


int main(int argc, char * argv[])
{  
   int img_h=227;//归一化图像大小227*227
   int img_w=227;
    string model_name = "squeeze_net";//名称
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
			const char* sqz_text_file：模型名称；
			const char* sqz_model_file：模型文件；
* 出口参数：        0：成功    -1：失败
*******************************************************************/
   if(load_model(model_name.c_str(),"caffe",sqz_text_file,sqz_model_file)<0)
      return 1; 
    cout<<"Load model successfully\n";
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

   /* get input tensor */
   int node_idx = 0;
   int tensor_idx = 0;
   tensor_t input_tensor;//输入
   tensor_t  output_tensor;//输出
   float  * input_data;//输入数据规格
    VideoCapture capture(0);//打开摄像头
    Mat srcImage;
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
        printf("Cannot find input tensor,node_idx: %d,tensor_idx: %d\n",node_idx,tensor_idx);
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
	    cout << "维度数目：" <<a<<  endl;

   while(1)
   {

    capture >> srcImage; //获取一帧图像
	 //Mat srcImage=imread("a.jpg");
	if (!srcImage.data)  //错误的信息处理
	{
		 cout << "Could not open or find the image" <<  endl;
		continue;
		//getchar();
		// return -1;
	}	
   /* prepare input data */
   get_input_data_sqz( srcImage, input_data, img_h, img_w, sqz_channel_mean,1);//获取输入
   
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
   string name_labels = "0" ;
   int rate_num = 0;
   PrintTopLabels_sqz(sqz_label_file,data,name_labels,rate_num);//打印图片标记
   int fd = UART_Init();
   if(rate_num != 0)
   {
    //cout << rate_num<< " - \"" << name_labels<< "\"\n";
    char sqz_temp[128];
	unsigned char temp_array[8];
	int len = 0;
	int len_buff=name_labels.size();
	int2char_num(rate_num,temp_array);
	strcpy(sqz_temp,name_labels.c_str());
	len=UART_Send(fd,(unsigned char*)sqz_temp,len_buff);//发送数据
	for(int i = 5;i>=3;i--)
	{
		temp_array[i] = temp_array[i-3];
	}
	temp_array[0]='(';
	temp_array[1]='0';
	temp_array[2]='.';
	temp_array[6]=')';
	temp_array[7]='\n';
	len=UART_Send(fd,(unsigned char*)temp_array,8);//发送数据
   }
   //PrintTopLabels_sqz(sqz_label_file, data);
   
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

