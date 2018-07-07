#pragma once

#include "deleter.hpp"
#include "vulkan_helpers.hpp"

#include <vulkan/vulkan.h>

#include <cassert>
#include <vector>

namespace stirling { namespace vulkan {

    struct QueueFamilyIndices {
        uint32_t graphics_queue = -1;
        uint32_t present_queue = -1;
    };

    template<typename From, typename To>
    struct Wrapper {
        Wrapper() : to (From{}) {}
        Wrapper(From&& from) : to (from) {}

        inline operator const To*() const { return &to; }
        inline const To& data() const { return to; }

    private:
        To to;
    };

    struct ApplicationInfoData {
        const char* application_name;
        uint32_t    application_version;
        const char* engine_name;
        uint32_t    engine_version;
        uint32_t    api_version;

        inline operator const VkApplicationInfo() const {
            return {
                .sType              = VK_STRUCTURE_TYPE_APPLICATION_INFO,
                .pApplicationName   = application_name,
                .applicationVersion = application_version,
                .pEngineName        = engine_name,
                .engineVersion      = engine_version,
                .apiVersion         = api_version
            };
        };
    };
    typedef Wrapper<ApplicationInfoData, VkApplicationInfo> ApplicationInfo;

    struct DebugReportCallbackCreateInfoEXTData {
        VkDebugReportFlagsEXT        flags;
        PFN_vkDebugReportCallbackEXT callback;
        void*                        user_data;

        inline operator const VkDebugReportCallbackCreateInfoEXT() const {
            return {
                .sType       = VK_STRUCTURE_TYPE_DEBUG_REPORT_CALLBACK_CREATE_INFO_EXT,
                .flags       = flags,
                .pfnCallback = callback,
                .pUserData   = user_data
            };
        }
    };
    typedef Wrapper<DebugReportCallbackCreateInfoEXTData, VkDebugReportCallbackCreateInfoEXT> DebugReportCallbackCreateInfoEXT;

    struct DeviceQueueCreateInfoData {
        uint32_t           queue_family_index;
        std::vector<float> queue_priorities;

        inline operator const VkDeviceQueueCreateInfo() const {
            return {
                .sType            = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
                .queueFamilyIndex = queue_family_index,
                .queueCount       = static_cast<uint32_t>(queue_priorities.size()),
                .pQueuePriorities = queue_priorities.data()
            };
        }
    }; 
    typedef Wrapper<DeviceQueueCreateInfoData, VkDeviceQueueCreateInfo> DeviceQueueCreateInfo; 

    struct SwapchainCreateInfoKHRData {
        VkSurfaceKHR                  surface;
        uint32_t                      min_image_count;
        VkFormat                      image_format;
        VkColorSpaceKHR               image_color_space;
        VkExtent2D                    image_extent;
        uint32_t                      image_array_layers;
        VkImageUsageFlags             image_usage;
        VkSharingMode                 image_sharing_mode;
        std::vector<uint32_t>         queue_family_indices;
        VkSurfaceTransformFlagBitsKHR pre_transform;
        VkCompositeAlphaFlagBitsKHR   composite_alpha;
        VkPresentModeKHR              present_mode;
        VkBool32                      clipped;
        VkSwapchainKHR                old_swapchain;

        inline operator const VkSwapchainCreateInfoKHR() const {
            return {
                .sType                 = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
                .surface               = surface,
                .minImageCount         = min_image_count,
                .imageFormat           = image_format,
                .imageColorSpace       = image_color_space,
                .imageExtent           = image_extent,
                .imageArrayLayers      = image_array_layers,
                .imageUsage            = image_usage,
                .imageSharingMode      = image_sharing_mode,
                .queueFamilyIndexCount = static_cast<uint32_t>(queue_family_indices.size()),
                .pQueueFamilyIndices   = queue_family_indices.data(),
                .preTransform          = pre_transform,
                .compositeAlpha        = composite_alpha,
                .presentMode           = present_mode,
                .clipped               = clipped,
                .oldSwapchain          = old_swapchain
            };
        }
    };
    typedef Wrapper<SwapchainCreateInfoKHRData, VkSwapchainCreateInfoKHR> SwapchainCreateInfoKHR;

    struct ImageViewCreateInfoData {
        VkImage                 image;
        VkImageViewType         view_type;
        VkFormat                format;
        VkComponentMapping      components;
        VkImageSubresourceRange subresource_range;

        inline operator const VkImageViewCreateInfo() const {
            return {
                .sType            = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
                .image            = image,
                .viewType         = view_type,
                .format           = format,
                .components       = components,
                .subresourceRange = subresource_range
            };
        }
    };
    typedef Wrapper<ImageViewCreateInfoData, VkImageViewCreateInfo> ImageViewCreateInfo;

    struct AttachmentDescriptionData {
        VkAttachmentDescriptionFlags flags;
        VkFormat                     format;
        VkSampleCountFlagBits        samples;
        VkAttachmentLoadOp           load_op;
        VkAttachmentStoreOp          store_op;
        VkAttachmentLoadOp           stencil_load_op;
        VkAttachmentStoreOp          stencil_store_op;
        VkImageLayout                initial_layout;
        VkImageLayout                final_layout;

        inline operator const VkAttachmentDescription() const {
            return {
                .flags          = flags,
                .format         = format,
                .samples        = samples,
                .loadOp         = load_op,
                .storeOp        = store_op,
                .stencilLoadOp  = stencil_load_op,
                .stencilStoreOp = stencil_store_op,
                .initialLayout  = initial_layout,
                .finalLayout    = final_layout
            };
        }
    };
    typedef Wrapper<AttachmentDescriptionData, VkAttachmentDescription> AttachmentDescription;

    struct SubpassDescriptionData {
        VkSubpassDescriptionFlags          flags;
        VkPipelineBindPoint                pipeline_bind_point;
        std::vector<VkAttachmentReference> input_attachments;
        std::vector<VkAttachmentReference> color_attachments;
        std::vector<VkAttachmentReference> resolve_attachments;
        VkAttachmentReference              depth_stencil_attachment = { VK_ATTACHMENT_UNUSED };
        std::vector<uint32_t>              preserve_attachments;

        inline operator const VkSubpassDescription() const {
            return {
                .flags                   = flags,
                .pipelineBindPoint       = pipeline_bind_point,
                .inputAttachmentCount    = static_cast<uint32_t>(input_attachments.size()),
                .pInputAttachments       = input_attachments.data(),
                .colorAttachmentCount    = static_cast<uint32_t>(color_attachments.size()),
                .pColorAttachments       = color_attachments.data(),
                .pResolveAttachments     = resolve_attachments.data(),
                .pDepthStencilAttachment = &depth_stencil_attachment,
                .preserveAttachmentCount = static_cast<uint32_t>(preserve_attachments.size()),
                .pPreserveAttachments    = preserve_attachments.data()
            };
        }
    };
    typedef Wrapper<SubpassDescriptionData, VkSubpassDescription> SubpassDescription;

    struct SubpassDependencyData {
        uint32_t             src_subpass;
        uint32_t             dst_subpass;
        VkPipelineStageFlags src_stage_mask;
        VkPipelineStageFlags dst_stage_mask;
        VkAccessFlags        src_access_mask;
        VkAccessFlags        dst_access_mask;
        VkDependencyFlags    dependency_flags;

        inline operator const VkSubpassDependency() const {
            return {
                .srcSubpass      = src_subpass,
                .dstSubpass      = dst_subpass,
                .srcStageMask    = src_stage_mask,
                .dstStageMask    = dst_stage_mask,
                .srcAccessMask   = src_access_mask,
                .dstAccessMask   = dst_access_mask,
                .dependencyFlags = dependency_flags
            };
        }
    };
    typedef Wrapper<SubpassDependencyData, VkSubpassDependency> SubpassDependency;

    struct RenderPassCreateInfoData {
        std::vector<AttachmentDescription> attachments;
        std::vector<SubpassDescription>    subpasses;
        std::vector<SubpassDependency>     dependencies;
        
        inline operator const VkRenderPassCreateInfo() const {
            return {
                .sType           = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
                .attachmentCount = static_cast<uint32_t>(attachments.size()),
                .pAttachments    = cast_vector<const VkAttachmentDescription*>(attachments),
                .subpassCount    = static_cast<uint32_t>(subpasses.size()),
                .pSubpasses      = cast_vector<const VkSubpassDescription*>(subpasses),
                .dependencyCount = static_cast<uint32_t>(dependencies.size()),
                .pDependencies   = cast_vector<const VkSubpassDependency*>(dependencies)
            };
        }
    };
    typedef Wrapper<RenderPassCreateInfoData, VkRenderPassCreateInfo> RenderPassCreateInfo;

    struct FramebufferCreateInfoData {
        VkRenderPass             render_pass;
        std::vector<VkImageView> attachments;
        uint32_t                 width;
        uint32_t                 height;
        uint32_t                 layers;

        inline operator const VkFramebufferCreateInfo() const {
            return {
                .sType           = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
                .renderPass      = render_pass,
                .attachmentCount = static_cast<uint32_t>(attachments.size()),
                .pAttachments    = attachments.data(),
                .width           = width,
                .height          = height,
                .layers          = layers
            };
        }
    };
    typedef Wrapper<FramebufferCreateInfoData, VkFramebufferCreateInfo> FramebufferCreateInfo;

    struct RenderPassBeginInfoData {
        VkRenderPass              render_pass;
        VkFramebuffer             framebuffer;
        VkRect2D                  render_area;
        std::vector<VkClearValue> clear_values;

        inline operator const VkRenderPassBeginInfo() const {
            return {
                .sType           = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
                .renderPass      = render_pass,
                .framebuffer     = framebuffer,
                .renderArea      = render_area,
                .clearValueCount = static_cast<uint32_t>(clear_values.size()),
                .pClearValues    = clear_values.data()
            };
        }
    };
    typedef Wrapper<RenderPassBeginInfoData, VkRenderPassBeginInfo> RenderPassBeginInfo;

    struct CommandBufferInheritanceInfoData {
        VkRenderPass                  render_pass;
        uint32_t                      subpass;
        VkFramebuffer                 framebuffer;
        VkBool32                      occlusion_query_enable;
        VkQueryControlFlags           query_flags;
        VkQueryPipelineStatisticFlags pipeline_statistics;

        inline operator const VkCommandBufferInheritanceInfo() const {
            return {
                .sType                = VK_STRUCTURE_TYPE_COMMAND_BUFFER_INHERITANCE_INFO,
                .renderPass           = render_pass,
                .subpass              = subpass,
                .framebuffer          = framebuffer,
                .occlusionQueryEnable = occlusion_query_enable,
                .queryFlags           = query_flags,
                .pipelineStatistics   = pipeline_statistics
            };
        }
    };
    typedef Wrapper<CommandBufferInheritanceInfoData, VkCommandBufferInheritanceInfo> CommandBufferInheritanceInfo;

    struct CommandBufferBeginInfoData {
        VkCommandBufferUsageFlags    flags;
        CommandBufferInheritanceInfo inheritance_info;

        inline operator const VkCommandBufferBeginInfo() const {
            return {
                .sType            = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
                .flags            = flags,
                .pInheritanceInfo = inheritance_info
            };
        }
    };
    typedef Wrapper<CommandBufferBeginInfoData, VkCommandBufferBeginInfo> CommandBufferBeginInfo;

    struct SubmitInfoData {
        std::vector<VkSemaphore>     wait_semaphores;
        VkPipelineStageFlags         wait_dst_stage_mask;
        std::vector<VkCommandBuffer> command_buffers;
        std::vector<VkSemaphore>     signal_semaphores;

        inline operator const VkSubmitInfo() const {
            return {
                .sType                = VK_STRUCTURE_TYPE_SUBMIT_INFO,
                .waitSemaphoreCount   = static_cast<uint32_t>(wait_semaphores.size()),
                .pWaitSemaphores      = wait_semaphores.data(),
                .pWaitDstStageMask    = &wait_dst_stage_mask,
                .commandBufferCount   = static_cast<uint32_t>(command_buffers.size()),
                .pCommandBuffers      = command_buffers.data(),
                .signalSemaphoreCount = static_cast<uint32_t>(signal_semaphores.size()),
                .pSignalSemaphores    = signal_semaphores.data(),
            };
        }
    };
    typedef Wrapper<SubmitInfoData, VkSubmitInfo> SubmitInfo;

    struct PresentInfoKHRData {
        std::vector<VkSemaphore>    wait_semaphores;
        std::vector<VkSwapchainKHR> swapchains;
        std::vector<uint32_t>       image_indices;

        inline operator const VkPresentInfoKHR() const {
            assert(image_indices.size() == swapchains.size());
            return {
                .sType              = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
                .waitSemaphoreCount = static_cast<uint32_t>(wait_semaphores.size()),
                .pWaitSemaphores    = wait_semaphores.data(),
                .swapchainCount     = static_cast<uint32_t>(swapchains.size()),
                .pSwapchains        = swapchains.data(),
                .pImageIndices      = image_indices.data(),
            };
        }
    };
    typedef Wrapper<PresentInfoKHRData, VkPresentInfoKHR> PresentInfoKHR;

    struct DescriptorSetLayoutCreateInfoData {
        VkDescriptorSetLayoutCreateFlags          flags;
        std::vector<VkDescriptorSetLayoutBinding> bindings;

        inline operator const VkDescriptorSetLayoutCreateInfo() const {
            return {
                .sType        = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
                .flags        = flags,
                .bindingCount = static_cast<uint32_t>(bindings.size()),
                .pBindings    = bindings.data()
            };
        }
    };
    typedef Wrapper<DescriptorSetLayoutCreateInfoData, VkDescriptorSetLayoutCreateInfo> DescriptorSetLayoutCreateInfo;

    struct PipelineLayoutCreateInfoData {
        std::vector<VkDescriptorSetLayout> set_layouts;
        std::vector<VkPushConstantRange>   push_constant_ranges;

        inline operator const VkPipelineLayoutCreateInfo() const {
            return {
                .sType                  = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
                .setLayoutCount         = static_cast<uint32_t>(set_layouts.size()),
                .pSetLayouts            = set_layouts.data(),
                .pushConstantRangeCount = static_cast<uint32_t>(push_constant_ranges.size()),
                .pPushConstantRanges    = push_constant_ranges.data()
            };
        }
    };
    typedef Wrapper<PipelineLayoutCreateInfoData, VkPipelineLayoutCreateInfo> PipelineLayoutCreateInfo;

    struct SpecializationInfoData {
        std::vector<VkSpecializationMapEntry> map_entries;
        std::vector<void*>                    data;

        inline operator const VkSpecializationInfo() const {
            return {
                .mapEntryCount = static_cast<uint32_t>(map_entries.size()),
                .pMapEntries   = map_entries.data(),
                .dataSize      = static_cast<uint32_t>(data.size()),
                .pData         = data.data()
            };
        }
    };
    typedef Wrapper<SpecializationInfoData, VkSpecializationInfo> SpecializationInfo;

    struct PipelineShaderStageCreateInfoData {
        VkShaderStageFlagBits    stage;
        Deleter<VkShaderModule>  module;
        const char*              name;
        SpecializationInfo specialization_info;

        inline operator const VkPipelineShaderStageCreateInfo() const {
            return {
                .sType               = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
                .stage               = stage,
                .module              = module,
                .pName               = name,
                .pSpecializationInfo = specialization_info
            };
        }
    };
    typedef Wrapper<PipelineShaderStageCreateInfoData, VkPipelineShaderStageCreateInfo> PipelineShaderStageCreateInfo;

    struct PipelineVertexInputStateCreateInfoData {
        std::vector<VkVertexInputBindingDescription>   vertex_binding_descriptions;
        std::vector<VkVertexInputAttributeDescription> vertex_attribute_descriptions;

        inline operator const VkPipelineVertexInputStateCreateInfo() const {
            return {
                .sType                           = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
                .vertexBindingDescriptionCount   = static_cast<uint32_t>(vertex_binding_descriptions.size()),
                .pVertexBindingDescriptions      = vertex_binding_descriptions.data(),
                .vertexAttributeDescriptionCount = static_cast<uint32_t>(vertex_attribute_descriptions.size()),
                .pVertexAttributeDescriptions    = vertex_attribute_descriptions.data()
            };
        }
    };
    typedef Wrapper<PipelineVertexInputStateCreateInfoData, VkPipelineVertexInputStateCreateInfo> PipelineVertexInputStateCreateInfo;

    struct PipelineInputAssemblyStateCreateInfoData {
        VkPrimitiveTopology topology;
        VkBool32            primitive_restart_enable;

        inline operator const VkPipelineInputAssemblyStateCreateInfo() const {
            return {
                .sType                  = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
                .topology               = topology,
                .primitiveRestartEnable = primitive_restart_enable
            };
        }
    };
    typedef Wrapper<PipelineInputAssemblyStateCreateInfoData, VkPipelineInputAssemblyStateCreateInfo> PipelineInputAssemblyStateCreateInfo;

    struct PipelineTessellationStateCreateInfoData {
        uint32_t patch_control_points;

        inline operator const VkPipelineTessellationStateCreateInfo() const {
            return {
                .sType              = VK_STRUCTURE_TYPE_PIPELINE_TESSELLATION_STATE_CREATE_INFO,
                .patchControlPoints = patch_control_points
            };
        }
    };
    typedef Wrapper<PipelineTessellationStateCreateInfoData, VkPipelineTessellationStateCreateInfo> PipelineTessellationStateCreateInfo;

    struct PipelineViewportStateCreateInfoData {
        std::vector<VkViewport> viewports;
        std::vector<VkRect2D>   scissors;

        inline operator const VkPipelineViewportStateCreateInfo() const {
            return {
                .sType         = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
                .viewportCount = static_cast<uint32_t>(viewports.size()),
                .pViewports    = viewports.data(),
                .scissorCount  = static_cast<uint32_t>(scissors.size()),
                .pScissors     = scissors.data()
            };
        }
    };
    typedef Wrapper<PipelineViewportStateCreateInfoData, VkPipelineViewportStateCreateInfo> PipelineViewportStateCreateInfo;

    struct PipelineRasterizationStateCreateInfoData {
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

        inline operator const VkPipelineRasterizationStateCreateInfo() const {
            return {
                .sType                   = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
                .depthClampEnable        = depth_clamp_enable,
                .rasterizerDiscardEnable = rasterizer_discard_enable,
                .polygonMode             = polygon_mode,
                .cullMode                = cull_mode,
                .frontFace               = front_face,
                .depthBiasEnable         = depth_bias_enable,
                .depthBiasConstantFactor = depth_bias_constant_factor,
                .depthBiasClamp          = depth_bias_clamp,
                .depthBiasSlopeFactor    = depth_bias_slope_factor,
                .lineWidth               = line_width
            };
        }
    };
    typedef Wrapper<PipelineRasterizationStateCreateInfoData, VkPipelineRasterizationStateCreateInfo> PipelineRasterizationStateCreateInfo;

    struct PipelineMultisampleStateCreateInfoData {
        VkSampleCountFlagBits rasterization_samples;
        VkBool32              sample_shading_enable;
        float                 min_sample_shading;
        VkSampleMask          sample_mask = -1u;
        VkBool32              alpha_to_coverage_enable;
        VkBool32              alpha_to_one_enable;

        inline operator const VkPipelineMultisampleStateCreateInfo() const {
            return {
                .sType                 = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
                .rasterizationSamples  = rasterization_samples,
                .sampleShadingEnable   = sample_shading_enable,
                .minSampleShading      = min_sample_shading,
                .pSampleMask           = &sample_mask,
                .alphaToCoverageEnable = alpha_to_coverage_enable,
                .alphaToOneEnable      = alpha_to_one_enable
            };
        }
    };
    typedef Wrapper<PipelineMultisampleStateCreateInfoData, VkPipelineMultisampleStateCreateInfo> PipelineMultisampleStateCreateInfo;

    struct PipelineDepthStencilStateCreateInfoData {
        VkBool32         depth_test_enable;
        VkBool32         depth_write_enable;
        VkCompareOp      depth_compare_op;
        VkBool32         depth_bounds_test_enable;
        VkBool32         stencil_test_enable;
        VkStencilOpState front;
        VkStencilOpState back;
        float            min_depth_bounds;
        float            max_depth_bounds;

        inline operator const VkPipelineDepthStencilStateCreateInfo() const {
            return {
                .sType                 = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO,
                .depthTestEnable       = depth_test_enable,
                .depthWriteEnable      = depth_write_enable,
                .depthCompareOp        = depth_compare_op,
                .depthBoundsTestEnable = depth_bounds_test_enable,
                .stencilTestEnable     = stencil_test_enable,
                .front                 = front,
                .back                  = back,
                .minDepthBounds        = min_depth_bounds,
                .maxDepthBounds        = max_depth_bounds
            };
        }
    };
    typedef Wrapper<PipelineDepthStencilStateCreateInfoData, VkPipelineDepthStencilStateCreateInfo> PipelineDepthStencilStateCreateInfo;

    struct PipelineColorBlendStateCreateInfoData {
        VkBool32                                         logic_op_enable;
        VkLogicOp                                        logic_op;
        std::vector<VkPipelineColorBlendAttachmentState> attachments;
        std::array<float, 4>                             blend_constants;

        inline operator const VkPipelineColorBlendStateCreateInfo() const {
            VkPipelineColorBlendStateCreateInfo create_info = {
                .sType           = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
                .logicOpEnable   = logic_op_enable,
                .logicOp         = logic_op,
                .attachmentCount = static_cast<uint32_t>(attachments.size()),
                .pAttachments    = attachments.data(),
            };
            std::copy(blend_constants.begin(), blend_constants.end(), create_info.blendConstants);
            return create_info;
        }
    };
    typedef Wrapper<PipelineColorBlendStateCreateInfoData, VkPipelineColorBlendStateCreateInfo> PipelineColorBlendStateCreateInfo;

    struct PipelineDynamicStateCreateInfoData {
        std::vector<VkDynamicState> dynamic_states;

        inline operator const VkPipelineDynamicStateCreateInfo() const {
            return {
                .sType             = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
                .dynamicStateCount = static_cast<uint32_t>(dynamic_states.size()),
                .pDynamicStates    = dynamic_states.data(),
            };
        }
    };
    typedef Wrapper<PipelineDynamicStateCreateInfoData, VkPipelineDynamicStateCreateInfo> PipelineDynamicStateCreateInfo;

    struct GraphicsPipelineCreateInfoData {
        std::vector<PipelineShaderStageCreateInfo>    stages;
        PipelineVertexInputStateCreateInfo            vertex_input_state;
        PipelineInputAssemblyStateCreateInfo          input_assembly_state;
        PipelineTessellationStateCreateInfo           tessellation_state;
        PipelineViewportStateCreateInfo               viewport_state;
        PipelineRasterizationStateCreateInfo          rasterization_state;
        PipelineMultisampleStateCreateInfo            multisample_state;
        PipelineDepthStencilStateCreateInfo           depth_stencil_state;
        PipelineColorBlendStateCreateInfo             color_blend_state;
        std::optional<PipelineDynamicStateCreateInfo> dynamic_state;
        VkPipelineLayout                              layout;
        VkRenderPass                                  render_pass;
        uint32_t                                      subpass;
        VkPipeline                                    base_pipeline_handle;
        int32_t                                       base_pipeline_index;

        inline operator const VkGraphicsPipelineCreateInfo() const {
            return {
                .sType               = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
                .stageCount          = static_cast<uint32_t>(stages.size()),
                .pStages             = cast_vector<const VkPipelineShaderStageCreateInfo*>(stages),
                .pVertexInputState   = vertex_input_state,
                .pInputAssemblyState = input_assembly_state,
                .pTessellationState  = tessellation_state,
                .pViewportState      = viewport_state,
                .pRasterizationState = rasterization_state,
                .pMultisampleState   = multisample_state,
                .pDepthStencilState  = depth_stencil_state,
                .pColorBlendState    = color_blend_state,
                .pDynamicState       = dynamic_state ? *dynamic_state : nullptr,
                .layout              = layout,
                .renderPass          = render_pass,
                .subpass             = subpass,
                .basePipelineHandle  = base_pipeline_handle,
                .basePipelineIndex   = base_pipeline_index
            };
        }
    };
    typedef Wrapper<GraphicsPipelineCreateInfoData, VkGraphicsPipelineCreateInfo> GraphicsPipelineCreateInfo;

    struct WriteDescriptorSetData {
        VkDescriptorSet        dst_set;
        uint32_t               dst_binding;
        uint32_t               dst_array_element;
        uint32_t               descriptor_count;
        VkDescriptorType       descriptor_type;
        VkDescriptorImageInfo  image_info;
        VkDescriptorBufferInfo buffer_info;
        VkBufferView*          texel_buffer_view;

        inline operator const VkWriteDescriptorSet() const {
            return {
                .sType            = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
                .dstSet           = dst_set,
                .dstBinding       = dst_binding,
                .dstArrayElement  = dst_array_element,
                .descriptorCount  = descriptor_count,
                .descriptorType   = descriptor_type,
                .pImageInfo       = &image_info,
                .pBufferInfo      = &buffer_info,
                .pTexelBufferView = texel_buffer_view
            };
        }
    };
    typedef Wrapper<WriteDescriptorSetData, VkWriteDescriptorSet> WriteDescriptorSet;

    struct CopyDescriptorSetData {
        VkDescriptorSet src_set;
        uint32_t        src_binding;
        uint32_t        src_array_element;
        VkDescriptorSet dst_set;
        uint32_t        dst_binding;
        uint32_t        dst_array_element;
        uint32_t        descriptor_count;

        inline operator const VkCopyDescriptorSet() const {
            return {
                .sType           = VK_STRUCTURE_TYPE_COPY_DESCRIPTOR_SET,
                .srcSet          = src_set,
                .srcBinding      = src_binding,
                .srcArrayElement = src_array_element,
                .dstSet          = dst_set,
                .dstBinding      = dst_binding,
                .dstArrayElement = dst_array_element,
                .descriptorCount = descriptor_count
            };
        }
    };
    typedef Wrapper<CopyDescriptorSetData, VkCopyDescriptorSet> CopyDescriptorSet;

}}