#pragma once

#include "deleter.hpp"
#include "vulkan_structs.hpp"

#include <vulkan/vulkan.h>

#include <functional>

namespace stirling { namespace vulkan {

    struct MemoryAllocateInfo {
        VkDeviceSize allocation_size;
        uint32_t     memory_type_index;
    };

    struct DeviceMemoryMapping {
        DeviceMemoryMapping(
            VkDevice       device,
            VkDeviceMemory memory,
            VkDeviceSize   offset,
            VkDeviceSize   size);

        ~DeviceMemoryMapping();

        DeviceMemoryMapping(DeviceMemoryMapping&&) = default;
        DeviceMemoryMapping& operator=(DeviceMemoryMapping&&) = default;

        void copy(const void* src, size_t size);

    private:
        void*                 data;
        std::function<void()> deleter;
    };

    struct DeviceMemory {
        DeviceMemory(
            const MemoryAllocateInfo& allocate_info,
            VkDevice                  device);

        inline operator const VkDeviceMemory() const { return memory; }

        DeviceMemoryMapping map(VkDeviceSize offset = 0, VkDeviceSize size = VK_WHOLE_SIZE) const;

    private:
        Deleter<VkDeviceMemory> memory;
        VkDevice                device;
    };

}}