#include "device_memory.hpp"
#include "vulkan.hpp"

namespace stirling { namespace vulkan {

    DeviceMemory::DeviceMemory(
        const MemoryAllocateInfo& allocate_info,
        VkDevice                  device) :

        memory (allocate_memory(allocate_info, device)) {
    }

}}