#pragma once

#include "buffer.hpp"
#include "deleter.hpp"
#include "device_memory.hpp"
#include "vulkan_structs.hpp"

#include <vulkan/vulkan.h>

#include <vector>

namespace stirling { namespace vulkan {

    struct CommandBuffer {
        CommandBuffer(VkCommandBuffer command_buffer);

        inline operator const VkCommandBuffer() const { return command_buffer; }

        void begin(const CommandBufferBeginInfo& begin_info) const;
        void end() const;

        void copy_buffer(
            VkBuffer                         src_buffer,
            VkBuffer                         dst_buffer,
            const std::vector<VkBufferCopy>& regions) const;

    private:
        VkCommandBuffer command_buffer;
    };

}}