CMAKE_MINIMUM_REQUIRED(VERSION 2.8)
SET(PRIVATE_MODULE_NAME my_log_proxy_wrapper)
project(${PRIVATE_MODULE_NAME})

include_directories(
    "${CMAKE_SOURCE_DIR}/Proxy"
)

SET(PRIVATE_MODULE_SRC_FILES
    "my_log_proxy_wrapper.cpp")

add_library(${PRIVATE_MODULE_NAME} SHARED ${PRIVATE_MODULE_SRC_FILES})
target_link_libraries(${PRIVATE_MODULE_NAME} 
-L${CMAKE_SOURCE_DIR}/build/Proxy -lmy_log_proxy)