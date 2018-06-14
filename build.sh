#!/bin/bash

#VULKAN_SDK=`pwd`/VulkanSDK/1.1.73.0/x86_64

mkdir -p build
cd build
cmake ..
make
echo
./stirling
