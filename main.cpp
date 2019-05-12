#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <pthread.h>
#include <iostream>
#include <string>
#include <vector>
#include <unistd.h>
#include <sys/time.h>
#include <memory.h>
#include <stdio.h>
#include <sys/mman.h>  
#include <sys/types.h>  
#include <sys/stat.h> 
#include <fcntl.h>  
#include <linux/gpio.h> //里面声明io口的操作函数
#include <functional>
#include <algorithm>
#include <fstream>
#include <iomanip>
 
#include "uart_io/uart_io.h"
#include "gpio_pi/gpio_pi.h" 
#include "gpio_pi/led_pi.h"
#include "gpio_pi/key_pi.h"

#include "color_mode/color_mode.h"
#include "track_mode/track_color_mode.h"
#include "bar_mode/bar_mode.h"
#include "lenet_num_mode/lenet_num_mode.h"  
#include "sqz_mode/sqz_mode.h"  


using namespace cv;
using namespace std;
using namespace TEngine;

//volatile unsigned int system_mode=0;  //系统的模式选择
static unsigned int system_mode=0;  //系统的模式选择
volatile unsigned char step=0;

int uart_fd = -1;

int set_station(int x, int y);//根据中心点，判断位置。

VideoCapture capture(0);   // 打开摄像头

pthread_t thread[6];//  6个线程
pthread_mutex_t mut;
void key_sensor();//线程1,负责按键任务
void led_sensor();//线程2，led灯任务
void uart_sensor();//线程3，UART接受任务
void muliti_fun1();//线程4
void muliti_fun2();//线程5
void muliti_fun3();//线程6

/******************************************************************************************************
* 线程函数(Function)：*thread1(void *args) ~ *thread6(void *args)
* 功能(Description)： 6个线程函数
* 输入参数（parameter）：无
* 返回数值(return)： return 0
* 其他(Others):处理不同的函数
********************************************************************************************************/
void *thread1(void *args)
{
    cout<< "thread1 : I'm thread 1，for key" << endl;
	key_sensor();
    cout<< "thread1 :main function is waiting me" << endl;
    pthread_exit(NULL);
}

void *thread2(void *args)
{
    cout<< "thread2 : I'm thread 2,for 	led" << endl;
	led_sensor();
    cout<< "thread2 :main function is waiting me" << endl;
    pthread_exit(NULL);
}

void *thread3(void *args)
{
    cout<< "thread3 : I'm thread 3" << endl;
	uart_sensor();
    cout<< "thread3 :main function is waiting me" << endl;
    pthread_exit(NULL);
}

void *thread4(void *args)
{
    cout<< "thread4 : I'm thread 4" << endl;
	muliti_fun1();
    cout<< "thread4 :main function is waiting me" << endl;
    pthread_exit(NULL);
}

void *thread5(void *args)
{
    cout<< "thread5 : I'm thread 5" << endl;
	muliti_fun2();
    cout<< "thread5 :main function is waiting me" << endl;
    pthread_exit(NULL);
}

void *thread6(void *args)
{
    cout<< "thread6 : I'm thread 4" << endl;
	muliti_fun3();
    cout<< "thread6 :main function is waiting me" << endl;
    pthread_exit(NULL);
}

/******************************************************************************************************
* 线程生成函数(Function)：thread_create（）
* 功能(Description)： 生成6个线程
* 输入参数（parameter）：无
* 返回数值(return)： return 0
* 其他(Others):线程失败需要单独处理。
********************************************************************************************************/
void thread_create(void)
{
    int temp;
    memset(&thread, 0, sizeof(thread));          
    if((temp = pthread_create(&thread[0], NULL, thread1, NULL)) != 0)     
        cout<<"thread1 is created failed" << endl;
    else
        cout<<"thread1 is created " << endl;
    if((temp = pthread_create(&thread[1], NULL, thread2, NULL)) != 0)  
        cout<<"thread2 created failed" << endl;
    else
        cout<<"thread2 is created" << endl;
	if((temp = pthread_create(&thread[2], NULL, thread3, NULL)) != 0)  
        cout<<"thread3 created failed" << endl;
    else
        cout<<"thread3 is created" << endl;
	if((temp = pthread_create(&thread[3], NULL, thread4, NULL)) != 0)  
        cout<<"thread4 created failed" << endl;
    else
        cout<<"thread4 is created" << endl;
	if((temp = pthread_create(&thread[4], NULL, thread5, NULL)) != 0)  
        cout<<"thread5 created failed" << endl;
    else
        cout<<"thread5 is created" << endl;
	if((temp = pthread_create(&thread[5], NULL, thread6, NULL)) != 0)  
        cout<<"thread6 created failed" << endl;
    else
        cout<<"thread6 is created" << endl;
}

/******************************************************************************************************
* 线程结束函数(Function)：thread_wait(void)
* 功能(Description)： 结束6个线程
* 输入参数（parameter）：无
* 返回数值(return)： 
* 其他(Others):结束6个线程
********************************************************************************************************/
void thread_wait(void)
{
    if(thread[0] !=0)
    {     
        pthread_join(thread[0],NULL);
        cout<<("thread1 is end \n");
    }
    if(thread[1] !=0) 
    {  
        pthread_join(thread[1],NULL);
        cout<<("thread2 is end \n");
    }
	if(thread[2] !=0)
    {     
        pthread_join(thread[2],NULL);
        cout<<("thread3 is end \n");
    }
    if(thread[3] !=0) 
    {  
        pthread_join(thread[3],NULL);
        cout<<("thread4 is end \n");
    }
	if(thread[4] !=0)
    {     
        pthread_join(thread[4],NULL);
        cout<<("thread5 is end \n");
    }
    if(thread[5] !=0) 
    {  
        pthread_join(thread[5],NULL);
        cout<<("thread6 is end \n");
    }
}

/******************************************************************************************************
* 主函数(Function)：main(void)
* 功能(Description)： 开启线程，监听串口
* 输入参数（parameter）：无
* 返回数值(return)： 
* 其他(Others):
********************************************************************************************************/
int main()
{
	if(led_Init()< 0)//初始化LED
	{
		printf("led Init failed\n");
		system_mode = 0x44;//系统故障	
		return -1;
	}
	if(key_Init()< 0)//初始化按键
	{
		printf("key Init failed\n");
		system_mode = 0x44;//系统故障		
	}
	uart_fd = UART_Init();
	if(uart_fd < 0)  //初始化UART
	{
		printf("UART Init failed\n");
		system_mode = 0x44;//系统故障
	}
	if (!capture.isOpened())
	{
		printf("could not load camera...\n");
		system_mode = 0x44;//系统故障
	}
	while(true)
	{
	if(system_mode == 0x44) 
	{
		led_Show(system_mode);//灯不停闪烁
		thread_wait();//结束进程
		continue;
	}
    pthread_mutex_init(&mut,NULL);//初始化进程
    cout<<"main function is created thread" << endl;
    thread_create();
    cout<<"main function is waiting thread" << endl;
    thread_wait();//结束进程
	}
    return 0;
}

/*******************************************************************
* 名称：   key_sensor
* 功能：    按键函数（持续检测）
* 入口参数：system_mode：系统模式
* 出口参数：检测得到结果
*******************************************************************/
void key_sensor()
{
	int temp_mode = 0;
	key_Press(system_mode);
}

/*******************************************************************
* 名称：   led_sensor
* 功能：    led灯函数（持续检测）
* 入口参数：system_mode：系统模式
* 出口参数：
*******************************************************************/
void led_sensor()
{
	//led_Init();
    while(1)
      {
      led_Show(system_mode);
	  }
}

/*******************************************************************
* 名称：    uart_sensor()
* 功能：    串口接受函数，根据命令，调整参数
* 入口参数：
* 出口参数：system_mode
*******************************************************************/
void uart_sensor()
{
 unsigned char Read_buffer[128];
 while(1)
 {
	int temp_len=UART_Read(uart_fd, Read_buffer);
	if(temp_len>0)
	{
       printf("%i Read_buffer bytes read : %s\n", temp_len, Read_buffer);
	   if(temp_len==1)//接收到的16进制文件
	   {
		unsigned char temp_buffer = Read_buffer[0];
	    switch(temp_buffer)
		{
			case 0x01://start
			     printf("***System Wakeup*** \n");
			     system_mode = 1;//睡眠唤醒
				 break;
			case 0x02://stop
				 printf("***Close Sensor*** \n");
				 system("poweroff");//关机
				 break;	
			case 0x03://sleep
				 printf("***Sleep Sensor*** \n");
				 system_mode = 1;//睡眠唤醒
				 break;	
			case 0x04://Open Led
				 printf("***Open Camera Led*** \n");
				 camera_led_Set(1);
				 break;
			case 0x05://Close Led
				 printf("***Close Camera Led*** \n");
				 camera_led_Set(0);
				 break;	
            case 0x06://color mode
				 printf("***Start Color Mode*** \n");
				 system_mode = 1;//颜色追踪模式开启
				 break;	
			case 0x07://color track mode
				 printf("***Start Color track Mode*** \n");
				 system_mode = 2;//颜色追踪模式开启
				 break;	
			case 0x08://bar  mode
				 printf("***Start Bar  Mode*** \n");
				 system_mode = 3;//颜色追踪模式开启
				 break;	
            case 0x09: //number  mode
				 printf("***Start Color Track to Get Target*** \n");
				 system_mode = 0x66;//颜色追踪模式获取特征
				 break;					 
			case 0x0a: //number  mode
				 printf("***Start Lenet Number Mode*** \n");
				 system_mode = 4;//颜色追踪模式开启
				 break;	
			case 0x0b: //number  mode
				 printf("***Start 1000-Classification Mode*** \n");
				 system_mode = 5;//颜色追踪模式开启
				 break;		
            default: break;				 
		}
	   }
	}
 }
}

/*******************************************************************
* 名称：   muliti_fun1()
* 功能：    多线程函数1
* 入口参数：system_mode：系统模式
* 出口参数：
*******************************************************************/
void muliti_fun1()//线程1
{
	Mat frame;
	bool ret = 0;
	while (true) 
	{
	//usleep(100);
	//cout << "system_mode="<<system_mode<<endl;
	if(system_mode==0)
	{
		usleep(100);
		//cout << "system_mode0="<<system_mode<<endl;
	}
    else  if(system_mode==1) //颜色识别模式
			{
			//cout << "system_mode1="<<system_mode<<endl;
			ret = capture.read(frame); //读取摄像头的一帧图像
			if (!ret) continue;
			vector<string> color_name;
			vector<Point> Rect_center;
			vector<Point> Rect_size;
			if (get_color_mode(frame, color_name, Rect_center, Rect_size) > 0)
			{
				for(int i=0;i<color_name.size();i++)
				{
				cout << color_name[i] << " " << Rect_center[i].x << " " << Rect_center[i].y << " " \
					<< Rect_size[i].x << " " << Rect_size[i].y << endl;
				int len;
				char color_temp[3];
				strcpy(color_temp,color_name[i].c_str());
				color_temp[2]=' ';
				
				len=UART_Send(uart_fd,(unsigned char*)color_temp,3);//颜色
				unsigned char temp[5];
				int2char_num(Rect_center[i].x,temp);
				len=UART_Send(uart_fd,(unsigned char*)temp,4);//x
				
				int2char_num(Rect_center[i].y,temp);
				len=UART_Send(uart_fd,(unsigned char*)temp,4);//y
				
				int2char_num(Rect_size[i].x,temp);
				len=UART_Send(uart_fd,(unsigned char*)temp,4);//w
				
				int2char_num(Rect_size[i].y,temp);
				temp[4]='\n';
				len=UART_Send(uart_fd,(unsigned char*)temp,5);//h
				
				}
			}
			}
			else if(system_mode==2)//目标跟踪模式
			{
				//cout << "system_mode2="<<system_mode<<endl;
				ret = capture.read(frame); //读取摄像头的一帧图像
				if (!ret) continue;
				if (track_have)
				{
					Point center, w_h;
					if(track_result(frame, center, w_h)>0)
					  cout << center.x << " " << center.y << " " << w_h.x << " " << w_h.y << endl;
				  
					unsigned char temp[5];
					int len=0;
					int2char_num(center.x,temp);
					len=UART_Send(uart_fd,(unsigned char*)temp,4);//x
					
					int2char_num(center.y,temp);
					len=UART_Send(uart_fd,(unsigned char*)temp,4);//y
					
					int2char_num( w_h.x,temp);
					len=UART_Send(uart_fd,(unsigned char*)temp,4);//w
					
					int2char_num(w_h.y,temp);
					temp[4]='\n';
					len=UART_Send(uart_fd,(unsigned char*)temp,5);//h	    
				}
				else
				{
					cout<<"you should get target first!" << endl;
					system_mode=0x66;  //采集特征 
				}
			}
			else if(system_mode==0x66)//目标跟踪采集特征模式
					{
						ret = capture.read(frame); //读取摄像头的一帧图像
						if (!ret) continue;
						if (track_pic_get(frame) == 0)//获取参数
						   system_mode=2;  //进入跟踪模式
						  else
							  continue;
					}
					else if(system_mode==3)//识别条形码模式
							{
								//cout << "system_mode3="<<system_mode<<endl;
								ret = capture.read(frame); //读取摄像头的一帧图像
								if (!ret) continue;
								string result = "";       //参数解析
								if (get_bar_num(frame, result) != "ERROR")
								{
								cout <<"code:"<<result << endl;
								int len;
								char color_temp[30];
								int len_buff=result.size();
								if(len_buff!=0&&len_buff<30)
								{
								strcpy(color_temp,result.c_str());
								color_temp[len_buff]='\n';
								len=UART_Send(uart_fd,(unsigned char*)color_temp,len_buff+1);//发送数据
								}
							    }
							}
							else if(system_mode == 4)//手写数字识别模式
							{
								//cout<<"system_mode4 = 4"<<endl;
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
								 // return -1;
								 continue;
							   }
								/*******************************************************************
								* 名称：                  request_tengine_version
								* 功能：                  版本设置
								* 入口参数：
								* 出口参数：        0：不支持    1：支持
								*******************************************************************/
							   if(request_tengine_version("0.1")<0)
								   continue;
								  //return -2;
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
								   continue;
								  //return 1; 
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
									   continue;
									   //return 1;
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
									   continue;
									   //return -1;
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
   
							   while(system_mode==4)//模式4不变
								   {
									vector<Rect> Rect_get;//矩阵数组参数
									vector<Mat> Mat_result;	//切割得到图像矩阵
									vector<Mat> Mat_dst;//归一化图像结果
									ret = capture.read(srcImage); //读取摄像头的一帧图像
									if (!ret) continue;							
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
										   continue;
										  // return -1;
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
											 unsigned char temp[5];
											 int len=0;
											 temp[0]=((unsigned char)num_temp&0x0f)+48;
											 temp[1] = ' ';
											 len=UART_Send(uart_fd,(unsigned char*)temp,2);//数字
											 int2char_num(Rect_num_x,temp);
											 len=UART_Send(uart_fd,(unsigned char*)temp,4);//x
													
											 int2char_num(Rect_num_y,temp);
											 len=UART_Send(uart_fd,(unsigned char*)temp,4);//y
										 } 
									    }
									    }
										   unsigned char temp_next[2]={' ','\n'};
										   UART_Send(uart_fd,(unsigned char*)temp_next,2);//y
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
							}
							else if(system_mode==5)//千分类网络模型
							{
						//cout << "system_mode5="<<system_mode<<endl;
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
							    continue;
							  //return -1;
						   }
						/*******************************************************************
						* 名称：                  request_tengine_version
						* 功能：                  版本设置
						* 入口参数：
						* 出口参数：        0：不支持    1：支持
						*******************************************************************/
						   if(request_tengine_version("0.1")<0)
							  //return -2;
						      continue;
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
							 // return 1; 
						    continue;
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
							   //return 1;
							    continue;
						   }

						   /* get input tensor */
						   int node_idx = 0;
						   int tensor_idx = 0;
						   tensor_t input_tensor;//输入
						   tensor_t  output_tensor;//输出
						   float  * input_data;//输入数据规格
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
							   //return -1;
							    continue;
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

						   while(system_mode == 5)
						   {
							Mat srcImage1;
                            ret = capture.read(srcImage1); //读取摄像头的一帧图像
							if (!ret) 
							{
								cout << "camera busy" <<a<<  endl;
								continue;	
							}
	
						   /* prepare input data */
						   get_input_data_sqz( srcImage1, input_data, img_h, img_w, sqz_channel_mean,1);//获取输入
						   
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
							   //return -1;
							   continue;
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
		                   // cout << "system_mode="<<system_mode<<endl;
							}
	}
 }

void muliti_fun2()//线程2
{
	usleep(1);
}

void muliti_fun3()//线程3
{
	usleep(1);
}

