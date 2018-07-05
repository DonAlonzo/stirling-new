#include "device.hpp"
#include "vulkan.hpp"

namespace stirling { namespace vulkan {

    CommandBuffer::CommandBuffer(VkCommandBuffer command_buffer) :
        command_buffer (command_buffer) {
    } 

    void CommandBuffer::begin(const CommandBufferBeginInfo& begin_info) const {
        vulkan::begin_command_buffer(begin_info, command_buffer);
    }

    void CommandBuffer::end() const {
        vulkan::end_command_buffer(command_buffer);
    }

    void CommandBuffer::copy_buffer(
        VkBuffer                         src_buffer,
        VkBuffer                         dst_buffer,
        const std::vector<VkBufferCopy>& regions) const {

        vulkan::cmd_copy_buffer(command_buffer, src_buffer, dst_buffer, regions);
    }

}}