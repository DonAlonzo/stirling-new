#pragma once

struct QueueFamilyIndices {
    uint32_t graphics_queue = -1;
    uint32_t present_queue = -1;
};

template<typename From, typename To>
struct VulkanWrapper {
    VulkanWrapper() = default;
    VulkanWrapper(From&& from) : to (std::forward(from)) {}
    
    inline operator const To*() const { return &to; }
    inline const To& data() const { return to; }

private:
    To to;
};

struct VulkanApplicationInfoData {
    const char* application_name;
    uint32_t    application_version;
    const char* engine_name;
    uint32_t    engine_version;
    uint32_t    api_version;

    inline operator const VkApplicationInfo() {
        return {
            .sType              = VK_STRUCTURE_TYPE_APPLICATION_INFO,
            .pNext              = nullptr,
            .pApplicationName   = application_name,
            .applicationVersion = application_version,
            .pEngineName        = engine_name,
            .engineVersion      = engine_version,
            .apiVersion         = api_version
        };
    };
};
typedef VulkanWrapper<VulkanApplicationInfoData, VkApplicationInfo>
    VulkanApplicationInfo;

struct VulkanInstanceCreateInfoData {
    VulkanApplicationInfo    application_info;
    std::vector<const char*> enabled_layers;
    std::vector<const char*> enabled_extensions;

    inline operator const VkInstanceCreateInfo() {
        return {
            .sType                   = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
            .pNext                   = nullptr,
            .flags                   = 0,
            .pApplicationInfo        = application_info,
            .enabledLayerCount       = enabled_layers.size(),
            .ppEnabledLayerNames     = enabled_layers.data(),
            .enabledExtensionCount   = enabled_extensions.size(),
            .ppEnabledExtensionNames = enabled_extensions.data()
        };
    }
};
typedef VulkanWrapper<VulkanInstanceCreateInfoData, VkInstanceCreateInfo>
    VulkanInstanceCreateInfo;

struct VulkanDebugReportCallbackCreateInfoEXTData {
    VkDebugReportFlagsEXT        flags;
    PFN_vkDebugReportCallbackEXT callback;
    void*                        user_data;

    inline operator const VkDebugReportCallbackCreateInfoEXT() {
        return {
            .sType       = VK_STRUCTURE_TYPE_DEBUG_REPORT_CALLBACK_CREATE_INFO_EXT,
            .pNext       = nullptr,
            .flags       = flags,
            .pfnCallback = callback,
            .pUserData   = user_data
        };
    }
};
typedef VulkanWrapper<VulkanDebugReportCallbackCreateInfoEXTData, VkDebugReportCallbackCreateInfoEXT>
    VulkanDebugReportCallbackCreateInfoEXT;

struct VulkanDeviceQueueCreateInfoData {
    uint32_t           queue_family_index;
    std::vector<float> queue_priorities;

    inline operator const VkDeviceQueueCreateInfo() {
        return {
            .sType            = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
            .pNext            = nullptr,
            .flags            = 0,
            .queueFamilyIndex = queue_family_index,
            .queueCount       = queue_priorities.size(),
            .pQueuePriorities = queue_priorities.data()
        };
    }
}; 
typedef VulkanWrapper<VulkanDeviceQueueCreateInfoData, VkDeviceQueueCreateInfo>
    VulkanDeviceQueueCreateInfo; 

struct VulkanDeviceCreateInfoData {
    std::vector<VulkanDeviceQueueCreateInfo> queues;
    std::vector<const char*>                 enabled_layers;
    std::vector<const char*>                 enabled_extensions;
    VkPhysicalDeviceFeatures                 enabled_features;

    inline operator const VkDeviceCreateInfo() {
        return {
            .sType                   = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
            .pNext                   = nullptr,
            .flags                   = 0,
            .queueCreateInfoCount    = queues.size(),
            .pQueueCreateInfos       = cast_vector<const VkDeviceQueueCreateInfo*>(queues)[0],
            .enabledLayerCount       = enabled_layers.size(),
            .ppEnabledLayerNames     = enabled_layers.data(),
            .enabledExtensionCount   = enabled_extensions.size(),
            .ppEnabledExtensionNames = enabled_extensions.data(),
            .pEnabledFeatures        = &enabled_features
        };
    }
};
typedef VulkanWrapper<VulkanDeviceCreateInfoData, VkDeviceCreateInfo>
    VulkanDeviceCreateInfo;

struct VulkanSwapchainCreateInfoKHRData {
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

    inline operator const VkSwapchainCreateInfoKHR() {
        return {
            .sType                 = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
            .pNext                 = nullptr,
            .flags                 = 0,
            .surface               = surface,
            .minImageCount         = min_image_count,
            .imageFormat           = image_format,
            .imageColorSpace       = image_color_space,
            .imageExtent           = image_extent,
            .imageArrayLayers      = image_array_layers,
            .imageUsage            = image_usage,
            .imageSharingMode      = image_sharing_mode,
            .queueFamilyIndexCount = queue_family_indices.size(),
            .pQueueFamilyIndices   = queue_family_indices.data(),
            .preTransform          = pre_transform,
            .compositeAlpha        = composite_alpha,
            .presentMode           = present_mode,
            .clipped               = clipped,
            .oldSwapchain          = old_swapchain
        };
    }
};
typedef VulkanWrapper<VulkanSwapchainCreateInfoKHRData, VkSwapchainCreateInfoKHR>
    VulkanSwapchainCreateInfoKHR;

struct VulkanImageViewCreateInfoData {
    VkImage                 image;
    VkImageViewType         view_type;
    VkFormat                format;
    VkComponentMapping      components;
    VkImageSubresourceRange subresource_range;

    inline operator const VkImageViewCreateInfo() {
        return {
            .sType            = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
            .pNext            = nullptr,
            .flags            = 0,
            .image            = image,
            .viewType         = view_type,
            .format           = format,
            .components       = components,
            .subresourceRange = subresource_range
        };
    }
};
typedef VulkanWrapper<VulkanImageViewCreateInfoData, VkImageViewCreateInfo>
    VulkanImageViewCreateInfo;

struct VulkanAttachmentDescriptionData {
    VkAttachmentDescriptionFlags flags;
    VkFormat                     format;
    VkSampleCountFlagBits        samples;
    VkAttachmentLoadOp           load_op;
    VkAttachmentStoreOp          store_op;
    VkAttachmentLoadOp           stencil_load_op;
    VkAttachmentStoreOp          stencil_store_op;
    VkImageLayout                initial_layout;
    VkImageLayout                final_layout;

    inline operator const VkAttachmentDescription() {
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
typedef VulkanWrapper<VulkanAttachmentDescriptionData, VkAttachmentDescription>
    VulkanAttachmentDescription;

struct VulkanSubpassDescriptionData {
    VkSubpassDescriptionFlags          flags;
    VkPipelineBindPoint                pipeline_bind_point;
    std::vector<VkAttachmentReference> input_attachments;
    std::vector<VkAttachmentReference> color_attachments;
    std::vector<VkAttachmentReference> resolve_attachments;
    VkAttachmentReference              depth_stencil_attachment;
    std::vector<uint32_t>              preserve_attachments;

    inline operator const VkSubpassDescription() {
        return {
            .flags                   = flags,
            .pipelineBindPoint       = pipeline_bind_point,
            .inputAttachmentCount    = input_attachments.size(),
            .pInputAttachments       = input_attachments.data(),
            .colorAttachmentCount    = color_attachments.size(),
            .pColorAttachments       = color_attachments.data(),
            .pResolveAttachments     = resolve_attachments.data(),
            .pDepthStencilAttachment = &depth_stencil_attachment,
            .preserveAttachmentCount = preserve_attachments.size(),
            .pPreserveAttachments    = preserve_attachments.data()
        };
    }
};
typedef VulkanWrapper<VulkanSubpassDescriptionData, VkSubpassDescription>
    VulkanSubpassDescription;

struct VulkanSubpassDependencyData {
    uint32_t             src_subpass;
    uint32_t             dst_subpass;
    VkPipelineStageFlags src_stage_mask;
    VkPipelineStageFlags dst_stage_mask;
    VkAccessFlags        src_access_mask;
    VkAccessFlags        dst_access_mask;
    VkDependencyFlags    dependency_flags;

    inline operator const VkSubpassDependency() {
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
typedef VulkanWrapper<VulkanSubpassDependencyData, VkSubpassDependency>
    VulkanSubpassDependency;

struct VulkanRenderPassCreateInfoData {
    std::vector<VulkanAttachmentDescription> attachments;
    std::vector<VulkanSubpassDescription>    subpasses;
    std::vector<VulkanSubpassDependency>     dependencies;
    
    inline operator const VkRenderPassCreateInfo() {
        return {
            .sType           = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
            .pNext           = nullptr,
            .flags           = 0,
            .attachmentCount = attachments.size(),
            .pAttachments    = cast_vector<const VkAttachmentDescription*>(attachments)[0],
            .subpassCount    = subpasses.size(),
            .pSubpasses      = cast_vector<const VkSubpassDescription*>(subpasses)[0],
            .dependencyCount = dependencies.size(),
            .pDependencies   = cast_vector<const VkSubpassDependency*>(dependencies)[0]
        };
    }
};
typedef VulkanWrapper<VulkanRenderPassCreateInfoData, VkRenderPassCreateInfo>
    VulkanRenderPassCreateInfo;

struct VulkanFramebufferCreateInfoData {
    VkRenderPass             render_pass;
    std::vector<VkImageView> attachments;
    uint32_t                 width;
    uint32_t                 height;
    uint32_t                 layers;

    inline operator const VkFramebufferCreateInfo() {
        return {
            .sType           = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
            .pNext           = nullptr,
            .flags           = 0,
            .renderPass      = render_pass,
            .attachmentCount = attachments.size(),
            .pAttachments    = attachments.data(),
            .width           = width,
            .height          = height,
            .layers          = layers
        };
    }
};
typedef VulkanWrapper<VulkanFramebufferCreateInfoData, VkFramebufferCreateInfo>
    VulkanFramebufferCreateInfo;

struct VulkanCommandPoolCreateInfoData {
    VkCommandPoolCreateFlags flags;
    uint32_t                 queue_family_index;

    operator const VkCommandPoolCreateInfo() {
        return {
            .sType            = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
            .pNext            = nullptr,
            .flags            = flags,
            .queueFamilyIndex = queue_family_index
        };
    }
};
typedef VulkanWrapper<VulkanCommandPoolCreateInfoData, VkCommandPoolCreateInfo>
    VulkanCommandPoolCreateInfo;

struct VulkanCommandBufferAllocateInfoData {
    VkCommandPool           command_pool;
    VkCommandBufferLevel    level;
    uint32_t                command_buffer_count;

    operator const VkCommandBufferAllocateInfo() {
        return {
            .sType              = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
            .pNext              = nullptr,
            .commandPool        = command_pool,
            .level              = level,
            .commandBufferCount = command_buffer_count
        };
    }
};
typedef VulkanWrapper<VulkanCommandBufferAllocateInfoData, VkCommandBufferAllocateInfo>
    VulkanCommandBufferAllocateInfo;

struct VulkanRenderPassBeginInfoData {
    VkRenderPass              render_pass;
    VkFramebuffer             framebuffer;
    VkRect2D                  render_area;
    std::vector<VkClearValue> clear_values;

    operator const VkRenderPassBeginInfo() {
        return {
            .sType           = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
            .pNext           = nullptr,
            .renderPass      = render_pass,
            .framebuffer     = framebuffer,
            .renderArea      = render_area,
            .clearValueCount = clear_values.size(),
            .pClearValues    = clear_values.data()
        };
    }
};
typedef VulkanWrapper<VulkanRenderPassBeginInfoData, VkRenderPassBeginInfo>
    VulkanRenderPassBeginInfo;

struct VulkanCommandBufferInheritanceInfoData {
    VkRenderPass                   render_pass;
    uint32_t                       subpass;
    VkFramebuffer                  framebuffer;
    VkBool32                       occlusion_query_enable;
    VkQueryControlFlags            query_flags;
    VkQueryPipelineStatisticFlags  pipeline_statistics;

    operator const VkCommandBufferInheritanceInfo() {
        return {
            .sType                = VK_STRUCTURE_TYPE_COMMAND_BUFFER_INHERITANCE_INFO,
            .pNext                = nullptr,
            .renderPass           = render_pass,
            .subpass              = subpass,
            .framebuffer          = framebuffer,
            .occlusionQueryEnable = occlusion_query_enable,
            .queryFlags           = query_flags,
            .pipelineStatistics   = pipeline_statistics
        };
    }
};
typedef VulkanWrapper<VulkanCommandBufferInheritanceInfoData, VkCommandBufferInheritanceInfo>
    VulkanCommandBufferInheritanceInfo;

struct VulkanCommandBufferBeginInfoData {
    VkCommandBufferUsageFlags          flags;
    VulkanCommandBufferInheritanceInfo inheritance_info;

    operator const VkCommandBufferBeginInfo() {
        return {
            .sType            = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
            .pNext            = nullptr,
            .flags            = flags,
            .pInheritanceInfo = inheritance_info
        };
    }
};
typedef VulkanWrapper<VulkanCommandBufferBeginInfoData, VkCommandBufferBeginInfo>
    VulkanCommandBufferBeginInfo;

struct VulkanSubmitInfoData {
    std::vector<VkSemaphore>     wait_semaphores;
    VkPipelineStageFlags         wait_dst_stage_mask;
    std::vector<VkCommandBuffer> command_buffers;
    std::vector<VkSemaphore>     signal_semaphores;

    operator const VkSubmitInfo() {
        return {
            .sType                = VK_STRUCTURE_TYPE_SUBMIT_INFO,
            .pNext                = nullptr,
            .waitSemaphoreCount   = wait_semaphores.size(),
            .pWaitSemaphores      = wait_semaphores.data(),
            .pWaitDstStageMask    = &wait_dst_stage_mask,
            .commandBufferCount   = command_buffers.size(),
            .pCommandBuffers      = command_buffers.data(),
            .signalSemaphoreCount = signal_semaphores.size(),
            .pSignalSemaphores    = signal_semaphores.data(),
        };
    }
};
typedef VulkanWrapper<VulkanSubmitInfoData, VkSubmitInfo>
    VulkanSubmitInfo;

struct VulkanPresentInfoKHRData {
    std::vector<VkSemaphore>    wait_semaphores;
    std::vector<VkSwapchainKHR> swapchains;
    std::vector<uint32_t>       image_indices;

    operator const VkPresentInfoKHR() {
        assert(image_indices.size() == swapchains.size());
        return {
            .sType              = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
            .pNext              = nullptr,
            .waitSemaphoreCount = wait_semaphores.size(),
            .pWaitSemaphores    = wait_semaphores.data(),
            .swapchainCount     = swapchains.size(),
            .pSwapchains        = swapchains.data(),
            .pImageIndices      = image_indices.data(),
            .pResults           = nullptr
        };
    }
};
typedef VulkanWrapper<VulkanPresentInfoKHRData, VkPresentInfoKHR>
    VulkanPresentInfoKHR;

struct VulkanPipelineLayoutCreateInfoData {
    std::vector<VkDescriptorSetLayout> set_layouts;
    std::vector<VkPushConstantRange>   push_constant_ranges;

    operator const VkPipelineLayoutCreateInfo() {
        return {
            .sType                  = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
            .pNext                  = nullptr,
            .flags                  = 0,
            .setLayoutCount         = set_layouts.size(),
            .pSetLayouts            = set_layouts.data(),
            .pushConstantRangeCount = push_constant_ranges.size(),
            .pPushConstantRanges    = push_constant_ranges.data()
        };
    }
};
typedef VulkanWrapper<VulkanPipelineLayoutCreateInfoData, VkPipelineLayoutCreateInfo>
    VulkanPipelineLayoutCreateInfo;

struct VulkanSpecializationInfoData {
    std::vector<VkSpecializationMapEntry> map_entries;
    std::vector<void*>                    data;

    operator const VkSpecializationInfo() {
        return {
            .mapEntryCount = map_entries.size(),
            .pMapEntries   = map_entries.data(),
            .dataSize      = data.size(),
            .pData         = data.data()
        };
    }
};
typedef VulkanWrapper<VulkanSpecializationInfoData, VkSpecializationInfo>
    VulkanSpecializationInfo;

struct VulkanPipelineShaderStageCreateInfoData {
    VkShaderStageFlagBits    stage;
    Deleter<VkShaderModule>  module;
    const char*              name;
    VulkanSpecializationInfo specialization_info;

    operator const VkPipelineShaderStageCreateInfo() {
        return {
            .sType               = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
            .pNext               = nullptr,
            .flags               = 0,
            .stage               = stage,
            .module              = module,
            .pName               = name,
            .pSpecializationInfo = specialization_info
        };
    }
};
typedef VulkanWrapper<VulkanPipelineShaderStageCreateInfoData, VkPipelineShaderStageCreateInfo>
    VulkanPipelineShaderStageCreateInfo;

struct VulkanPipelineVertexInputStateCreateInfoData {
    std::vector<VkVertexInputBindingDescription>   vertex_binding_descriptions;
    std::vector<VkVertexInputAttributeDescription> vertex_attribute_descriptions;

    operator const VkPipelineVertexInputStateCreateInfo() {
        return {
            .sType                           = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
            .pNext                           = nullptr,
            .flags                           = 0,
            .vertexBindingDescriptionCount   = vertex_binding_descriptions.size(),
            .pVertexBindingDescriptions      = vertex_binding_descriptions.data(),
            .vertexAttributeDescriptionCount = vertex_attribute_descriptions.size(),
            .pVertexAttributeDescriptions    = vertex_attribute_descriptions.data()
        };
    }
};
typedef VulkanWrapper<VulkanPipelineVertexInputStateCreateInfoData, VkPipelineVertexInputStateCreateInfo>
    VulkanPipelineVertexInputStateCreateInfo;

struct VulkanPipelineInputAssemblyStateCreateInfoData {
    VkPrimitiveTopology topology;
    VkBool32            primitive_restart_enable;

    operator const VkPipelineInputAssemblyStateCreateInfo() {
        return {
            .sType                  = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
            .pNext                  = nullptr,
            .flags                  = 0,
            .topology               = topology,
            .primitiveRestartEnable = primitive_restart_enable
        };
    }
};
typedef VulkanWrapper<VulkanPipelineInputAssemblyStateCreateInfoData, VkPipelineInputAssemblyStateCreateInfo>
    VulkanPipelineInputAssemblyStateCreateInfo;

struct VulkanPipelineTessellationStateCreateInfoData {
    uint32_t patch_control_points;

    operator const VkPipelineTessellationStateCreateInfo() {
        return {
            .sType              = VK_STRUCTURE_TYPE_PIPELINE_TESSELLATION_STATE_CREATE_INFO,
            .pNext              = nullptr,
            .flags              = 0,
            .patchControlPoints = patch_control_points
        };
    }
};
typedef VulkanWrapper<VulkanPipelineTessellationStateCreateInfoData, VkPipelineTessellationStateCreateInfo>
    VulkanPipelineTessellationStateCreateInfo;

struct VulkanPipelineViewportStateCreateInfoData {
    std::vector<VkViewport> viewports;
    std::vector<VkRect2D>   scissors;

    operator const VkPipelineViewportStateCreateInfo() {
        return {
            .sType         = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
            .pNext         = nullptr,
            .flags         = 0,
            .viewportCount = viewports.size(),
            .pViewports    = viewports.data(),
            .scissorCount  = scissors.size(),
            .pScissors     = scissors.data()
        };
    }
};
typedef VulkanWrapper<VulkanPipelineViewportStateCreateInfoData, VkPipelineViewportStateCreateInfo>
    VulkanPipelineViewportStateCreateInfo;

struct VulkanPipelineRasterizationStateCreateInfoData {
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

    operator const VkPipelineRasterizationStateCreateInfo() {
        return {
            .sType                   = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
            .pNext                   = nullptr,
            .flags                   = 0,
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
typedef VulkanWrapper<VulkanPipelineRasterizationStateCreateInfoData, VkPipelineRasterizationStateCreateInfo>
    VulkanPipelineRasterizationStateCreateInfo;

struct VulkanPipelineMultisampleStateCreateInfoData {
    VkSampleCountFlagBits rasterization_samples;
    VkBool32              sample_shading_enable;
    float                 min_sample_shading;
    VkSampleMask          sample_mask;
    VkBool32              alpha_to_coverage_enable;
    VkBool32              alpha_to_one_enable;

    operator const VkPipelineMultisampleStateCreateInfo() {
        return {
            .sType                 = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
            .pNext                 = nullptr,
            .flags                 = 0,
            .rasterizationSamples  = rasterization_samples,
            .sampleShadingEnable   = sample_shading_enable,
            .minSampleShading      = min_sample_shading,
            .pSampleMask           = &sample_mask,
            .alphaToCoverageEnable = alpha_to_coverage_enable,
            .alphaToOneEnable      = alpha_to_one_enable
        };
    }
};
typedef VulkanWrapper<VulkanPipelineMultisampleStateCreateInfoData, VkPipelineMultisampleStateCreateInfo>
    VulkanPipelineMultisampleStateCreateInfo;

struct VulkanPipelineDepthStencilStateCreateInfoData {
    VkBool32         depth_test_enable;
    VkBool32         depth_write_enable;
    VkCompareOp      depth_compare_op;
    VkBool32         depth_bounds_test_enable;
    VkBool32         stencil_test_enable;
    VkStencilOpState front;
    VkStencilOpState back;
    float            min_depth_bounds;
    float            max_depth_bounds;

    operator const VkPipelineDepthStencilStateCreateInfo() {
        return {
            .sType                 = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO,
            .pNext                 = nullptr,
            .flags                 = 0,
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
typedef VulkanWrapper<VulkanPipelineDepthStencilStateCreateInfoData, VkPipelineDepthStencilStateCreateInfo>
    VulkanPipelineDepthStencilStateCreateInfo;

struct VulkanPipelineColorBlendStateCreateInfoData {
    VkBool32                                         logic_op_enable;
    VkLogicOp                                        logic_op;
    std::vector<VkPipelineColorBlendAttachmentState> attachments;
    std::array<float, 4>                             blend_constants;

    operator const VkPipelineColorBlendStateCreateInfo() {
        VkPipelineColorBlendStateCreateInfo create_info = {
            .sType           = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
            .pNext           = nullptr,
            .flags           = 0,
            .logicOpEnable   = logic_op_enable,
            .logicOp         = logic_op,
            .attachmentCount = attachments.size(),
            .pAttachments    = attachments.data(),
        };
        std::copy(blend_constants.begin(), blend_constants.end(), create_info.blendConstants);
        return create_info;
    }
};
typedef VulkanWrapper<VulkanPipelineColorBlendStateCreateInfoData, VkPipelineColorBlendStateCreateInfo>
    VulkanPipelineColorBlendStateCreateInfo;

struct VulkanPipelineDynamicStateCreateInfoData {
    std::vector<VkDynamicState> dynamic_states;

    operator const VkPipelineDynamicStateCreateInfo() {
        return {
            .sType             = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
            .pNext             = nullptr,
            .flags             = 0,
            .dynamicStateCount = dynamic_states.size(),
            .pDynamicStates    = dynamic_states.data(),
        };
    }
};
typedef VulkanWrapper<VulkanPipelineDynamicStateCreateInfoData, VkPipelineDynamicStateCreateInfo>
    VulkanPipelineDynamicStateCreateInfo;

struct VulkanGraphicsPipelineCreateInfoData {
    std::vector<VulkanPipelineShaderStageCreateInfo> stages;
    VulkanPipelineVertexInputStateCreateInfo         vertex_input_state;
    VulkanPipelineInputAssemblyStateCreateInfo       input_assembly_state;
    VulkanPipelineTessellationStateCreateInfo        tessellation_state;
    VulkanPipelineViewportStateCreateInfo            viewport_state;
    VulkanPipelineRasterizationStateCreateInfo       rasterization_state;
    VulkanPipelineMultisampleStateCreateInfo         multisample_state;
    VulkanPipelineDepthStencilStateCreateInfo        depth_stencil_state;
    VulkanPipelineColorBlendStateCreateInfo          color_blend_state;
    VulkanPipelineDynamicStateCreateInfo             dynamic_state;
    VkPipelineLayout                                 layout;
    VkRenderPass                                     render_pass;
    uint32_t                                         subpass;
    VkPipeline                                       base_pipeline_handle;
    int32_t                                          base_pipeline_index;

    operator const VkGraphicsPipelineCreateInfo() {
        return {
            .sType               = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
            .pNext               = nullptr,
            .flags               = 0,
            .stageCount          = stages.size(),
            .pStages             = cast_vector<const VkPipelineShaderStageCreateInfo*>(stages)[0],
            .pVertexInputState   = vertex_input_state,
            .pInputAssemblyState = input_assembly_state,
            .pTessellationState  = tessellation_state,
            .pViewportState      = viewport_state,
            .pRasterizationState = rasterization_state,
            .pMultisampleState   = multisample_state,
            .pDepthStencilState  = depth_stencil_state,
            .pColorBlendState    = color_blend_state,
            .pDynamicState       = dynamic_state,
            .layout              = layout,
            .renderPass          = render_pass,
            .subpass             = subpass,
            .basePipelineHandle  = base_pipeline_handle,
            .basePipelineIndex   = base_pipeline_index
        };
    }
};
typedef VulkanWrapper<VulkanGraphicsPipelineCreateInfoData, VkGraphicsPipelineCreateInfo>
    VulkanGraphicsPipelineCreateInfo;
