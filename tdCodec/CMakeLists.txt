project(codec)
 
set(srcs 
	src/TdH264.cpp
	src/TdCodec.cpp)
 
include_directories(. "inc")

add_library(codec STATIC ${incs}  ${srcs})
target_link_libraries (codec -lpthread -lm -lopenh264 scale util)