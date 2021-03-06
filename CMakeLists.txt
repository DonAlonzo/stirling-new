cmake_minimum_required(VERSION 3.11)
project(stirling)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_compile_options(
    "$<$<CXX_COMPILER_ID:Clang>:-O3>"
    "$<$<CXX_COMPILER_ID:Clang>:-Wno-narrowing>"
    "$<$<CXX_COMPILER_ID:Clang>:-Wno-c++11-narrowing>"
)

# Dependencies

find_package(Vulkan REQUIRED)
find_package(glfw3 3.2 REQUIRED)

# Stirling Engine

add_executable(${PROJECT_NAME} "")

set(${PROJECT_NAME}_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
target_sources(${PROJECT_NAME}
    PUBLIC
        ${${PROJECT_NAME}_SOURCE_DIR}/vulkan/buffer.cpp
        ${${PROJECT_NAME}_SOURCE_DIR}/vulkan/command_buffer.cpp
        ${${PROJECT_NAME}_SOURCE_DIR}/vulkan/command_pool.cpp
        ${${PROJECT_NAME}_SOURCE_DIR}/vulkan/descriptor_pool.cpp
        ${${PROJECT_NAME}_SOURCE_DIR}/vulkan/descriptor_set.cpp
        ${${PROJECT_NAME}_SOURCE_DIR}/vulkan/device.cpp
        ${${PROJECT_NAME}_SOURCE_DIR}/vulkan/device_memory.cpp
        ${${PROJECT_NAME}_SOURCE_DIR}/vulkan/fence.cpp
        ${${PROJECT_NAME}_SOURCE_DIR}/vulkan/instance.cpp
        ${${PROJECT_NAME}_SOURCE_DIR}/vulkan/physical_device.cpp
        ${${PROJECT_NAME}_SOURCE_DIR}/vulkan/pipeline.cpp
        ${${PROJECT_NAME}_SOURCE_DIR}/vulkan/surface.cpp
        ${${PROJECT_NAME}_SOURCE_DIR}/vulkan/swapchain.cpp
        ${${PROJECT_NAME}_SOURCE_DIR}/vulkan/queue.cpp
        ${${PROJECT_NAME}_SOURCE_DIR}/file.cpp
        ${${PROJECT_NAME}_SOURCE_DIR}/main.cpp
        ${${PROJECT_NAME}_SOURCE_DIR}/window.cpp)

target_include_directories(${PROJECT_NAME}
    PUBLIC
        ${${PROJECT_NAME}_SOURCE_DIR}
        ${Vulkan_INCLUDE_DIR})

target_link_libraries(${PROJECT_NAME}
    glfw
    ${Vulkan_LIBRARY})
