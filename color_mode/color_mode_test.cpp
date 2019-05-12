/******************************************************************************************************
* 主函数(Function)：int main（）
* 功能(Description)： 测试颜色模式
* 调用函数（Calls）：get_color_mode()等
* 输入参数（parameter）：无
* 返回数值(return)： return 0
* 其他(Others):用于测试使用，用来opencv库函数，依赖uart函数。
********************************************************************************************************/
#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>
#include <string>
#include <vector>
#include "color_mode.h"
#include "uart_io/uart_io.h"

using namespace cv;
using namespace std;

VideoCapture capture(0);   // 打开摄像头

int main(int argc, char** argv) {
	if (!capture.isOpened())
	{
		printf("could not load camera...\n");
		return -1;
	}
	
	int fd = UART_Init();
	int tx_length=3;
	unsigned char Read_buffer[256];
	unsigned char tx_buffer[3]={'H','X','R'};
	
	Mat frame;
	while (true) {
		bool ret = capture.read(frame); //读取摄像头的一帧图像
		if (!ret) continue;
		
		int temp_len=UART_Read(fd, Read_buffer);
	    if(temp_len>0)
	       printf("%i Read_buffer bytes read : %s\n", temp_len, Read_buffer);
		
		//imshow("SRC", frame);
		//waitKey(10);
		vector<string> color_name;
		vector<Point> Rect_center;
		vector<Point> Rect_size;
		if (get_color_mode(frame, color_name, Rect_center, Rect_size) > 0)
			//get_color_mode(frame, color_name, Rect_center, Rect_size);
		{
			for(int i=0;i<color_name.size();i++)
			{
			cout << color_name[i] << " " << Rect_center[i].x << " " << Rect_center[i].y << " " \
				<< Rect_size[i].x << " " << Rect_size[i].y << endl;
			/*imshow("output", frame);
			waitKey(10);*/
			int len;
			char color_temp[3];
			strcpy(color_temp,color_name[i].c_str());
			color_temp[2]=' ';
			len=UART_Send(fd,(unsigned char*)color_temp,3);//颜色
			
			unsigned char temp[5];
			int2char_num(Rect_center[i].x,temp);
			len=UART_Send(fd,(unsigned char*)temp,4);//x
			
			int2char_num(Rect_center[i].y,temp);
			len=UART_Send(fd,(unsigned char*)temp,4);//y
			
			int2char_num(Rect_size[i].x,temp);
			len=UART_Send(fd,(unsigned char*)temp,4);//w
			
			int2char_num(Rect_size[i].y,temp);
			temp[4]='\n';
			len=UART_Send(fd,(unsigned char*)temp,5);//h	
		    }
		}
		
		
		}
	return 0;
}