#pragma once

#include "vulkan_structs.hpp"

#include <vulkan/vulkan.h>

namespace stirling { namespace vulkan {

    struct PhysicalDevice {
        PhysicalDevice(VkPhysicalDevice physical_device);

        inline operator const VkPhysicalDevice() const { return physical_device; }

    private:
        VkPhysicalDevice physical_device;
    };

}}