project(rtp)

set(srcs 
	src/TdRtp.cpp)
 
include_directories(. "inc" "/usr/local/include/jrtplib3" "../tdCodec")

add_library(rtp STATIC ${incs}  ${srcs})
target_link_libraries (rtp -lpthread -lm -ljrtp codec scale ps)