#pragma once

#include "buffer.hpp"
#include "command_buffer.hpp"
#include "command_pool.hpp"
#include "deleter.hpp"
#include "device_memory.hpp"
#include "vulkan_structs.hpp"
#include "queue.hpp"

#include <vulkan/vulkan.h>

namespace stirling { namespace vulkan {

    struct DeviceCreateInfo {
        std::vector<DeviceQueueCreateInfo> queues;
        std::vector<const char*>           enabled_extensions;
        VkPhysicalDeviceFeatures           enabled_features;
    };

    struct Device {
        Device(
            const DeviceCreateInfo& create_info,
            VkPhysicalDevice        physical_device);

        inline operator const VkDevice() const { return device; }
        
        VkMemoryRequirements get_buffer_memory_requirements(VkBuffer buffer) const;
        DeviceMemory allocate_memory(const MemoryAllocateInfo& allocate_info) const;
        Buffer create_buffer(const BufferCreateInfo& create_info) const;
        CommandPool create_command_pool(const CommandPoolCreateInfo& create_info) const;
        Queue get_queue(uint32_t queue_family, uint32_t queue_index) const;

    private:
        Deleter<VkDevice> device;
    };

}}