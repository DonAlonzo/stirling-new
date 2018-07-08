#pragma once

#include "deleter.hpp"
#include "vulkan.hpp"
#include "vulkan_structs.hpp"

#include <vulkan/vulkan.h>

namespace stirling { namespace vulkan {

    struct BufferCreateInfo {
        VkDeviceSize          size;
        VkBufferUsageFlags    usage;
        VkSharingMode         sharing_mode;
        std::vector<uint32_t> queue_family_indices;
    };

    struct Buffer {
        Buffer(
            const BufferCreateInfo& create_info,
            VkDevice                device);

        inline operator const VkBuffer() const { return buffer; }

        void bind(VkDeviceMemory memory, VkDeviceSize offset) const;
        
        vulkan::MemoryRequirements get_memory_requirements() const;

    private:
        Deleter<VkBuffer> buffer;
        VkDevice device;
    };

}}