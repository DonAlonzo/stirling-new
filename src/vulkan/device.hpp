#pragma once

#include "deleter.hpp"
#include "vulkan_structs.hpp"

#include <vulkan/vulkan.h>

namespace stirling { namespace vulkan {

    struct Device {
        Device(
            const DeviceCreateInfo& create_info,
            VkPhysicalDevice        physical_device);

        inline operator const VkDevice() const { return device; }

    private:
        Deleter<VkDevice> device;
    };

}}