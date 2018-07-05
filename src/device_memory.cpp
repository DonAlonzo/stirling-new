#include "device_memory.hpp"
#include "vulkan.hpp"

namespace stirling { namespace vulkan {

    DeviceMemory::DeviceMemory(
        VkDevice              device,
        VkBuffer              buffer,
        VkMemoryPropertyFlags flags) {
        
        // const auto requirements = get_buffer_memory_requirements(device, buffer);

        // memory (allocate_memory({{
        //     .allocation_size   = requirements.size,
        //     .memory_type_index = find_memory_type(
        //         physical_device,
        //         memory_requirements.memoryTypeBits,
        //         VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
        //     )
        // }}, device)) {

        // // Bind memory to buffer
        // vkBindBufferMemory(device, buffer, memory, 0);
    }

}}