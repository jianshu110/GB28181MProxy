project(gb28181)

set(srcs 
	src/TdNetClient.cpp
    src/TdPugixml.cpp
    src/TdDevice.cpp)
 
include_directories(. "inc")

add_library(gb28181 STATIC ${incs}  ${srcs})
target_link_libraries (gb28181 eXosip2 osip2 osipparser2 pthread resolv util rtp channel)