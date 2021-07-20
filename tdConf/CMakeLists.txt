project(conf)
 
set(srcs 
	src/TdConf.cpp)
 
include_directories(. "inc")

add_library(conf STATIC ${incs}  ${srcs})
target_link_libraries (conf -lm util)