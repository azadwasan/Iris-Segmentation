cmake_minimum_required(VERSION 3.7)

add_definitions(-std=c++17)

find_package( OpenCV REQUIRED )

set(CXX_FLAGS "-Wall")
set(CMAKE_CXX_FLAGS, "${CXX_FLAGS}")

project(IrisRecognition)

add_executable(IrisRecognition
     src/main.cpp
     src/irisSegmentation.cpp
     src/MaxLocation.cpp)

target_link_libraries( IrisRecognition ${OpenCV_LIBS} )
