#include "buffer.hpp"
#include "vulkan.hpp"
#include "vulkan_create.hpp"

#include <memory>

namespace stirling { namespace vulkan {

    inline Deleter<VkBuffer> create_buffer(
        const BufferCreateInfo& create_info,
        VkDevice                device) {

        const VkBufferCreateInfo vk_create_info {
            .sType                 = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
            .size                  = create_info.size,
            .usage                 = create_info.usage,
            .sharingMode           = create_info.sharing_mode,
            .queueFamilyIndexCount = static_cast<uint32_t>(create_info.queue_family_indices.size()),
            .pQueueFamilyIndices   = create_info.queue_family_indices.data()
        };

        return create<VkBuffer>(
            vkCreateBuffer,
            vkDestroyBuffer,
            device,
            "Failed to create buffer.",
            &vk_create_info
        );
    }

    Buffer::Buffer(
        const BufferCreateInfo& create_info,
        VkDevice                device) :

        buffer (create_buffer(create_info, device)),
        device (device) {
    };

    void Buffer::bind(VkDeviceMemory memory, VkDeviceSize offset) const {
        vkBindBufferMemory(device, buffer, memory, offset);
    }

    MemoryRequirements Buffer::get_memory_requirements() const {
        return vulkan::get_buffer_memory_requirements(device, buffer);
    }

}}