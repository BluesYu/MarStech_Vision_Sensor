MarStech_Vision_Sensor
====================================================
   MarStech_Vision_Sensor：With the development of computer and artificial intelligence technology, teenager robot programming education 
 is becoming more and more popular, and the related teaching equipment has gradually become a research hotspot. Visual sensor is the core component of relevant teaching equipment, and also the key module affecting the intelligent performance of robots. The existing visual sensors can achieve a single function, and the accuracy of recognition and classification is low. At the same time, the higher price is difficult to popularize in the education market, so they can not meet the market demand. The system based on low-cost omnidirectional H5 chip can not only run digital image processing algorithm, but also build a lightweight neural network framework, which is suitable for the design of visual sensor for robot programming education.
   火星人视觉传感器：随着计算机和人工智能技术的日益发展，青少年机器人编程教育也越来越流行，相关的教学设备逐渐成为研究的热点。视觉传感器是相关教学设备的核心部件，也是影响机器人智能化表现的关键模块。现有视觉传感器可实现功能单一，且识别、分类准确率较低，同时价格较高难以在教育市场推广普及，因而无法满足市场需求。本传感器采用的以'低成本'全志H5芯片为基础的系统不仅可以运行'数字图像处理算法'，还可以搭建'轻量级神经网络框架'，适合用于机器人编程教育视觉传感器的设计。

Hardware Circuit Design（硬件电路设计）
-------------------------------------
* Key（按键）：key1，key2，key3依次对应为：GPA0,GPA6,GPA2,低电平有效;
* LED（显示）：led为补光led，对应GPA2，高电平有效;
             led1(GN)，led2(YL)，led3(BU)依次对应为：GPC0,GPC1,GPA1,低电平有效;

Construction of Development Environment（开发环境搭建）
-----------------------------------------------------
 *  [开发环境搭建](http://blog.csdn.net/guodongxiaren)
 * 测试程序编译命令为：
g++ main.cpp gpio_pi/gpio_pi.cpp gpio_pi/key_pi.cpp gpio_pi/led_pi.cpp  uart_io/uart_io.cpp  color_mode/color_mode.cpp track_mode/track_color_mode.cpp bar_mode/bar_mode.cpp -o main_test -lopencv_core -lopencv_highgui -lopencv_imgproc -lpthread -std=c++11


（Code Organization）代码结构
-------------------------------------
* bar_mode：
* color_mode：
* track_color_mode：
* lenet_num_mode：
* lenet：
* sqz_mode：
* sqz_net：
* gpio_pi：
* uart_io：

Author（作者）
-------------------------------------
* Author(作者)：BluesYu
* E-mail(邮箱)：254972181@qq.com
