#include "pipeline.hpp"
#include "vulkan.hpp"

namespace stirling { namespace vulkan {

    inline Deleter<VkPipeline> create_pipeline(
        const GraphicsPipelineCreateInfo& create_info,
        VkPipelineCache                   pipeline_cache,
        VkDevice                          device) {

        std::vector<VkSpecializationInfo> specialization_infos;
        specialization_infos.reserve(create_info.stages.size());
        const auto stages = cast_vector<VkPipelineShaderStageCreateInfo>(
            create_info.stages,
            [&specialization_infos](const PipelineShaderStageCreateInfo& create_info) {
                return VkPipelineShaderStageCreateInfo {
                    .sType               = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
                    .stage               = create_info.stage,
                    .module              = create_info.module,
                    .pName               = create_info.name,
                    .pSpecializationInfo = &specialization_infos.emplace_back(VkSpecializationInfo {
                        .mapEntryCount = static_cast<uint32_t>(create_info.specialization_info.map_entries.size()),
                        .pMapEntries   = create_info.specialization_info.map_entries.data(),
                        .dataSize      = static_cast<uint32_t>(create_info.specialization_info.data.size()),
                        .pData         = create_info.specialization_info.data.data()
                    })
                };
            }
        );

        const VkPipelineVertexInputStateCreateInfo vertex_input_state {
            .sType                           = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
            .vertexBindingDescriptionCount   = static_cast<uint32_t>(create_info.vertex_input_state.vertex_binding_descriptions.size()),
            .pVertexBindingDescriptions      = create_info.vertex_input_state.vertex_binding_descriptions.data(),
            .vertexAttributeDescriptionCount = static_cast<uint32_t>(create_info.vertex_input_state.vertex_attribute_descriptions.size()),
            .pVertexAttributeDescriptions    = create_info.vertex_input_state.vertex_attribute_descriptions.data()
        };

        const VkPipelineInputAssemblyStateCreateInfo input_assembly_state {
            .sType                  = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
            .topology               = create_info.input_assembly_state.topology,
            .primitiveRestartEnable = create_info.input_assembly_state.primitive_restart_enable
        };

        const VkPipelineTessellationStateCreateInfo tessellation_state {
            .sType              = VK_STRUCTURE_TYPE_PIPELINE_TESSELLATION_STATE_CREATE_INFO,
            .patchControlPoints = create_info.tessellation_state.patch_control_points
        };

        const VkPipelineViewportStateCreateInfo viewport_state {
            .sType         = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
            .viewportCount = static_cast<uint32_t>(create_info.viewport_state.viewports.size()),
            .pViewports    = create_info.viewport_state.viewports.data(),
            .scissorCount  = static_cast<uint32_t>(create_info.viewport_state.scissors.size()),
            .pScissors     = create_info.viewport_state.scissors.data()
        };

        const VkPipelineRasterizationStateCreateInfo rasterization_state {
            .sType                   = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
            .depthClampEnable        = create_info.rasterization_state.depth_clamp_enable,
            .rasterizerDiscardEnable = create_info.rasterization_state.rasterizer_discard_enable,
            .polygonMode             = create_info.rasterization_state.polygon_mode,
            .cullMode                = create_info.rasterization_state.cull_mode,
            .frontFace               = create_info.rasterization_state.front_face,
            .depthBiasEnable         = create_info.rasterization_state.depth_bias_enable,
            .depthBiasConstantFactor = create_info.rasterization_state.depth_bias_constant_factor,
            .depthBiasClamp          = create_info.rasterization_state.depth_bias_clamp,
            .depthBiasSlopeFactor    = create_info.rasterization_state.depth_bias_slope_factor,
            .lineWidth               = create_info.rasterization_state.line_width
        };

        const VkPipelineMultisampleStateCreateInfo multisample_state {
            .sType                 = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
            .rasterizationSamples  = create_info.multisample_state.rasterization_samples,
            .sampleShadingEnable   = create_info.multisample_state.sample_shading_enable,
            .minSampleShading      = create_info.multisample_state.min_sample_shading,
            .pSampleMask           = &create_info.multisample_state.sample_mask,
            .alphaToCoverageEnable = create_info.multisample_state.alpha_to_coverage_enable,
            .alphaToOneEnable      = create_info.multisample_state.alpha_to_one_enable
        };

        const VkPipelineDepthStencilStateCreateInfo depth_stencil_state {
            .sType                 = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO,
            .depthTestEnable       = create_info.depth_stencil_state.depth_test_enable,
            .depthWriteEnable      = create_info.depth_stencil_state.depth_write_enable,
            .depthCompareOp        = create_info.depth_stencil_state.depth_compare_op,
            .depthBoundsTestEnable = create_info.depth_stencil_state.depth_bounds_test_enable,
            .stencilTestEnable     = create_info.depth_stencil_state.stencil_test_enable,
            .front                 = create_info.depth_stencil_state.front,
            .back                  = create_info.depth_stencil_state.back,
            .minDepthBounds        = create_info.depth_stencil_state.min_depth_bounds,
            .maxDepthBounds        = create_info.depth_stencil_state.max_depth_bounds
        };

        VkPipelineColorBlendStateCreateInfo color_blend_state {
            .sType           = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
            .logicOpEnable   = create_info.color_blend_state.logic_op_enable,
            .logicOp         = create_info.color_blend_state.logic_op,
            .attachmentCount = static_cast<uint32_t>(create_info.color_blend_state.attachments.size()),
            .pAttachments    = create_info.color_blend_state.attachments.data()
        };
        std::copy(
            create_info.color_blend_state.blend_constants.begin(), 
            create_info.color_blend_state.blend_constants.end(),
            color_blend_state.blendConstants
        );

        const VkPipelineDynamicStateCreateInfo dynamic_state {
            .sType             = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
            .dynamicStateCount = static_cast<uint32_t>(create_info.dynamic_state.dynamic_states.size()),
            .pDynamicStates    = create_info.dynamic_state.dynamic_states.data(),
        };

        const VkGraphicsPipelineCreateInfo vk_create_info {
            .sType               = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
            .stageCount          = static_cast<uint32_t>(stages.size()),
            .pStages             = stages.data(),
            .pVertexInputState   = &vertex_input_state,
            .pInputAssemblyState = &input_assembly_state,
            .pTessellationState  = &tessellation_state,
            .pViewportState      = &viewport_state,
            .pRasterizationState = &rasterization_state,
            .pMultisampleState   = &multisample_state,
            .pDepthStencilState  = &depth_stencil_state,
            .pColorBlendState    = &color_blend_state,
            .pDynamicState       = create_info.dynamic_state.dynamic_states.size() > 0 ? &dynamic_state : nullptr,
            .layout              = create_info.layout,
            .renderPass          = create_info.render_pass,
            .subpass             = create_info.subpass,
            .basePipelineHandle  = create_info.base_pipeline_handle,
            .basePipelineIndex   = create_info.base_pipeline_index
        };

        return create<VkPipeline>(
            vkCreateGraphicsPipelines,
            vkDestroyPipeline,
            device,
            "Failed to create pipeline.",
            pipeline_cache,
            1,
            &vk_create_info
        );
    }

    Pipeline::Pipeline(
        const GraphicsPipelineCreateInfo& create_info,
        VkPipelineCache                   pipeline_cache,
        VkDevice                          device) :

        pipeline (create_pipeline(create_info, pipeline_cache, device)) {
    }

}}