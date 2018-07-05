#include "device.hpp"
#include "vulkan.hpp"

namespace stirling { namespace vulkan {

    inline Deleter<VkDevice> create_device(
        const DeviceCreateInfo& create_info,
        VkPhysicalDevice        physical_device) {
        
        return create<VkDevice>(
            vkCreateDevice,
            vkDestroyDevice,
            "Failed to create device.",
            physical_device,
            create_info
        );
    }

    Device::Device(
        const DeviceCreateInfo& create_info,
        VkPhysicalDevice        physical_device) :

        device (create_device(create_info, physical_device)) {
    }

    DeviceMemory Device::allocate_memory(const MemoryAllocateInfo& allocate_info) const {
        return {allocate_info, device};
    }

}}