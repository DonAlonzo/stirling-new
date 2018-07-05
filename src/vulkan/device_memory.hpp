#pragma once

#include "deleter.hpp"

#include <vulkan/vulkan.h>

namespace stirling { namespace vulkan {

    struct DeviceMemory {
        DeviceMemory(
            VkDevice              device,
            VkBuffer              buffer,
            VkMemoryPropertyFlags flags);

    private:
        Deleter<VkDeviceMemory> memory;
    };

}}