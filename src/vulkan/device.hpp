#pragma once

#include "deleter.hpp"
#include "vulkan_structs.hpp"

#include <vulkan/vulkan.h>

namespace stirling { namespace vulkan {

    struct Device {
        Device(
            const MemoryAllocateInfo& allocate_info,
            VkDevice                  device);

        inline operator const VkDevice() const { return memory; }

    private:
        Deleter<VkDevice> memory;
    };

}}