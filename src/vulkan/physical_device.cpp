#include "physical_device.hpp"
#include "vulkan.hpp"

namespace stirling { namespace vulkan {

    PhysicalDevice::PhysicalDevice(VkPhysicalDevice physical_device) :
        physical_device (physical_device) {
    }

}}