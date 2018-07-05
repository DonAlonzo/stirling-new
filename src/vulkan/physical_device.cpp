#include "physical_device.hpp"
#include "vulkan.hpp"

namespace stirling { namespace vulkan {

    PhysicalDevice::PhysicalDevice(VkPhysicalDevice physical_device) :
        physical_device (physical_device) {
    }

    VkPhysicalDeviceProperties PhysicalDevice::get_physical_device_properties() const {
        return vulkan::get_physical_device_properties(physical_device);
    }

    VkPhysicalDeviceFeatures PhysicalDevice::get_physical_device_features() const {
        return vulkan::get_physical_device_features(physical_device);
    }

    QueueFamilyIndices PhysicalDevice::get_queue_families(VkSurfaceKHR surface) const {
        return vulkan::get_queue_families(physical_device, surface);
    }

}}