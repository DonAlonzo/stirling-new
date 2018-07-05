#pragma once

#include "buffer.hpp"
#include "command_buffer.hpp"
#include "command_pool.hpp"
#include "deleter.hpp"
#include "device_memory.hpp"
#include "vulkan_structs.hpp"

#include <vulkan/vulkan.h>

namespace stirling { namespace vulkan {

    struct DeviceCreateInfoData {
        std::vector<DeviceQueueCreateInfo> queues;
        std::vector<const char*>           enabled_extensions;
        VkPhysicalDeviceFeatures           enabled_features;

        inline operator const VkDeviceCreateInfo() const {
            return {
                .sType                   = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
                .queueCreateInfoCount    = static_cast<uint32_t>(queues.size()),
                .pQueueCreateInfos       = cast_vector<const VkDeviceQueueCreateInfo*>(queues),
                .enabledExtensionCount   = static_cast<uint32_t>(enabled_extensions.size()),
                .ppEnabledExtensionNames = enabled_extensions.data(),
                .pEnabledFeatures        = &enabled_features
            };
        }
    };
    typedef Wrapper<DeviceCreateInfoData, VkDeviceCreateInfo> DeviceCreateInfo;

    struct Device {
        Device(
            const DeviceCreateInfo& create_info,
            VkPhysicalDevice        physical_device);

        inline operator const VkDevice() const { return device; }
        
        VkMemoryRequirements get_buffer_memory_requirements(VkBuffer buffer) const;
        DeviceMemory allocate_memory(const MemoryAllocateInfo& allocate_info) const;
        Buffer create_buffer(const BufferCreateInfo& create_info) const;
        CommandPool create_command_pool(const CommandPoolCreateInfo& create_info) const;

    private:
        Deleter<VkDevice> device;
    };

}}