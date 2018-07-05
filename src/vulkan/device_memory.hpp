#pragma once

#include "deleter.hpp"
#include "vulkan_structs.hpp"

#include <vulkan/vulkan.h>

namespace stirling { namespace vulkan {

    struct MemoryAllocateInfoData {
        VkDeviceSize allocation_size;
        uint32_t     memory_type_index;

        inline operator const VkMemoryAllocateInfo() const {
            return {
                .sType           = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
                .allocationSize  = allocation_size,
                .memoryTypeIndex = memory_type_index
            };
        }
    };
    typedef Wrapper<MemoryAllocateInfoData, VkMemoryAllocateInfo> MemoryAllocateInfo;

    struct DeviceMemory {
        DeviceMemory(
            const MemoryAllocateInfo& allocate_info,
            VkDevice                  device);

        inline operator const VkDeviceMemory() const { return memory; }

    private:
        Deleter<VkDeviceMemory> memory;
    };

}}