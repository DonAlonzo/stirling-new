#pragma once

#include "deleter.hpp"
#include "vulkan_structs.hpp"

#include <vulkan/vulkan.h>

namespace stirling { namespace vulkan {

    struct DeviceMemory {
        DeviceMemory(
            const MemoryAllocateInfo& allocate_info,
            VkDevice                  device);

        inline operator const VkDeviceMemory() const { return memory; }

    private:
        Deleter<VkDeviceMemory> memory;
    };

}}