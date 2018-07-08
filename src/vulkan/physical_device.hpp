#pragma once

#include "device.hpp"
#include "vulkan_structs.hpp"
#include "vulkan.hpp"

#include <vulkan/vulkan.h>

namespace stirling { namespace vulkan {

    struct PhysicalDevice {
        PhysicalDevice(VkPhysicalDevice physical_device);

        inline operator const VkPhysicalDevice() const { return physical_device; }
        
        Device create_device(const DeviceCreateInfo& create_info) const;

        VkPhysicalDeviceProperties get_properties() const;
        VkPhysicalDeviceFeatures get_features() const;
        QueueFamilyIndices get_queue_families(const Surface& surface) const;

    private:
        VkPhysicalDevice physical_device;
    };

}}