cmake_minimum_required(VERSION 2.8)

project(CDCapture)

set(Boost_USE_STATIC_LIBS OFF) 
set(Boost_USE_MULTITHREADED ON)  
set(Boost_USE_STATIC_RUNTIME OFF) 

find_package(Boost COMPONENTS filesystem date_time REQUIRED) 

find_package( OpenCV REQUIRED )

include_directories(${Boost_INCLUDE_DIRS} $ENV{OPENNI2_INCLUDE})

link_directories(${LINK_DIRECTORIES} $ENV{OPENNI2_LIB} ${BOOST_LIBRARYDIR})
 
add_executable(CDCapture src/RGBDCapture.cpp src/Exception.h src/OpenNICamera.h) 

target_link_libraries(CDCapture ${Boost_LIBRARIES} ${OpenCV_LIBS} OpenNI2)
