project(GMProxy_bin)

include_directories("../scale" "inc" "../tdRtp")
add_executable(GMProxy_bin main.cpp)

target_link_libraries(GMProxy_bin scale rtp server gb28181 codec)