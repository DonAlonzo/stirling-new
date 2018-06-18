#pragma once

#include <GLFW/glfw3.h>

#include <cassert>
#include <memory>
#include <vector>

template<typename T>
struct Wrapper {
    Wrapper(T t) : t (t) {}
    operator const T*() { return &t; }
private:
    T t;
};

template<typename T, typename F>
std::vector<T> cast_vector(const std::vector<F>& from) {
    return std::vector<T>{from.begin(), from.end()};
}

struct QueueFamilyIndices {
    uint32_t graphics_queue = -1;
    uint32_t present_queue = -1;
};

struct VulkanApplicationInfo {
    const char* application_name;
    uint32_t    application_version;
    const char* engine_name;
    uint32_t    engine_version;
    uint32_t    api_version;

    operator const VkApplicationInfo*() const {
        static VkApplicationInfo application_info;
        application_info = {
            .sType              = VK_STRUCTURE_TYPE_APPLICATION_INFO,
            .pNext              = nullptr,
            .pApplicationName   = application_name,
            .applicationVersion = application_version,
            .pEngineName        = engine_name,
            .engineVersion      = engine_version,
            .apiVersion         = api_version
        };
        return &application_info;
    }
};

struct VulkanInstanceCreateInfo {
    VulkanApplicationInfo      application_info;
    std::vector<const char*>   enabled_layers;
    std::vector<const char*>   enabled_extensions;

    operator const VkInstanceCreateInfo*() const {
        static VkInstanceCreateInfo create_info;
        create_info = {
            .sType                   = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
            .pNext                   = nullptr,
            .flags                   = 0,
            .pApplicationInfo        = application_info,
            .enabledLayerCount       = enabled_layers.size(),
            .ppEnabledLayerNames     = enabled_layers.data(),
            .enabledExtensionCount   = enabled_extensions.size(),
            .ppEnabledExtensionNames = enabled_extensions.data()
        };
        return &create_info;
    }
};

struct VulkanDebugReportCallbackCreateInfoEXT {
    VkDebugReportFlagsEXT        flags;
    PFN_vkDebugReportCallbackEXT callback;
    void*                        user_data;

    operator const VkDebugReportCallbackCreateInfoEXT*() const {
        static VkDebugReportCallbackCreateInfoEXT create_info;
        create_info = {
            .sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CALLBACK_CREATE_INFO_EXT,
            .pNext = nullptr,
            .flags = flags,
            .pfnCallback = callback,
            .pUserData = nullptr
        };
        return &create_info;
    }
};

struct VulkanDeviceQueueCreateInfo {
    uint32_t           queue_family_index;
    std::vector<float> queue_priorities;

    operator const VkDeviceQueueCreateInfo*() const {
        static VkDeviceQueueCreateInfo create_info;
        create_info = {
            .sType            = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
            .pNext            = nullptr,
            .flags            = 0,
            .queueFamilyIndex = queue_family_index,
            .queueCount       = queue_priorities.size(),
            .pQueuePriorities = queue_priorities.data()
        };
        return &create_info;
    }
};

struct VulkanDeviceCreateInfo {
    std::vector<VulkanDeviceQueueCreateInfo> queues;
    std::vector<const char*>                 enabled_layers;
    std::vector<const char*>                 enabled_extensions;
    VkPhysicalDeviceFeatures                 enabled_features;
    
    operator const VkDeviceCreateInfo*() const {
        static VkDeviceCreateInfo create_info;
        create_info = {
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
        return &create_info;
    }
};

struct VulkanSwapchainCreateInfoKHR {
    VkSurfaceKHR                     surface;
    uint32_t                         min_image_count;
    VkFormat                         image_format;
    VkColorSpaceKHR                  image_color_space;
    VkExtent2D                       image_extent;
    uint32_t                         image_array_layers;
    VkImageUsageFlags                image_usage;
    VkSharingMode                    image_sharing_mode;
    std::vector<uint32_t>            queue_family_indices;
    VkSurfaceTransformFlagBitsKHR    pre_transform;
    VkCompositeAlphaFlagBitsKHR      composite_alpha;
    VkPresentModeKHR                 present_mode;
    VkBool32                         clipped;
    VkSwapchainKHR                   old_swapchain;

    operator const VkSwapchainCreateInfoKHR*() const {
        static VkSwapchainCreateInfoKHR create_info;
        create_info = {
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
        return &create_info;
    }
};

struct VulkanImageViewCreateInfo {
    VkImage                 image;
    VkImageViewType         view_type;
    VkFormat                format;
    VkComponentMapping      components;
    VkImageSubresourceRange subresource_range;

    operator const VkImageViewCreateInfo*() const {
        static VkImageViewCreateInfo create_info;
        create_info = {
            .sType            = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
            .pNext            = nullptr,
            .flags            = 0,
            .image            = image,
            .viewType         = view_type,
            .format           = format,
            .components       = components,
            .subresourceRange = subresource_range
        };
        return &create_info;
    }
};

struct VulkanAttachmentDescription {
    VkAttachmentDescriptionFlags flags;
    VkFormat                     format;
    VkSampleCountFlagBits        samples;
    VkAttachmentLoadOp           load_op;
    VkAttachmentStoreOp          store_op;
    VkAttachmentLoadOp           stencil_load_op;
    VkAttachmentStoreOp          stencil_store_op;
    VkImageLayout                initial_layout;
    VkImageLayout                final_layout;

    operator const VkAttachmentDescription*() const {
        static VkAttachmentDescription description;
        description = {
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
        return &description;
    }
};

struct VulkanSubpassDescription {
    VkSubpassDescriptionFlags          flags;
    VkPipelineBindPoint                pipeline_bind_point;
    std::vector<VkAttachmentReference> input_attachments;
    std::vector<VkAttachmentReference> color_attachments;
    std::vector<VkAttachmentReference> resolve_attachments;
    VkAttachmentReference              depth_stencil_attachment;
    std::vector<uint32_t>              preserve_attachments;

    operator const VkSubpassDescription*() const {
        static VkSubpassDescription description;
        description = {
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
        return &description;
    }
};

struct VulkanSubpassDependency {
    uint32_t             src_subpass;
    uint32_t             dst_subpass;
    VkPipelineStageFlags src_stage_mask;
    VkPipelineStageFlags dst_stage_mask;
    VkAccessFlags        src_access_mask;
    VkAccessFlags        dst_access_mask;
    VkDependencyFlags    dependency_flags;

    operator const VkSubpassDependency*() const {
        static VkSubpassDependency description;
        description = {
            .srcSubpass      = src_subpass,
            .dstSubpass      = dst_subpass,
            .srcStageMask    = src_stage_mask,
            .dstStageMask    = dst_stage_mask,
            .srcAccessMask   = src_access_mask,
            .dstAccessMask   = dst_access_mask,
            .dependencyFlags = dependency_flags
        };
        return &description;
    }
};


struct VulkanRenderPassCreateInfo {
    std::vector<VulkanAttachmentDescription> attachments;
    std::vector<VulkanSubpassDescription>    subpasses;
    std::vector<VulkanSubpassDependency>     dependencies;
    
    operator const VkRenderPassCreateInfo*() const {
        static VkRenderPassCreateInfo create_info;
        create_info = {
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
        return &create_info;
    }
};

struct VulkanFramebufferCreateInfo {
    VkRenderPass             render_pass;
    std::vector<VkImageView> attachments;
    uint32_t                 width;
    uint32_t                 height;
    uint32_t                 layers;
    
    operator const VkFramebufferCreateInfo*() const {
        static VkFramebufferCreateInfo create_info;
        create_info = {
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
        return &create_info;
    }
};

struct VulkanCommandPoolCreateInfo {
    VkCommandPoolCreateFlags flags;
    uint32_t                 queue_family_index;

    operator const VkCommandPoolCreateInfo*() const {
        static VkCommandPoolCreateInfo create_info;
        create_info = {
            .sType            = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
            .pNext            = nullptr,
            .flags            = flags,
            .queueFamilyIndex = queue_family_index
        };
        return &create_info;
    }
};

struct VulkanCommandBufferAllocateInfo {
    VkCommandPool           command_pool;
    VkCommandBufferLevel    level;
    uint32_t                command_buffer_count;

    operator const VkCommandBufferAllocateInfo*() const {
        static VkCommandBufferAllocateInfo create_info;
        create_info = {
            .sType              = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
            .pNext              = nullptr,
            .commandPool        = command_pool,
            .level              = level,
            .commandBufferCount = command_buffer_count
        };
        return &create_info;
    }
};

struct VulkanRenderPassBeginInfo {
    VkRenderPass              render_pass;
    VkFramebuffer             framebuffer;
    VkRect2D                  render_area;
    std::vector<VkClearValue> clear_values;

    operator const VkRenderPassBeginInfo*() const {
        static VkRenderPassBeginInfo begin_info;
        begin_info = {
            .sType           = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
            .pNext           = nullptr,
            .renderPass      = render_pass,
            .framebuffer     = framebuffer,
            .renderArea      = render_area,
            .clearValueCount = clear_values.size(),
            .pClearValues    = clear_values.data()
        };
        return &begin_info;
    }
};

struct VulkanCommandBufferInheritanceInfo {
    VkRenderPass                   render_pass;
    uint32_t                       subpass;
    VkFramebuffer                  framebuffer;
    VkBool32                       occlusion_query_enable;
    VkQueryControlFlags            query_flags;
    VkQueryPipelineStatisticFlags  pipeline_statistics;

    operator const VkCommandBufferInheritanceInfo*() const {
        static VkCommandBufferInheritanceInfo inheritance_info;
        inheritance_info = {
            .sType                = VK_STRUCTURE_TYPE_COMMAND_BUFFER_INHERITANCE_INFO,
            .pNext                = nullptr,
            .renderPass           = render_pass,
            .subpass              = subpass,
            .framebuffer          = framebuffer,
            .occlusionQueryEnable = occlusion_query_enable,
            .queryFlags           = query_flags,
            .pipelineStatistics   = pipeline_statistics
        };
        return &inheritance_info;
    }
};

struct VulkanCommandBufferBeginInfo {
    VkCommandBufferUsageFlags          flags;
    VulkanCommandBufferInheritanceInfo inheritance_info;

    operator const VkCommandBufferBeginInfo*() const {
        static VkCommandBufferBeginInfo begin_info;
        begin_info = {
            .sType            = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
            .pNext            = nullptr,
            .flags            = flags,
            .pInheritanceInfo = inheritance_info
        };
        return &begin_info;
    }
};

struct VulkanSubmitInfo {
    std::vector<VkSemaphore>     wait_semaphores;
    VkPipelineStageFlags         wait_dst_stage_mask;
    std::vector<VkCommandBuffer> command_buffers;
    std::vector<VkSemaphore>     signal_semaphores;

    operator const VkSubmitInfo*() const {
        static VkSubmitInfo submit_info;
        submit_info = {
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
        return &submit_info;
    }
};

struct VulkanPresentInfoKHR {
    std::vector<VkSemaphore>    wait_semaphores;
    std::vector<VkSwapchainKHR> swapchains;
    std::vector<uint32_t>       image_indices;

    operator const VkPresentInfoKHR*() const {
        static VkPresentInfoKHR present_info;
        assert(image_indices.size() == swapchains.size());
        present_info = {
            .sType              = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
            .pNext              = nullptr,
            .waitSemaphoreCount = wait_semaphores.size(),
            .pWaitSemaphores    = wait_semaphores.data(),
            .swapchainCount     = swapchains.size(),
            .pSwapchains        = swapchains.data(),
            .pImageIndices      = image_indices.data(),
            .pResults           = nullptr
        };
        return &present_info;
    }
};

inline void vulkan_assert(VkResult result, const char* assertion_message) {
    switch (result) {
    case VK_SUCCESS: return;
    default: throw assertion_message;
    }
}

template<typename Handle, typename Create, typename Destroy, typename... Args>
inline Deleter<Handle> vulkan_create(
    Create      create,
    Destroy     destroy,
    const char* assertion_message,
    Args...     args) {

    Deleter<Handle> handle{destroy};
    vulkan_assert(create(args..., nullptr, handle.replace()), assertion_message);
    return handle;
}

template<typename Handle, typename Create, typename Destroy, typename Parent, typename... Args>
inline Deleter<Handle> vulkan_create(
    Create      create,
    Destroy     destroy,
    Parent      parent,
    const char* assertion_message,
    Args...     args) {

    Deleter<Handle> handle{destroy, parent};
    vulkan_assert(create(parent, args..., nullptr, handle.replace()), assertion_message);
    return handle;
}

inline Deleter<VkInstance> vulkan_create_instance(const VulkanInstanceCreateInfo& create_info) {
    return vulkan_create<VkInstance>(
        vkCreateInstance,
        vkDestroyInstance,
        "Failed to create instance.",
        create_info
    );
}

inline Deleter<VkDebugReportCallbackEXT> vulkan_create_debug_report_callback(
    const VulkanDebugReportCallbackCreateInfoEXT& create_info,
    VkInstance                                    instance) {

    const auto create = reinterpret_cast<PFN_vkCreateDebugReportCallbackEXT>(vkGetInstanceProcAddr(instance, "vkCreateDebugReportCallbackEXT"));
    const auto destroy = reinterpret_cast<PFN_vkDestroyDebugReportCallbackEXT>(vkGetInstanceProcAddr(instance, "vkDestroyDebugReportCallbackEXT"));
    if (create == nullptr || destroy == nullptr) throw "Debug report callback extension not present.";
    return vulkan_create<VkDebugReportCallbackEXT>(
        create,
        destroy,
        instance,
        "Failed to create debug report callback.",
        create_info
    );
}

inline Deleter<VkSurfaceKHR> vulkan_create_surface(VkInstance instance, GLFWwindow* window) {
    return vulkan_create<VkSurfaceKHR>(
        glfwCreateWindowSurface,
        vkDestroySurfaceKHR,
        instance,
        "Failed to create surface.",
        window
    );
}

inline Deleter<VkDevice> vulkan_create_device(const VulkanDeviceCreateInfo& create_info, VkPhysicalDevice physical_device) {
    return vulkan_create<VkDevice>(
        vkCreateDevice,
        vkDestroyDevice,
        "Failed to create device.",
        physical_device,
        create_info
    );
}

inline Deleter<VkSwapchainKHR> vulkan_create_swapchain(const VulkanSwapchainCreateInfoKHR& create_info, VkDevice device) {
    return vulkan_create<VkSwapchainKHR>(
        vkCreateSwapchainKHR,
        vkDestroySwapchainKHR,
        device,
        "Failed to create swapchain.",
        create_info
    );
}

inline Deleter<VkImageView> vulkan_create_image_view(const VulkanImageViewCreateInfo& create_info, VkDevice device) {
    return vulkan_create<VkImageView>(
        vkCreateImageView,
        vkDestroyImageView,
        device,
        "Failed to create image view.",
        create_info
    );
}

inline Deleter<VkShaderModule> vulkan_create_shader_module(const VkShaderModuleCreateInfo& create_info, VkDevice device) {
    return vulkan_create<VkShaderModule>(
        vkCreateShaderModule,
        vkDestroyShaderModule,
        device,
        "Failed to create shader module.",
        &create_info
    );
}

inline Deleter<VkShaderModule> vulkan_create_shader_module(const char* file_name, VkDevice device) {
    const auto code = read_file(file_name);
    return vulkan_create_shader_module({
        .sType    = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
        .pNext    = nullptr,
        .flags    = 0,
        .codeSize = code.size(),
        .pCode    = reinterpret_cast<const uint32_t*>(code.data())
    }, device);
}

inline Deleter<VkRenderPass> vulkan_create_render_pass(const VulkanRenderPassCreateInfo& create_info, VkDevice device) {
    return vulkan_create<VkRenderPass>(
        vkCreateRenderPass,
        vkDestroyRenderPass,
        device,
        "Failed to create render pass.",
        create_info
    );
}

inline Deleter<VkPipelineLayout> vulkan_create_pipeline_layout(const VkPipelineLayoutCreateInfo& create_info, VkDevice device) {
    return vulkan_create<VkPipelineLayout>(
        vkCreatePipelineLayout,
        vkDestroyPipelineLayout,
        device,
        "Failed to create pipeline layout.",
        &create_info
    );
}

inline std::vector<Deleter<VkPipeline>> vulkan_create_pipelines(
    const std::vector<VkGraphicsPipelineCreateInfo>& create_infos,
    VkPipelineCache                                  pipeline_cache,
    VkDevice                                         device) {

    VkPipeline* pipeline_pointer;
    vulkan_assert(
        vkCreateGraphicsPipelines(device, pipeline_cache, create_infos.size(), create_infos.data(), nullptr, pipeline_pointer),
        "Failed to create pipelines."
    );

    std::vector<Deleter<VkPipeline>> pipelines{create_infos.size()};
    for (size_t i = 0; i < create_infos.size(); ++i) {
        pipelines[i].replace(*(pipeline_pointer + i));
    }

    return pipelines;
}

inline Deleter<VkPipeline> vulkan_create_pipeline(
    const VkGraphicsPipelineCreateInfo& create_info,
    VkPipelineCache                     pipeline_cache,
    VkDevice                            device) {

    return vulkan_create<VkPipeline>(
        vkCreateGraphicsPipelines,
        vkDestroyPipeline,
        device,
        "Failed to create pipeline.",
        pipeline_cache,
        1,
        &create_info
    );
}

inline Deleter<VkFramebuffer> vulkan_create_framebuffer(const VulkanFramebufferCreateInfo& create_info, VkDevice device) {
    return vulkan_create<VkFramebuffer>(
        vkCreateFramebuffer,
        vkDestroyFramebuffer,
        device,
        "Failed to create framebuffer.",
        create_info
    );
}

inline Deleter<VkSemaphore> vulkan_create_semaphore(const VkSemaphoreCreateInfo& create_info, VkDevice device) {
    return vulkan_create<VkSemaphore>(
        vkCreateSemaphore,
        vkDestroySemaphore,
        device,
        "Failed to create semaphore.",
        &create_info
    );
}

inline Deleter<VkSemaphore> vulkan_create_semaphore(VkDevice device) {
    return vulkan_create_semaphore({
        .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0
    }, device);
}

inline std::vector<Deleter<VkSemaphore>> vulkan_create_semaphores(VkDevice device, size_t count) {
    std::vector<Deleter<VkSemaphore>> semaphores{count};
    for (size_t i = 0; i < count; ++i) {
        semaphores[i] = vulkan_create_semaphore(device);
    }
    return semaphores;
}

inline Deleter<VkFence> vulkan_create_fence(const VkFenceCreateInfo& create_info, VkDevice device) {
    return vulkan_create<VkFence>(
        vkCreateFence,
        vkDestroyFence,
        device,
        "Failed to create fence.",
        &create_info
    );
}

inline Deleter<VkFence> vulkan_create_fence(VkDevice device, bool signaled = false) {
    return vulkan_create_fence({
        .sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
        .pNext = nullptr,
        .flags = signaled ? VK_FENCE_CREATE_SIGNALED_BIT : 0
    }, device);
}

inline std::vector<Deleter<VkFence>> vulkan_create_fences(VkDevice device, size_t count, bool signaled = false) {
    std::vector<Deleter<VkFence>> fences{count};
    for (size_t i = 0; i < count; ++i) {
        fences[i] = vulkan_create_fence(device, signaled);
    }
    return fences;
}

inline Deleter<VkCommandPool> vulkan_create_command_pool(const VulkanCommandPoolCreateInfo& create_info, VkDevice device) {
    return vulkan_create<VkCommandPool>(
        vkCreateCommandPool,
        vkDestroyCommandPool,
        device,
        "Failed to create command pool.",
        create_info
    );
}

inline std::vector<VkCommandBuffer> vulkan_allocate_command_buffers(const VulkanCommandBufferAllocateInfo& allocate_info, VkDevice device) {
    std::vector<VkCommandBuffer> command_buffers{allocate_info.command_buffer_count};
    vulkan_assert(
        vkAllocateCommandBuffers(device, allocate_info, command_buffers.data()),
        "Failed to allocate command buffers."
    );
    return command_buffers;
}

inline void vulkan_begin_command_buffer(const VulkanCommandBufferBeginInfo& begin_info, VkCommandBuffer command_buffer) {
    vulkan_assert(
        vkBeginCommandBuffer(command_buffer, begin_info),
        "Failed to begin command buffer."
    );
}

inline void vulkan_begin_render_pass(const VulkanRenderPassBeginInfo& begin_info, VkCommandBuffer command_buffer, VkSubpassContents contents) {
    vkCmdBeginRenderPass(command_buffer, begin_info, contents);
}

inline void vulkan_wait_for_fence(
    VkDevice device,
    std::vector<VkFence> fences,
    VkBool32 wait_all = VK_TRUE,
    uint64_t timeout = std::numeric_limits<uint64_t>::max()) {

    vulkan_assert(
        vkWaitForFences(device, fences.size(), fences.data(), wait_all, timeout),
        "Failed to wait for fences."
    );
}

inline void vulkan_wait_for_fence(
    VkDevice device,
    VkFence fence,
    VkBool32 wait_all = VK_TRUE,
    uint64_t timeout = std::numeric_limits<uint64_t>::max()) {

    vulkan_assert(
        vkWaitForFences(device, 1, &fence, wait_all, timeout),
        "Failed to wait for fence."
    );
}

inline void vulkan_reset_fence(VkDevice device, std::vector<VkFence> fences) {
    vulkan_assert(
        vkResetFences(device, fences.size(), fences.data()),
        "Failed to reset fences."
    );
}

inline void vulkan_reset_fence(VkDevice device, VkFence fence) {
    vulkan_assert(
        vkResetFences(device, 1, &fence),
        "Failed to reset fence."
    );
}

inline uint32_t vulkan_acquire_next_image(
    VkDevice       device,
    VkSwapchainKHR swapchain,
    VkSemaphore    semaphore = VK_NULL_HANDLE,
    VkFence        fence = VK_NULL_HANDLE,
    uint64_t       timeout = std::numeric_limits<uint64_t>::max()) {

    uint32_t image_index;
    vkAcquireNextImageKHR(device, swapchain, timeout, semaphore, fence, &image_index);
    return image_index;
}

inline void vulkan_queue_submit(const std::vector<VulkanSubmitInfo>& submit_infos, VkQueue queue, VkFence fence) {
    vulkan_assert(
        vkQueueSubmit(
            queue,
            submit_infos.size(),
            cast_vector<const VkSubmitInfo*>(submit_infos)[0],
            fence
        ),
        "Failed to submit to queue."
    );
}

inline void vulkan_queue_present(const VulkanPresentInfoKHR& present_info, VkQueue queue) {
    vulkan_assert(vkQueuePresentKHR(queue, present_info), "Failed to present to queue.");
}

inline std::vector<VkPhysicalDevice> vulkan_get_physical_devices(VkInstance instance) {
    // Get number of physical devices
    uint32_t physical_device_count = 0;
    vulkan_assert(
        vkEnumeratePhysicalDevices(instance, &physical_device_count, nullptr),
        "Failed to get number of physical devices."
    );

    // Get physical devices
    std::vector<VkPhysicalDevice> physical_devices{physical_device_count};
    vulkan_assert(
        vkEnumeratePhysicalDevices(instance, &physical_device_count, physical_devices.data()),
        "Failed to get physical devices."
    );
    return physical_devices;
}

inline VkPhysicalDeviceProperties vulkan_get_physical_device_properties(VkPhysicalDevice physical_device) {
    VkPhysicalDeviceProperties properties;
    vkGetPhysicalDeviceProperties(physical_device, &properties);
    return properties;
}

inline VkPhysicalDeviceFeatures vulkan_get_physical_device_features(VkPhysicalDevice physical_device) {
    VkPhysicalDeviceFeatures features;
    vkGetPhysicalDeviceFeatures(physical_device, &features);
    return features;
}

inline VkBool32 vulkan_get_surface_present_support(VkPhysicalDevice physical_device, uint32_t queue_family_index, VkSurfaceKHR surface) {
    VkBool32 present_support;
    vulkan_assert(
        vkGetPhysicalDeviceSurfaceSupportKHR(physical_device, queue_family_index, surface, &present_support),
        "Failed to get surface present support."
    );
    return present_support;
}

inline std::vector<VkSurfaceFormatKHR> vulkan_get_surface_formats(VkPhysicalDevice physical_device, VkSurfaceKHR surface) {
    // Get supported surface format count
    uint32_t surface_format_count;
    vulkan_assert(
        vkGetPhysicalDeviceSurfaceFormatsKHR(physical_device, surface, &surface_format_count, nullptr),
        "Failed to get number of surface formats."
    );

    // Get supported surface formats
    std::vector<VkSurfaceFormatKHR> surface_formats{surface_format_count};
    vulkan_assert(
        vkGetPhysicalDeviceSurfaceFormatsKHR(physical_device, surface, &surface_format_count, surface_formats.data()),
        "Failed to get surface formats."
    );
    return surface_formats;
}

inline std::vector<VkPresentModeKHR> vulkan_get_surface_present_modes(VkPhysicalDevice physical_device, VkSurfaceKHR surface) {
    // Get surface present mode count
    uint32_t present_mode_count;
    vulkan_assert(
        vkGetPhysicalDeviceSurfacePresentModesKHR(physical_device, surface, &present_mode_count, nullptr),
        "Failed to get number of surface present modes."
    );

    // Get surface present modes
    std::vector<VkPresentModeKHR> present_modes{present_mode_count};
    vulkan_assert(
        vkGetPhysicalDeviceSurfacePresentModesKHR(physical_device, surface, &present_mode_count, present_modes.data()),
        "Failed to get surface present modes."
    );
    return present_modes;
}

inline VkSurfaceCapabilitiesKHR vulkan_get_surface_capabilities(VkPhysicalDevice physical_device, VkSurfaceKHR surface) {
    // Get surface capabilities
    VkSurfaceCapabilitiesKHR surface_capabilities;
    vulkan_assert(
        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physical_device, surface, &surface_capabilities),
        "Failed to get surface capabilities."
    );
    return surface_capabilities;
}

inline VkExtent2D vulkan_get_surface_extent(VkSurfaceCapabilitiesKHR surface_capabilities, uint32_t width, uint32_t height) {
    VkExtent2D surface_extent;
    if (surface_capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
        surface_extent = surface_capabilities.currentExtent;
    } else {
        surface_extent.width = std::max(surface_capabilities.minImageExtent.width, std::min(surface_capabilities.maxImageExtent.width, width));
        surface_extent.height = std::max(surface_capabilities.minImageExtent.height, std::min(surface_capabilities.maxImageExtent.height, height));
    }
    return surface_extent;
}

inline std::vector<VkQueueFamilyProperties> vulkan_get_queue_family_properties(VkPhysicalDevice physical_device) {
    // Get number of queue families on physical device
    uint32_t queue_family_property_count;
    vkGetPhysicalDeviceQueueFamilyProperties(physical_device, &queue_family_property_count, nullptr);

    // Get queue families on physical device
    std::vector<VkQueueFamilyProperties> queue_family_properties{queue_family_property_count};
    vkGetPhysicalDeviceQueueFamilyProperties(physical_device, &queue_family_property_count, queue_family_properties.data());
    return queue_family_properties;
}

inline QueueFamilyIndices vulkan_get_queue_families(VkPhysicalDevice physical_device, VkSurfaceKHR surface) {
    const auto queue_family_properties = vulkan_get_queue_family_properties(physical_device);

    // Find queue family indices
    QueueFamilyIndices queue_family_indices;
    for (uint32_t i = 0; i < queue_family_properties.size(); ++i) {
        if (queue_family_properties[i].queueCount > 0) {
            // Check if graphics queue
            if (queue_family_properties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
                queue_family_indices.graphics_queue = i;
            }

            // Check if present queue
            if (vulkan_get_surface_present_support(physical_device, i, surface)) {
                queue_family_indices.present_queue = i;
            }
        }
    }
    if (queue_family_indices.graphics_queue == -1 || queue_family_indices.present_queue == -1) {
        throw "Failed to find all queue families.";
    }

    return queue_family_indices;
}

inline std::vector<VkImage> vulkan_get_swapchain_images(VkDevice device, VkSwapchainKHR swapchain) {
    // Get swapchain image count
    uint32_t swapchain_image_count;
    vulkan_assert(
        vkGetSwapchainImagesKHR(device, swapchain, &swapchain_image_count, nullptr),
        "Failed to get number of swapchain images."
    );

    // Get swapchain images
    std::vector<VkImage> swapchain_images{swapchain_image_count};
    vulkan_assert(
        vkGetSwapchainImagesKHR(device, swapchain, &swapchain_image_count, swapchain_images.data()),
        "Failed to get swapchain images."
    );
    return swapchain_images;
}

inline VkQueue vulkan_get_queue(VkDevice device, uint32_t queue_family, uint32_t queue_index) {
    VkQueue queue;
    vkGetDeviceQueue(device, queue_family, queue_index, &queue);
    return queue;
}