project(channel)

set(srcs 
	src/TdChanManager.cpp)
 
include_directories(. "inc")

add_library(channel STATIC ${incs}  ${srcs})
target_link_libraries (channel -lpthread -lm rtp)