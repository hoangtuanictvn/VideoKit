cmake_minimum_required(VERSION 3.7)
project(VideoKit)

set(CMAKE_C_STANDARD 99)
include_directories(./include/)
link_directories(./lib/)
set(SOURCE_FILES src/main.c include/dev.h)
add_executable(VideoKit ${SOURCE_FILES})
target_link_libraries(
        VideoKit
        libavcodec.so
        libavdevice.so
        libavfilter.so
        libavformat.so
        libavutil.so
        libswresample.so
        libswscale.so
)