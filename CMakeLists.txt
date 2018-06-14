cmake_minimum_required(VERSION 3.11)
project(stirling)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Dependencies

find_package(Vulkan REQUIRED)
find_package(glfw3 3.2 REQUIRED)

# Stirling Engine

add_executable(${PROJECT_NAME} "")

set(${PROJECT_NAME}_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
target_sources(${PROJECT_NAME}
    PUBLIC
        ${${PROJECT_NAME}_SOURCE_DIR}/deleter.cpp
        ${${PROJECT_NAME}_SOURCE_DIR}/main.cpp)

target_include_directories(${PROJECT_NAME}
    PUBLIC
        ${${PROJECT_NAME}_SOURCE_DIR}
        ${Vulkan_INCLUDE_DIR})

target_link_libraries(${PROJECT_NAME}
    glfw
    ${Vulkan_LIBRARY})
