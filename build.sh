#!/bin/bash

#VULKAN_SDK=`pwd`/VulkanSDK/1.1.73.0/x86_64

mkdir -p build
cd build

# Build Stirling Engine
cmake ..
make

# Build shaders
glslangValidator -V ../shaders/shader.vert
glslangValidator -V ../shaders/shader.frag
glslangValidator -V ../shaders/shader.geom

#Run Stirling Engine Demo
echo
./stirling
