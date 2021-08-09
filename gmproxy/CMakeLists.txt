project(GMProxy_bin)

include_directories("../scale" "inc" "../tdRtp")
add_executable(GB28181MProxy_bin main.cpp)

target_link_libraries(GB28181MProxy_bin scale rtp server gb28181 codec)