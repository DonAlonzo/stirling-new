#pragma once

#include "buffer.hpp"
#include "deleter.hpp"
#include "device_memory.hpp"
#include "vulkan_structs.hpp"

#include <vulkan/vulkan.h>

#include <vector>

namespace stirling { namespace vulkan {

    struct CommandBuffer {
        CommandBuffer(Deleter<VkCommandBuffer>&& command_buffer);

        inline operator const VkCommandBuffer() const { return command_buffer; }

        const CommandBuffer& begin(const CommandBufferBeginInfo& begin_info) const;
        const CommandBuffer& end() const;

        const CommandBuffer& begin_render_pass(
            const RenderPassBeginInfo& begin_info,
            VkSubpassContents          contents) const;
        const CommandBuffer& end_render_pass() const;

        const CommandBuffer& copy_buffer(
            VkBuffer                         src_buffer,
            VkBuffer                         dst_buffer,
            const std::vector<VkBufferCopy>& regions) const;

        const CommandBuffer& draw_indexed(
            uint32_t index_count,
            uint32_t instance_count,
            uint32_t first_index,
            int32_t  vertex_offset,
            uint32_t first_instance) const;

        const CommandBuffer& bind_pipeline(
            VkPipelineBindPoint pipeline_bind_point,
            VkPipeline          pipeline) const;

        const CommandBuffer& bind_vertex_buffers(
            uint32_t                         first_binding,
            const std::vector<VkBuffer>&     buffers,
            const std::vector<VkDeviceSize>& offsets) const;

        const CommandBuffer& bind_index_buffer(
            VkBuffer     buffer,
            VkDeviceSize offset,
            VkIndexType  index_type) const;

        const CommandBuffer& bind_descriptor_sets(
            VkPipelineBindPoint          pipeline_bind_point,
            VkPipelineLayout             layout,
            uint32_t                     first_set,
            std::vector<VkDescriptorSet> descriptor_sets,
            std::vector<uint32_t>        dynamic_offsets) const;

    private:
        Deleter<VkCommandBuffer> command_buffer;
    };

}}