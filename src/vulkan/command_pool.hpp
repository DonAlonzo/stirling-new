#pragma once

#include "command_buffer.hpp"
#include "deleter.hpp"
#include "vulkan_structs.hpp"

#include <vulkan/vulkan.h>

namespace stirling { namespace vulkan {

    struct CommandPoolCreateInfo {
        VkCommandPoolCreateFlags flags;
        uint32_t                 queue_family_index;
    };

    struct CommandBufferAllocateInfo {
        VkCommandBufferLevel level;
        uint32_t             command_buffer_count;
    };

    struct CommandPool {
        CommandPool(
            const CommandPoolCreateInfo& create_info,
            VkDevice                     device);

        inline operator const VkCommandPool() const { return command_pool; }

        std::vector<CommandBuffer> allocate_command_buffers(
            const CommandBufferAllocateInfo& allocate_info) const;

    private:
        Deleter<VkCommandPool> command_pool;
        VkDevice               device;
    };

}}