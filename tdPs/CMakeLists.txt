project(ps)

set(srcs 
	src/TdPs.cpp)
 
include_directories(. "inc" )

add_library(ps STATIC ${incs}  ${srcs})
target_link_libraries (ps -lpthread -lm)
