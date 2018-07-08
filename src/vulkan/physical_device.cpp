#include "physical_device.hpp"
#include "vulkan.hpp"

namespace stirling { namespace vulkan {

    PhysicalDevice::PhysicalDevice(VkPhysicalDevice physical_device) :
        physical_device (physical_device) {
    }

    Device PhysicalDevice::create_device(const DeviceCreateInfo& create_info) const {
        return {create_info, physical_device};
    }

    VkPhysicalDeviceProperties PhysicalDevice::get_properties() const {
        return vulkan::get_physical_device_properties(physical_device);
    }

    VkPhysicalDeviceFeatures PhysicalDevice::get_features() const {
        return vulkan::get_physical_device_features(physical_device);
    }

    QueueFamilyIndices PhysicalDevice::get_queue_families(const Surface& surface) const {
        return vulkan::get_queue_families(physical_device, surface);
    }

}}