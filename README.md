MarStech_Vision_Sensor
====================================================
   * MarStech_Vision_Sensor：With the development of computer and artificial intelligence technology, teenager robot programming education is becoming more and more popular, and the related teaching equipment has gradually become a research hotspot. Visual sensor is the core component of relevant teaching equipment, and also the key module affecting the intelligent performance of robots. The existing visual sensors can achieve a single function, and the accuracy of recognition and classification is low. At the same time, the higher price is difficult to popularize in the education market, so they can not meet the market demand. The system based on **low-cost** omnidirectional H5 chip can not only run **digital image processing algorithm**, but also build a **lightweight neural network framework**, which is suitable for the design of visual sensor for robot programming education
    （火星人视觉传感器：随着计算机和人工智能技术的日益发展，青少年机器人编程教育也越来越流行，相关的教学设备逐渐成为研究的热点。视觉传感器是相关教学设备的核心部件，也是影响机器人智能化表现的关键模块。现有视觉传感器可实现功能单一，且识别、分类准确率较低，同时价格较高难以在教育市场推广普及，因而无法满足市场需求。本传感器采用的以**低成本**全志H5芯片为基础的系统不仅可以运行**数字图像处理算法**，还可以搭建**轻量级神经网络框架**，适合用于机器人编程教育视觉传感器的设计）

Hardware Circuit Design（硬件电路设计）
-------------------------------------
* Key（按键）：key1，key2，key3 Corresponding to（依次对应为）：GPA0,GPA6,GPA2,Low Level Effective（低电平有效）;
* LED（显示）：led1 Used for supplementary lighting（用于补光)，Corresponding to(对应于)GPA2，High Level Effectiveness（高电平有效）;
              led1(GN)，led2(YL)，led3(BU) Corresponding to（依次对应为）：GPC0,GPC1,GPA1,Low Level Effective（低电平有效）;

Construction of Development Environment（开发环境搭建）
-----------------------------------------------------
 *  [Construction of Development Environment（开发环境搭建）](https://blog.csdn.net/bluesyxx/article/details/85255634#commentsedit)
 * The test program compiles the command as follows（测试程序编译命令为）：
g++ main.cpp gpio_pi/gpio_pi.cpp gpio_pi/key_pi.cpp gpio_pi/led_pi.cpp  uart_io/uart_io.cpp  color_mode/color_mode.cpp track_mode/track_color_mode.cpp bar_mode/bar_mode.cpp -o main_test -lopencv_core -lopencv_highgui -lopencv_imgproc -lpthread -std=c++11


Code Organization （代码结构）
-------------------------------------
* bar_mode：One-dimensional barcode recognition mode, which can be used for image processing, temporarily only supports EAN13 barcode（一维条形码识别模式，可以进行图像，暂时只支持EAN13条形码）
* color_mode：The color recognition pattern code can recognize five colors, *red, yellow, green, blue and purple*, and support multiple objects（颜色识别模式代码，可识别*红、黄、绿、蓝、紫*五种颜色，支持多个物体）
* track_color_mode：Object tracking based on color only supports brightly colored objects temporarily（根据颜色进行物体跟踪，暂时只支持颜色鲜艳物体）
* lenet_num_mode：Digital recognition using LeNet-5 network to support multi-line digits（使用LeNet-5网络进行数字识别，支持多行数字）
* lenet：LeNet-5 Trained Network Model（LeNet-5训练好的网络模型）
* sqz_mode：SuqeezeNet's Thousand Classification Network Based on ImageNet Data Set（SuqeezeNet 的千分类网络，基于ImageNet数据集）
* sqz_net：SuqeezeNet Network Model（SuqeezeNet 网络模型）
* gpio_pi：Sensor port output, including keys, indicator lights and other driving functions（传感器端口输出，包括按键、指示灯等驱动函数）
* uart_io：Serial Communication Interface Function（串口通信接口函数）
* main.cpp:（主函数）

Author（作者）
-------------------------------------
* Author(作者)：Blues Yu,Wengan Shao,Chengxin Zhang，Haoqiang Yuan,Lichao Xia  
* E-mail(邮箱)：254972181@qq.com
