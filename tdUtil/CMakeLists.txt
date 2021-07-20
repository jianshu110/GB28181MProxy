project(util)
 
set(srcs 
	src/json11.cpp
	src/uuid4.cpp)
 
include_directories(. "inc")

add_library(util STATIC ${incs}  ${srcs})