#include "device_memory.hpp"
#include "vulkan.hpp"

namespace stirling { namespace vulkan {

    inline Deleter<VkDeviceMemory> allocate_memory(
        const MemoryAllocateInfo& allocate_info,
        VkDevice                  device) {

        return create<VkDeviceMemory>(
            vkAllocateMemory,
            vkFreeMemory,
            device,
            "Failed to allocate memory.",
            allocate_info
        );
    }

    DeviceMemory::DeviceMemory(
        const MemoryAllocateInfo& allocate_info,
        VkDevice                  device) :

        memory (allocate_memory(allocate_info, device)) {
    }

}}