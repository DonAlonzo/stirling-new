#include "buffer.hpp"
#include "vulkan.hpp"
#include "vulkan_create.hpp"

#include <memory>

namespace stirling { namespace vulkan {

    inline Deleter<VkBuffer> create_buffer(
        const BufferCreateInfo& create_info,
        VkDevice                device) {

        return create<VkBuffer>(
            vkCreateBuffer,
            vkDestroyBuffer,
            device,
            "Failed to create buffer.",
            create_info
        );
    }

    Buffer::Buffer(
        const BufferCreateInfo& create_info,
        VkDevice                device) :

        buffer (create_buffer(create_info, device)) {
    };

}}