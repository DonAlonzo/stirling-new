#include "device.hpp"
#include "vulkan.hpp"

namespace stirling { namespace vulkan {

    inline Deleter<VkDevice> create_device(
        const DeviceCreateInfo& create_info,
        VkPhysicalDevice        physical_device) {
        
        const VkDeviceCreateInfo vk_create_info {
            .sType                   = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
            .queueCreateInfoCount    = static_cast<uint32_t>(create_info.queues.size()),
            .pQueueCreateInfos       = cast_vector<const VkDeviceQueueCreateInfo*>(create_info.queues),
            .enabledExtensionCount   = static_cast<uint32_t>(create_info.enabled_extensions.size()),
            .ppEnabledExtensionNames = create_info.enabled_extensions.data(),
            .pEnabledFeatures        = &create_info.enabled_features
        };

        return create<VkDevice>(
            vkCreateDevice,
            vkDestroyDevice,
            "Failed to create device.",
            physical_device,
            &vk_create_info
        );
    }

    Device::Device(
        const DeviceCreateInfo& create_info,
        VkPhysicalDevice        physical_device) :

        device (create_device(create_info, physical_device)) {
    }

    VkMemoryRequirements Device::get_buffer_memory_requirements(VkBuffer buffer) const {
        return vulkan::get_buffer_memory_requirements(device, buffer);
    }

    DeviceMemory Device::allocate_memory(const MemoryAllocateInfo& allocate_info) const {
        return {allocate_info, device};
    }

    Buffer Device::create_buffer(const BufferCreateInfo& create_info) const {
        return {create_info, device};
    }

    CommandPool Device::create_command_pool(const CommandPoolCreateInfo& create_info) const {
        return {create_info, device};
    }

}}