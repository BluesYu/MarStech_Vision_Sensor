# ------------------------------------------FileInfo-----------------------------------------------------
# File name:              CmksList.txt
# Created by:             BluesYu  
# Last modified Date:    2019-04-23
# Last Version:               1.0
# Descriptions:       模型编译所需设置     
# ------------------------------------------------------------------------------------------------------
cmake_minimum_required (VERSION 2.8)
project(LeNet)

set( TENGINE_DIR ~/Tengine)
#set( TENGINE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../ )
set( INSTALL_DIR ${TENGINE_DIR}/install/ )
set( TENGINE_LIBS tengine protobuf)

#flag
set(CMAKE_CFLAGS "-std=gnu89 -O3 -Wall")
set(CMAKE_CXX_FLAGS "-std=c++11 -O3 -Wall")

#opencv
find_package(OpenCV REQUIRED)

#include
include_directories(${INSTALL_DIR}/include)
include_directories(./uart_io)
include_directories(~/lenet_num_mode)

#lib
link_directories(${INSTALL_DIR}/lib)
find_package(OpenCV REQUIRED)

#exe
add_executable(lenet_test lenet_test.cpp lenet_num_mode.cpp uart_io/uart_io.cpp)#将名为.cpp的源文件编译成一个名称为lenet_test的可执行文件
target_link_libraries(lenet_test ${TENGINE_LIBS} ${OpenCV_LIBS})#将目标文件与库文件进行链接
