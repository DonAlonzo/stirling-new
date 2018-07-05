#include "device.hpp"
#include "vulkan.hpp"

namespace stirling { namespace vulkan {

    Device::Device(
        const MemoryAllocateInfo& allocate_info,
        VkDevice                  device) :

        memory (allocate_memory(allocate_info, device)) {
    }

}}