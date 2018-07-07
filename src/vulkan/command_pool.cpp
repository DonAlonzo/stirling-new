#include "command_pool.hpp"
#include "vulkan.hpp"

namespace stirling { namespace vulkan {

    inline Deleter<VkCommandPool> create_command_pool(
        const CommandPoolCreateInfo& create_info,
        VkDevice                     device) {
        
        const VkCommandPoolCreateInfo vk_create_info {
            .sType            = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
            .flags            = create_info.flags,
            .queueFamilyIndex = create_info.queue_family_index
        };

        return create<VkCommandPool>(
            vkCreateCommandPool,
            vkDestroyCommandPool,
            device,
            "Failed to create command pool.",
            &vk_create_info
        );
    }

    CommandPool::CommandPool(
        const CommandPoolCreateInfo& create_info,
        VkDevice                     device) :

        command_pool (create_command_pool(create_info, device)),
        device       (device) {
    }

    std::vector<CommandBuffer> CommandPool::allocate_command_buffers(const CommandBufferAllocateInfo& allocate_info) const {
        const VkCommandBufferAllocateInfo vk_allocate_info {
            .sType              = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
            .commandPool        = command_pool,
            .level              = allocate_info.level,
            .commandBufferCount = allocate_info.command_buffer_count
        };

        std::vector<VkCommandBuffer> vk_command_buffers{allocate_info.command_buffer_count};
        vulkan_assert(
            vkAllocateCommandBuffers(device, &vk_allocate_info, vk_command_buffers.data()),
            "Failed to allocate command buffers."
        );

        std::vector<CommandBuffer> command_buffers;
        command_buffers.reserve(allocate_info.command_buffer_count);
        std::transform(
            vk_command_buffers.begin(),
            vk_command_buffers.end(),
            std::back_inserter(command_buffers),
            [this](VkCommandBuffer command_buffer) -> Deleter<VkCommandBuffer> {
                return {
                    std::bind(vulkan::free_command_buffer, device, command_pool, std::placeholders::_1),
                    command_buffer
                };
            }
        );
        return command_buffers;
    }

}}