#pragma once

#include "vulkan_structs.hpp"
#include "vulkan.hpp"

#include <vulkan/vulkan.h>

namespace stirling { namespace vulkan {

    struct PhysicalDevice {
        PhysicalDevice(VkPhysicalDevice physical_device);

        inline operator const VkPhysicalDevice() const { return physical_device; }

        VkPhysicalDeviceProperties get_physical_device_properties() const;
        VkPhysicalDeviceFeatures get_physical_device_features() const;
        QueueFamilyIndices get_queue_families(VkSurfaceKHR surface) const;

    private:
        VkPhysicalDevice physical_device;
    };

}}