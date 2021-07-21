cmake_minimum_required (VERSION 3.5 FATAL_ERROR)
project(ffmpeg_test)
 
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -O -O2 -g -fPIC")
 
include_directories (${INCLUDE_DIRS})
 
link_directories    (${LIBRARY_DIRS})
add_definitions     (${DEFINITIONS})

set(FFMPEG_LIB /home/tdwl/ffmpeg_lib_n4.4/)
 
 
include_directories(${FFMPEG_LIB}/include)
include_directories(/usr/local/include/jrtplib3/)
include_directories(/usr/local/include/wels/)
include_directories(/usr/local/include/)
include_directories(${PROJECT_SOURCE_DIR}/tdCodec)
include_directories(${PROJECT_SOURCE_DIR}/tdScale)
include_directories(${PROJECT_SOURCE_DIR}/tdServer)
include_directories(${PROJECT_SOURCE_DIR}/tdPs)
include_directories(${PROJECT_SOURCE_DIR}/tdRtp)
include_directories(${PROJECT_SOURCE_DIR}/tdConf)
include_directories(${PROJECT_SOURCE_DIR}/tdUtil)
include_directories(${PROJECT_SOURCE_DIR}/tdGb28181)
include_directories(${PROJECT_SOURCE_DIR}/tdChannel)
include_directories(${PROJECT_SOURCE_DIR}/tdUtil/inc)
link_directories(${PROJECT_BINARY_DIR}/libs)
link_directories(/usr/local/lib/)
link_directories(${FFMPEG_LIB}/lib)

set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/libs)
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR})

add_subdirectory(tdRtp)
add_subdirectory(tdScale)
add_subdirectory(tdCodec)
add_subdirectory(tdPs)
add_subdirectory(tdServer)
add_subdirectory(tdConf)
add_subdirectory(tdUtil)
add_subdirectory(tdGb28181)
add_subdirectory(tdChannel)
add_subdirectory(gmproxy)