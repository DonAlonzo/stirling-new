#pragma once

#include "deleter.hpp"
#include "vulkan_structs.hpp"

#include <vulkan/vulkan.h>

namespace stirling { namespace vulkan {

    struct SpecializationInfo {
        std::vector<VkSpecializationMapEntry> map_entries;
        std::vector<void*>                    data;
    };

    struct PipelineShaderStageCreateInfo {
        VkShaderStageFlagBits   stage;
        Deleter<VkShaderModule> module;
        const char*             name;
        SpecializationInfo      specialization_info;
    };

    struct PipelineVertexInputStateCreateInfo {
        std::vector<VkVertexInputBindingDescription>   vertex_binding_descriptions;
        std::vector<VkVertexInputAttributeDescription> vertex_attribute_descriptions;
    };

    struct PipelineInputAssemblyStateCreateInfo {
        VkPrimitiveTopology topology;
        VkBool32            primitive_restart_enable;
    };

    struct PipelineTessellationStateCreateInfo {
        uint32_t patch_control_points;
    };

    struct PipelineViewportStateCreateInfo {
        std::vector<VkViewport> viewports;
        std::vector<VkRect2D>   scissors;
    };

    struct PipelineRasterizationStateCreateInfo {
        VkBool32        depth_clamp_enable;
        VkBool32        rasterizer_discard_enable;
        VkPolygonMode   polygon_mode;
        VkCullModeFlags cull_mode;
        VkFrontFace     front_face;
        VkBool32        depth_bias_enable;
        float           depth_bias_constant_factor;
        float           depth_bias_clamp;
        float           depth_bias_slope_factor;
        float           line_width;
    };

    struct PipelineMultisampleStateCreateInfo {
        VkSampleCountFlagBits rasterization_samples;
        VkBool32              sample_shading_enable;
        float                 min_sample_shading;
        VkSampleMask          sample_mask = -1u;
        VkBool32              alpha_to_coverage_enable;
        VkBool32              alpha_to_one_enable;
    };

    struct PipelineDepthStencilStateCreateInfo {
        VkBool32         depth_test_enable;
        VkBool32         depth_write_enable;
        VkCompareOp      depth_compare_op;
        VkBool32         depth_bounds_test_enable;
        VkBool32         stencil_test_enable;
        VkStencilOpState front;
        VkStencilOpState back;
        float            min_depth_bounds;
        float            max_depth_bounds;
    };

    struct PipelineColorBlendStateCreateInfo {
        VkBool32                                         logic_op_enable;
        VkLogicOp                                        logic_op;
        std::vector<VkPipelineColorBlendAttachmentState> attachments;
        std::array<float, 4>                             blend_constants;
    };

    struct PipelineDynamicStateCreateInfo {
        std::vector<VkDynamicState> dynamic_states;
    };

    struct GraphicsPipelineCreateInfo {
        std::vector<PipelineShaderStageCreateInfo> stages;
        PipelineVertexInputStateCreateInfo         vertex_input_state;
        PipelineInputAssemblyStateCreateInfo       input_assembly_state;
        PipelineTessellationStateCreateInfo        tessellation_state;
        PipelineViewportStateCreateInfo            viewport_state;
        PipelineRasterizationStateCreateInfo       rasterization_state;
        PipelineMultisampleStateCreateInfo         multisample_state;
        PipelineDepthStencilStateCreateInfo        depth_stencil_state;
        PipelineColorBlendStateCreateInfo          color_blend_state;
        PipelineDynamicStateCreateInfo             dynamic_state;
        VkPipelineLayout                           layout;
        VkRenderPass                               render_pass;
        uint32_t                                   subpass;
        VkPipeline                                 base_pipeline_handle;
        int32_t                                    base_pipeline_index;
    };

    struct Pipeline {

        Pipeline(
            const GraphicsPipelineCreateInfo& create_info,
            VkPipelineCache                   pipeline_cache,
            VkDevice                          device);

        inline operator const VkPipeline() const { return pipeline; }

    private:
        Deleter<VkPipeline> pipeline;
    };

}}