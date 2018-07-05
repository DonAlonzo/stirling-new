#pragma once

#include "vulkan_helpers.hpp"

#include <vulkan/vulkan.h>

#include <vector>

struct QueueFamilyIndices {
    uint32_t graphics_queue = -1;
    uint32_t present_queue = -1;
};

template<typename From, typename To>
struct VulkanWrapper {
    VulkanWrapper() : to (From{}) {}
    VulkanWrapper(From&& from) : to (from) {}

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
            .pApplicationInfo        = application_info,
            .enabledLayerCount       = static_cast<uint32_t>(enabled_layers.size()),
            .ppEnabledLayerNames     = enabled_layers.data(),
            .enabledExtensionCount   = static_cast<uint32_t>(enabled_extensions.size()),
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
            .queueFamilyIndex = queue_family_index,
            .queueCount       = static_cast<uint32_t>(queue_priorities.size()),
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
            .queueCreateInfoCount    = static_cast<uint32_t>(queues.size()),
            .pQueueCreateInfos       = cast_vector<const VkDeviceQueueCreateInfo*>(queues),
            .enabledLayerCount       = static_cast<uint32_t>(enabled_layers.size()),
            .ppEnabledLayerNames     = enabled_layers.data(),
            .enabledExtensionCount   = static_cast<uint32_t>(enabled_extensions.size()),
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
    VkAttachmentReference              depth_stencil_attachment = { VK_ATTACHMENT_UNUSED };
    std::vector<uint32_t>              preserve_attachments;

    inline operator const VkSubpassDescription() {
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
            .attachmentCount = static_cast<uint32_t>(attachments.size()),
            .pAttachments    = cast_vector<const VkAttachmentDescription*>(attachments),
            .subpassCount    = static_cast<uint32_t>(subpasses.size()),
            .pSubpasses      = cast_vector<const VkSubpassDescription*>(subpasses),
            .dependencyCount = static_cast<uint32_t>(dependencies.size()),
            .pDependencies   = cast_vector<const VkSubpassDependency*>(dependencies)
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
            .renderPass      = render_pass,
            .attachmentCount = static_cast<uint32_t>(attachments.size()),
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

    inline operator const VkCommandPoolCreateInfo() {
        return {
            .sType            = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
            .flags            = flags,
            .queueFamilyIndex = queue_family_index
        };
    }
};
typedef VulkanWrapper<VulkanCommandPoolCreateInfoData, VkCommandPoolCreateInfo>
    VulkanCommandPoolCreateInfo;

struct VulkanCommandBufferAllocateInfoData {
    VkCommandPool        command_pool;
    VkCommandBufferLevel level;
    uint32_t             command_buffer_count;

    inline operator const VkCommandBufferAllocateInfo() {
        return {
            .sType              = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
            .commandPool        = command_pool,
            .level              = level,
            .commandBufferCount = command_buffer_count
        };
    }
};
typedef VulkanWrapper<VulkanCommandBufferAllocateInfoData, VkCommandBufferAllocateInfo>
    VulkanCommandBufferAllocateInfo;

struct VulkanDescriptorSetAllocateInfoData {
    VkDescriptorPool                   descriptor_pool;
    std::vector<VkDescriptorSetLayout> set_layouts;

    inline operator const VkDescriptorSetAllocateInfo() {
        return {
            .sType              = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
            .descriptorPool     = descriptor_pool,
            .descriptorSetCount = static_cast<uint32_t>(set_layouts.size()),
            .pSetLayouts        = set_layouts.data()
        };
    }
};
typedef VulkanWrapper<VulkanDescriptorSetAllocateInfoData, VkDescriptorSetAllocateInfo>
    VulkanDescriptorSetAllocateInfo;

struct VulkanRenderPassBeginInfoData {
    VkRenderPass              render_pass;
    VkFramebuffer             framebuffer;
    VkRect2D                  render_area;
    std::vector<VkClearValue> clear_values;

    inline operator const VkRenderPassBeginInfo() {
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
typedef VulkanWrapper<VulkanRenderPassBeginInfoData, VkRenderPassBeginInfo>
    VulkanRenderPassBeginInfo;

struct VulkanCommandBufferInheritanceInfoData {
    VkRenderPass                  render_pass;
    uint32_t                      subpass;
    VkFramebuffer                 framebuffer;
    VkBool32                      occlusion_query_enable;
    VkQueryControlFlags           query_flags;
    VkQueryPipelineStatisticFlags pipeline_statistics;

    inline operator const VkCommandBufferInheritanceInfo() {
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
typedef VulkanWrapper<VulkanCommandBufferInheritanceInfoData, VkCommandBufferInheritanceInfo>
    VulkanCommandBufferInheritanceInfo;

struct VulkanCommandBufferBeginInfoData {
    VkCommandBufferUsageFlags          flags;
    VulkanCommandBufferInheritanceInfo inheritance_info;

    inline operator const VkCommandBufferBeginInfo() {
        return {
            .sType            = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
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

    inline operator const VkSubmitInfo() {
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
typedef VulkanWrapper<VulkanSubmitInfoData, VkSubmitInfo>
    VulkanSubmitInfo;

struct VulkanPresentInfoKHRData {
    std::vector<VkSemaphore>    wait_semaphores;
    std::vector<VkSwapchainKHR> swapchains;
    std::vector<uint32_t>       image_indices;

    inline operator const VkPresentInfoKHR() {
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
typedef VulkanWrapper<VulkanPresentInfoKHRData, VkPresentInfoKHR>
    VulkanPresentInfoKHR;

struct VulkanDescriptorSetLayoutCreateInfoData {
    VkDescriptorSetLayoutCreateFlags          flags;
    std::vector<VkDescriptorSetLayoutBinding> bindings;

    inline operator const VkDescriptorSetLayoutCreateInfo() {
        return {
            .sType        = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
            .flags        = flags,
            .bindingCount = static_cast<uint32_t>(bindings.size()),
            .pBindings    = bindings.data()
        };
    }
};
typedef VulkanWrapper<VulkanDescriptorSetLayoutCreateInfoData, VkDescriptorSetLayoutCreateInfo>
    VulkanDescriptorSetLayoutCreateInfo;

struct VulkanPipelineLayoutCreateInfoData {
    std::vector<VkDescriptorSetLayout> set_layouts;
    std::vector<VkPushConstantRange>   push_constant_ranges;

    inline operator const VkPipelineLayoutCreateInfo() {
        return {
            .sType                  = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
            .setLayoutCount         = static_cast<uint32_t>(set_layouts.size()),
            .pSetLayouts            = set_layouts.data(),
            .pushConstantRangeCount = static_cast<uint32_t>(push_constant_ranges.size()),
            .pPushConstantRanges    = push_constant_ranges.data()
        };
    }
};
typedef VulkanWrapper<VulkanPipelineLayoutCreateInfoData, VkPipelineLayoutCreateInfo>
    VulkanPipelineLayoutCreateInfo;

struct VulkanSpecializationInfoData {
    std::vector<VkSpecializationMapEntry> map_entries;
    std::vector<void*>                    data;

    inline operator const VkSpecializationInfo() {
        return {
            .mapEntryCount = static_cast<uint32_t>(map_entries.size()),
            .pMapEntries   = map_entries.data(),
            .dataSize      = static_cast<uint32_t>(data.size()),
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

    inline operator const VkPipelineShaderStageCreateInfo() {
        return {
            .sType               = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
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

    inline operator const VkPipelineVertexInputStateCreateInfo() {
        return {
            .sType                           = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
            .vertexBindingDescriptionCount   = static_cast<uint32_t>(vertex_binding_descriptions.size()),
            .pVertexBindingDescriptions      = vertex_binding_descriptions.data(),
            .vertexAttributeDescriptionCount = static_cast<uint32_t>(vertex_attribute_descriptions.size()),
            .pVertexAttributeDescriptions    = vertex_attribute_descriptions.data()
        };
    }
};
typedef VulkanWrapper<VulkanPipelineVertexInputStateCreateInfoData, VkPipelineVertexInputStateCreateInfo>
    VulkanPipelineVertexInputStateCreateInfo;

struct VulkanPipelineInputAssemblyStateCreateInfoData {
    VkPrimitiveTopology topology;
    VkBool32            primitive_restart_enable;

    inline operator const VkPipelineInputAssemblyStateCreateInfo() {
        return {
            .sType                  = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
            .topology               = topology,
            .primitiveRestartEnable = primitive_restart_enable
        };
    }
};
typedef VulkanWrapper<VulkanPipelineInputAssemblyStateCreateInfoData, VkPipelineInputAssemblyStateCreateInfo>
    VulkanPipelineInputAssemblyStateCreateInfo;

struct VulkanPipelineTessellationStateCreateInfoData {
    uint32_t patch_control_points;

    inline operator const VkPipelineTessellationStateCreateInfo() {
        return {
            .sType              = VK_STRUCTURE_TYPE_PIPELINE_TESSELLATION_STATE_CREATE_INFO,
            .patchControlPoints = patch_control_points
        };
    }
};
typedef VulkanWrapper<VulkanPipelineTessellationStateCreateInfoData, VkPipelineTessellationStateCreateInfo>
    VulkanPipelineTessellationStateCreateInfo;

struct VulkanPipelineViewportStateCreateInfoData {
    std::vector<VkViewport> viewports;
    std::vector<VkRect2D>   scissors;

    inline operator const VkPipelineViewportStateCreateInfo() {
        return {
            .sType         = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
            .viewportCount = static_cast<uint32_t>(viewports.size()),
            .pViewports    = viewports.data(),
            .scissorCount  = static_cast<uint32_t>(scissors.size()),
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

    inline operator const VkPipelineRasterizationStateCreateInfo() {
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
typedef VulkanWrapper<VulkanPipelineRasterizationStateCreateInfoData, VkPipelineRasterizationStateCreateInfo>
    VulkanPipelineRasterizationStateCreateInfo;

struct VulkanPipelineMultisampleStateCreateInfoData {
    VkSampleCountFlagBits rasterization_samples;
    VkBool32              sample_shading_enable;
    float                 min_sample_shading;
    VkSampleMask          sample_mask = -1u;
    VkBool32              alpha_to_coverage_enable;
    VkBool32              alpha_to_one_enable;

    inline operator const VkPipelineMultisampleStateCreateInfo() {
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

    inline operator const VkPipelineDepthStencilStateCreateInfo() {
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
typedef VulkanWrapper<VulkanPipelineDepthStencilStateCreateInfoData, VkPipelineDepthStencilStateCreateInfo>
    VulkanPipelineDepthStencilStateCreateInfo;

struct VulkanPipelineColorBlendStateCreateInfoData {
    VkBool32                                         logic_op_enable;
    VkLogicOp                                        logic_op;
    std::vector<VkPipelineColorBlendAttachmentState> attachments;
    std::array<float, 4>                             blend_constants;

    inline operator const VkPipelineColorBlendStateCreateInfo() {
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
typedef VulkanWrapper<VulkanPipelineColorBlendStateCreateInfoData, VkPipelineColorBlendStateCreateInfo>
    VulkanPipelineColorBlendStateCreateInfo;

struct VulkanPipelineDynamicStateCreateInfoData {
    std::vector<VkDynamicState> dynamic_states;

    inline operator const VkPipelineDynamicStateCreateInfo() {
        return {
            .sType             = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
            .dynamicStateCount = static_cast<uint32_t>(dynamic_states.size()),
            .pDynamicStates    = dynamic_states.data(),
        };
    }
};
typedef VulkanWrapper<VulkanPipelineDynamicStateCreateInfoData, VkPipelineDynamicStateCreateInfo>
    VulkanPipelineDynamicStateCreateInfo;

struct VulkanGraphicsPipelineCreateInfoData {
    std::vector<VulkanPipelineShaderStageCreateInfo>    stages;
    VulkanPipelineVertexInputStateCreateInfo            vertex_input_state;
    VulkanPipelineInputAssemblyStateCreateInfo          input_assembly_state;
    VulkanPipelineTessellationStateCreateInfo           tessellation_state;
    VulkanPipelineViewportStateCreateInfo               viewport_state;
    VulkanPipelineRasterizationStateCreateInfo          rasterization_state;
    VulkanPipelineMultisampleStateCreateInfo            multisample_state;
    VulkanPipelineDepthStencilStateCreateInfo           depth_stencil_state;
    VulkanPipelineColorBlendStateCreateInfo             color_blend_state;
    std::optional<VulkanPipelineDynamicStateCreateInfo> dynamic_state;
    VkPipelineLayout                                    layout;
    VkRenderPass                                        render_pass;
    uint32_t                                            subpass;
    VkPipeline                                          base_pipeline_handle;
    int32_t                                             base_pipeline_index;

    inline operator const VkGraphicsPipelineCreateInfo() {
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
typedef VulkanWrapper<VulkanGraphicsPipelineCreateInfoData, VkGraphicsPipelineCreateInfo>
    VulkanGraphicsPipelineCreateInfo;

struct VulkanBufferCreateInfoData {
    VkDeviceSize          size;
    VkBufferUsageFlags    usage;
    VkSharingMode         sharing_mode;
    std::vector<uint32_t> queue_family_indices;

    inline operator const VkBufferCreateInfo() {
        return {
            .sType                 = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
            .size                  = size,
            .usage                 = usage,
            .sharingMode           = sharing_mode,
            .queueFamilyIndexCount = static_cast<uint32_t>(queue_family_indices.size()),
            .pQueueFamilyIndices   = queue_family_indices.data()
        };
    }
};
typedef VulkanWrapper<VulkanBufferCreateInfoData, VkBufferCreateInfo>
    VulkanBufferCreateInfo;

struct VulkanMemoryAllocateInfoData {
    VkDeviceSize allocation_size;
    uint32_t     memory_type_index;

    inline operator const VkMemoryAllocateInfo() {
        return {
            .sType           = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
            .allocationSize  = allocation_size,
            .memoryTypeIndex = memory_type_index
        };
    }
};
typedef VulkanWrapper<VulkanMemoryAllocateInfoData, VkMemoryAllocateInfo>
    VulkanMemoryAllocateInfo;

struct VulkanDescriptorPoolCreateInfoData {
    VkDescriptorPoolCreateFlags       flags;
    uint32_t                          max_sets;
    std::vector<VkDescriptorPoolSize> pool_sizes;

    inline operator const VkDescriptorPoolCreateInfo() {
        return {
            .sType         = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
            .flags         = flags,
            .maxSets       = max_sets,
            .poolSizeCount = static_cast<uint32_t>(pool_sizes.size()),
            .pPoolSizes    = pool_sizes.data()
        };
    }
};
typedef VulkanWrapper<VulkanDescriptorPoolCreateInfoData, VkDescriptorPoolCreateInfo>
    VulkanDescriptorPoolCreateInfo;

struct VulkanWriteDescriptorSetData {
    VkDescriptorSet        dst_set;
    uint32_t               dst_binding;
    uint32_t               dst_array_element;
    uint32_t               descriptor_count;
    VkDescriptorType       descriptor_type;
    VkDescriptorImageInfo  image_info;
    VkDescriptorBufferInfo buffer_info;
    VkBufferView*          texel_buffer_view;

    inline operator const VkWriteDescriptorSet() {
        return {
            .sType            = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
            .pNext            = nullptr,
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
typedef VulkanWrapper<VulkanWriteDescriptorSetData, VkWriteDescriptorSet>
    VulkanWriteDescriptorSet;

struct VulkanCopyDescriptorSetData {
    VkDescriptorSet    src_set;
    uint32_t           src_binding;
    uint32_t           src_array_element;
    VkDescriptorSet    dst_set;
    uint32_t           dst_binding;
    uint32_t           dst_array_element;
    uint32_t           descriptor_count;

    inline operator const VkCopyDescriptorSet() {
        return {
            .sType           = VK_STRUCTURE_TYPE_COPY_DESCRIPTOR_SET,
            .pNext           = nullptr,
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
typedef VulkanWrapper<VulkanCopyDescriptorSetData, VkCopyDescriptorSet>
    VulkanCopyDescriptorSet;