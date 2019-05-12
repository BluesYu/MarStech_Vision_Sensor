1，需要使用摄像头；
2，采集图像信息，通过串口发送；
3，依赖简易的opencv库函数，需要白平衡算法；
4, 测试函数编译命令为：g++ color_mode_test.cpp color_mode.cpp uart_io.cpp -o color_mode_test -lopencv_core -lopencv_highgui -lopencv_imgproc -lpthread -std=c++11

