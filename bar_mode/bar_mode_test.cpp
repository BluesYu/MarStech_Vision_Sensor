/******************************************************************************************************
* 主函数(Function)：    main()
* 功能(Description)： 测试条形码检测函数
* 调用函数（Calls）： get_bar_num（）
* 输入参数（parameter）：
* 返回数值(return)：     
* 其他(Others):     函数需要调节摄像头焦距。
********************************************************************************************************/
#include <string.h>
#include <unistd.h>
#include <iostream>
#include <unordered_map>
#include "bar_mode.h"
#include "uart_io/uart_io.h"

VideoCapture capture(0);

using namespace std;

int main() {
	Mat frame;
	
		
	int fd = UART_Init();
	int tx_length=3;
	unsigned char Read_buffer[256];
	unsigned char tx_buffer[3]={'H','X','R'};
	
	//if (1)
	while (1)
	{
		capture >> frame;
		//srcImg = imread(str1);
		//imshow("1,原始图片", srcImg);
		//cvWaitKey(10);
		if (frame.data == NULL)
		{
			cout<<"No Camera"<<endl;
			sleep(1);
			continue;
		}
		string result = "";       //参数解析
		if (get_bar_num(frame, result) != "ERROR")
		{
		cout <<"code:"<<result << endl;
		int len;
		char color_temp[20];
		int len_buff=result.size();
		if(len_buff!=0&&len_buff<20)
		{
		strcpy(color_temp,result.c_str());
		color_temp[len_buff]='\n';
		len=UART_Send(fd,(unsigned char*)color_temp,len_buff+1);//发送数据
		}
	}
	//cvReleaseImage(&srcImg);
	return 0;
}