#pragma once

#include "deleter.hpp"
#include "device_memory.hpp"
#include "vulkan_structs.hpp"

#include <vulkan/vulkan.h>

namespace stirling { namespace vulkan {

    struct Buffer {
        Buffer(
            const BufferCreateInfo& create_info,
            VkDevice                device,
            VkPhysicalDevice        physical_device);

    private:
        Deleter<VkBuffer> buffer;
    };

}}