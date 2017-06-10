cmake_minimum_required(VERSION 3.7)
project(VideoKit)

set(CMAKE_C_STANDARD 99)
include_directories(./include/)
link_directories(./lib/)
set(SOURCE_FILES src/main.c include/java.h include/utils.h include/error.h)
add_executable(VideoKit ${SOURCE_FILES})
target_link_libraries(
        VideoKit
        avcodec
        avdevice
        avfilter
        avformat
        avresample
        avutil
        postproc
        swresample
        swscale
)