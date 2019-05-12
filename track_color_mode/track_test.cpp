#include <iostream>
#include <vector>
#include <string>
#include "track_color_mode.h"
#include "uart_io/uart_io.h"

using namespace std;
using namespace cv;

VideoCapture capture(0);   // 打开摄像头

int main()
{
	bool first = true;
	
	Mat frame;             // 原始图像     
	Rect rect_track;           // 选中的区域
	if (!capture.isOpened())
	{
		cout << "can't open camera!" << endl;
		return 0;
	}

	int fd = UART_Init();
	int tx_length=3;
	unsigned char Read_buffer[256];
	unsigned char tx_buffer[3]={'H','X','R'};
	
	while (1)
	{
		capture >> frame;
		if (frame.data == NULL)
			continue;
		if (track_pic_get(frame) == 0)//获取参数
			break;
	}
	
	while (1)
	{
		capture >> frame;
		if (frame.data == NULL)
			continue;

		if (track_have)
		{
			Point center, w_h;
			if(track_result(frame, center, w_h)>0)
			  cout << center.x << " " << center.y << " " << w_h.x << " " << w_h.y << endl;
		  
			unsigned char temp[5];
			int len=0;
			int2char_num(center.x,temp);
			len=UART_Send(fd,(unsigned char*)temp,4);//x
			
			int2char_num(center.y,temp);
			len=UART_Send(fd,(unsigned char*)temp,4);//y
			
			int2char_num( w_h.x,temp);
			len=UART_Send(fd,(unsigned char*)temp,4);//w
			
			int2char_num(w_h.y,temp);
			temp[4]='\n';
			len=UART_Send(fd,(unsigned char*)temp,5);//h	    
		}
	}

	return 1;
}