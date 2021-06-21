cmake_minimum_required(VERSION 3.1)

project(ssh-tcpip)

add_executable(${PROJECT_NAME}-host "src/host.c")
target_include_directories(${PROJECT_NAME}-host PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
if (MINGW)
target_link_library(${PROJECT_NAME}-host wsock32 wininet)
endif()

if (UNIX)
add_executable(${PROJECT_NAME}-remote "src/remote.c")
target_include_directories(${PROJECT_NAME}-remote PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
endif()

