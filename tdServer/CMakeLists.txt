project(server)

set(srcs 
	src/TdRedis.cpp)
 
include_directories(. "inc" )

add_library(server STATIC ${incs}  ${srcs})
target_link_libraries (server -lpthread -lm -lcpp_redis -ltacopie conf)
