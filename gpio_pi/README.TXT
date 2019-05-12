1，用于GPIO和硬件（按键、LED）；
2，按键：key1，key2，key3依次对应为：GPA0,GPA6,GPA2,低电平有效;
2，LED：led为补光led，对应GPA2，高电平有;
        led1(GN)，led2(YL)，led3(BU)依次对应为：GPC0,GPC1,GPA1,低电平有效;
3, 测试程序编译命令为：
g++ gpio_test.cpp gpio_pi.cpp -o gpio_test -std=c++11
g++ key_test.cpp key_pi.cpp  gpio_pi.cpp -o key_test -std=c++11
g++ led_pi_test.cpp led_pi.cpp  gpio_pi.cpp -o led_test -std=c++11
