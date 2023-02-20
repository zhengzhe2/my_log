CMAKE_MINIMUM_REQUIRED(VERSION 2.8)
# arm toolchain file
# include("./toolchain.cmake") 
project("my_log")

message("xxxxx "${CMAKE_C_COMPILER})
if (${CMAKE_C_COMPILER} STREQUAL /home/$ENV{USER}/proj/gcc-linaro-6.5.0-2018.12-x86_64_aarch64-linux-gnu/bin/aarch64-linux-gnu-gcc)
set(BUILD_ENV "arm64")
else()
set(BUILD_ENV "x86")
endif()
message(${BUILD_ENV})

add_definitions(-D LINUX_OS)

add_subdirectory(Proxy)
add_subdirectory(ProxyWrapper)
add_subdirectory(testApp)
