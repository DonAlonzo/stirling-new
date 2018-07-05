#pragma once

#include "deleter.hpp"
#include "vulkan_structs.hpp"

#include <vulkan/vulkan.h>

namespace stirling { namespace vulkan {

    struct BufferCreateInfoData {
        VkDeviceSize          size;
        VkBufferUsageFlags    usage;
        VkSharingMode         sharing_mode;
        std::vector<uint32_t> queue_family_indices;

        inline operator const VkBufferCreateInfo() const {
            return {
                .sType                 = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
                .size                  = size,
                .usage                 = usage,
                .sharingMode           = sharing_mode,
                .queueFamilyIndexCount = static_cast<uint32_t>(queue_family_indices.size()),
                .pQueueFamilyIndices   = queue_family_indices.data()
            };
        }
    };
    typedef Wrapper<BufferCreateInfoData, VkBufferCreateInfo> BufferCreateInfo;

    struct Buffer {
        Buffer(
            const BufferCreateInfo& create_info,
            VkDevice                device);

        inline operator const VkBuffer() const { return buffer; }

    private:
        Deleter<VkBuffer> buffer;
    };

}}