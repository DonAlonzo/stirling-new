#include "command_buffer.hpp"
#include "vulkan.hpp"

namespace stirling { namespace vulkan {

    CommandBuffer::CommandBuffer(Deleter<VkCommandBuffer>&& command_buffer) :
        command_buffer (command_buffer) {
    } 

    const CommandBuffer& CommandBuffer::begin(const CommandBufferBeginInfo& begin_info) const {
        vulkan::begin_command_buffer(begin_info, command_buffer);
        return *this;
    }

    const CommandBuffer& CommandBuffer::end() const {
        vulkan::end_command_buffer(command_buffer);
        return *this;
    }

    const CommandBuffer& CommandBuffer::begin_render_pass(
        const RenderPassBeginInfo& begin_info,
        VkSubpassContents          contents) const {
        
        vulkan::begin_render_pass(begin_info, command_buffer, contents);
        return *this;
    }

    const CommandBuffer& CommandBuffer::end_render_pass() const {
        vkCmdEndRenderPass(command_buffer);
        return *this;
    }

    const CommandBuffer& CommandBuffer::copy_buffer(
        VkBuffer                         src_buffer,
        VkBuffer                         dst_buffer,
        const std::vector<VkBufferCopy>& regions) const {

        vulkan::cmd_copy_buffer(command_buffer, src_buffer, dst_buffer, regions);
        return *this;
    }

    const CommandBuffer& CommandBuffer::draw_indexed(
        uint32_t index_count,
        uint32_t instance_count,
        uint32_t first_index,
        int32_t  vertex_offset,
        uint32_t first_instance) const {

        vkCmdDrawIndexed(
            command_buffer,
            index_count,
            instance_count,
            first_index,
            vertex_offset,
            first_instance
        );
        return *this;
    }

    const CommandBuffer& CommandBuffer::bind_pipeline(
        VkPipelineBindPoint pipeline_bind_point,
        VkPipeline          pipeline) const {

        vkCmdBindPipeline(command_buffer, pipeline_bind_point, pipeline);
        return *this;
    }

    const CommandBuffer& CommandBuffer::bind_vertex_buffers(
        uint32_t                         first_binding,
        const std::vector<VkBuffer>&     buffers,
        const std::vector<VkDeviceSize>& offsets) const {

        vulkan::cmd_bind_vertex_buffers(command_buffer, first_binding, buffers, offsets);
        return *this;
    }

    const CommandBuffer& CommandBuffer::bind_index_buffer(
        VkBuffer     buffer,
        VkDeviceSize offset,
        VkIndexType  index_type) const {

        vulkan::cmd_bind_index_buffer(command_buffer, buffer, offset, index_type);
        return *this;
    }

    const CommandBuffer& CommandBuffer::bind_descriptor_sets(
        VkPipelineBindPoint          pipeline_bind_point,
        VkPipelineLayout             layout,
        uint32_t                     first_set,
        std::vector<VkDescriptorSet> descriptor_sets,
        std::vector<uint32_t>        dynamic_offsets) const {

        vulkan::cmd_bind_descriptor_sets(
            command_buffer,
            pipeline_bind_point,
            layout,
            first_set,
            descriptor_sets,
            dynamic_offsets
        );
        return *this;
    }
    
}}