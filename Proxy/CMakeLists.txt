CMAKE_MINIMUM_REQUIRED(VERSION 2.8)
SET(PRIVATE_MODULE_NAME my_log_proxy)
project(${PRIVATE_MODULE_NAME})

SET(PRIVATE_MODULE_SRC_FILES
    "MY_Log_Proxy_IF.cpp"
    "MY_Log_Manager.cpp"
    "MY_Log_Proxy_Impl.cpp")

add_library(${PRIVATE_MODULE_NAME} SHARED ${PRIVATE_MODULE_SRC_FILES})
target_link_libraries(${PRIVATE_MODULE_NAME} -lpthread -lz)