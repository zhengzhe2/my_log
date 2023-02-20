CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

set(CMAKE_CXX_STANDARD 14)

SET(PRIVATE_MODULE_NAME my_log_test_app)

project(${PRIVATE_MODULE_NAME})

include_directories(
        ${CMAKE_SOURCE_DIR}/ProxyWrapper
)

add_executable(${PRIVATE_MODULE_NAME} main.cpp)
target_link_libraries(${PRIVATE_MODULE_NAME} -lz 
-L${CMAKE_SOURCE_DIR}/build/Proxy -lmy_log_proxy
-L${CMAKE_SOURCE_DIR}/build/ProxyWrapper -lmy_log_proxy_wrapper)