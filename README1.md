大标题  
====

中标题  
------

这是一段普通的文本，  
直接回车不能换行，<br>  
要使用\<br>
    欢迎到访  
Thank `You` . Please `Call` Me `Coder`
[我的博客](http://blog.csdn.net/guodongxiaren)

* 昵称：果冻虾仁  
* 别名：隔壁老王  
* 英文名：Jelly


#MarStech Vision Sensor  
====
1，用于GPIO和硬件（按键、LED）；
2，按键：key1，key2，key3依次对应为：GPA0,GPA6,GPA2,低电平有效;
2，LED：led为补光led，对应GPA2，高电平有效;
        led1(GN)，led2(YL)，led3(BU)依次对应为：GPC0,GPC1,GPA1,低电平有效;
3, 测试程序编译命令为：
g++ main.cpp gpio_pi/gpio_pi.cpp gpio_pi/key_pi.cpp gpio_pi/led_pi.cpp  uart_io/uart_io.cpp  color_mode/color_mode.cpp track_mode/track_color_mode.cpp bar_mode/bar_mode.cpp -o main_test -lopencv_core -lopencv_highgui -lopencv_imgproc -lpthread -std=c++11
