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

        void begin(const CommandBufferBeginInfo& begin_info) const;
        void end() const;

        void begin_render_pass(
            const RenderPassBeginInfo& begin_info,
            VkSubpassContents          contents) const;
        void end_render_pass() const;

        void copy_buffer(
            VkBuffer                         src_buffer,
            VkBuffer                         dst_buffer,
            const std::vector<VkBufferCopy>& regions) const;

        void draw_indexed(
            uint32_t index_count,
            uint32_t instance_count,
            uint32_t first_index,
            int32_t  vertex_offset,
            uint32_t first_instance) const;

        void bind_pipeline(
            VkPipelineBindPoint pipeline_bind_point,
            VkPipeline          pipeline) const;

        void bind_vertex_buffers(
            uint32_t                         first_binding,
            const std::vector<VkBuffer>&     buffers,
            const std::vector<VkDeviceSize>& offsets) const;

        void bind_index_buffer(
            VkBuffer     buffer,
            VkDeviceSize offset,
            VkIndexType  index_type) const;

        void bind_descriptor_sets(
            VkPipelineBindPoint          pipeline_bind_point,
            VkPipelineLayout             layout,
            uint32_t                     first_set,
            std::vector<VkDescriptorSet> descriptor_sets,
            std::vector<uint32_t>        dynamic_offsets) const;

    private:
        Deleter<VkCommandBuffer> command_buffer;
    };

}}