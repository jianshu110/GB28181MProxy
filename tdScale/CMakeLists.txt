project(scale)
 
set(srcs 
	src/TdScale.cpp)
 
include_directories(. "inc")

add_library(scale STATIC ${incs}  ${srcs})
target_link_libraries (scale -lpthread -lm -lyuv -ljpeg)