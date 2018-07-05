#include "device.hpp"
#include "vulkan.hpp"

namespace stirling { namespace vulkan {

    Device::Device(
        const DeviceCreateInfo& create_info,
        VkPhysicalDevice        physical_device) :

        device (create_device(create_info, physical_device)) {
    }

}}