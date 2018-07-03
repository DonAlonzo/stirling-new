#pragma once

#include "vulkan_structs.hpp"

template<typename Handle, typename Create, typename Destroy, typename... Args>
inline Deleter<Handle> vulkan_create(
    Create      create,
    Destroy     destroy,
    const char* assertion_message,
    Args&&...   args) {

    Deleter<Handle> handle{destroy};
    vulkan_assert(create(std::forward<Args>(args)..., nullptr, handle.replace()), assertion_message);
    return handle;
}

template<typename Handle, typename Create, typename Destroy, typename Parent, typename... Args>
inline Deleter<Handle> vulkan_create(
    Create      create,
    Destroy     destroy,
    Parent      parent,
    const char* assertion_message,
    Args&&...   args) {

    Deleter<Handle> handle{destroy, parent};
    vulkan_assert(create(parent, std::forward<Args>(args)..., nullptr, handle.replace()), assertion_message);
    return handle;
}

inline Deleter<VkInstance> vulkan_create_instance(VulkanInstanceCreateInfo&& create_info) {
    return vulkan_create<VkInstance>(
        vkCreateInstance,
        vkDestroyInstance,
        "Failed to create instance.",
        create_info
    );
}

inline Deleter<VkDebugReportCallbackEXT> vulkan_create_debug_report_callback(
    VulkanDebugReportCallbackCreateInfoEXT&& create_info,
    VkInstance                               instance) {

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

inline Deleter<VkDevice> vulkan_create_device(VulkanDeviceCreateInfo&& create_info, VkPhysicalDevice physical_device) {
    return vulkan_create<VkDevice>(
        vkCreateDevice,
        vkDestroyDevice,
        "Failed to create device.",
        physical_device,
        create_info
    );
}

inline Deleter<VkSwapchainKHR> vulkan_create_swapchain(VulkanSwapchainCreateInfoKHR&& create_info, VkDevice device) {
    return vulkan_create<VkSwapchainKHR>(
        vkCreateSwapchainKHR,
        vkDestroySwapchainKHR,
        device,
        "Failed to create swapchain.",
        create_info
    );
}

inline Deleter<VkImageView> vulkan_create_image_view(VulkanImageViewCreateInfo&& create_info, VkDevice device) {
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

inline Deleter<VkRenderPass> vulkan_create_render_pass(VulkanRenderPassCreateInfo&& create_info, VkDevice device) {
    return vulkan_create<VkRenderPass>(
        vkCreateRenderPass,
        vkDestroyRenderPass,
        device,
        "Failed to create render pass.",
        create_info
    );
}

inline Deleter<VkPipelineLayout> vulkan_create_pipeline_layout(VulkanPipelineLayoutCreateInfo&& create_info, VkDevice device) {
    return vulkan_create<VkPipelineLayout>(
        vkCreatePipelineLayout,
        vkDestroyPipelineLayout,
        device,
        "Failed to create pipeline layout.",
        create_info
    );
}

inline std::vector<Deleter<VkPipeline>> vulkan_create_pipelines(
    const std::vector<VulkanGraphicsPipelineCreateInfo>& create_infos,
    VkPipelineCache                                      pipeline_cache,
    VkDevice                                             device) {

    VkPipeline* pipeline_pointer;
    vulkan_assert(
        vkCreateGraphicsPipelines(
            device,
            pipeline_cache,
            create_infos.size(),
            cast_vector<const VkGraphicsPipelineCreateInfo*>(create_infos)[0],
            nullptr,
            pipeline_pointer
        ),
        "Failed to create pipelines."
    );

    std::vector<Deleter<VkPipeline>> pipelines{create_infos.size()};
    for (size_t i = 0; i < create_infos.size(); ++i) {
        pipelines[i].replace(*(pipeline_pointer + i));
    }

    return pipelines;
}

inline Deleter<VkPipeline> vulkan_create_pipeline(
    const VulkanGraphicsPipelineCreateInfo& create_info,
    VkPipelineCache                         pipeline_cache,
    VkDevice                                device) {

    return vulkan_create<VkPipeline>(
        vkCreateGraphicsPipelines,
        vkDestroyPipeline,
        device,
        "Failed to create pipeline.",
        pipeline_cache,
        1,
        create_info
    );
}

inline Deleter<VkFramebuffer> vulkan_create_framebuffer(VulkanFramebufferCreateInfo&& create_info, VkDevice device) {
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

inline Deleter<VkCommandPool> vulkan_create_command_pool(VulkanCommandPoolCreateInfo&& create_info, VkDevice device) {
    return vulkan_create<VkCommandPool>(
        vkCreateCommandPool,
        vkDestroyCommandPool,
        device,
        "Failed to create command pool.",
        create_info
    );
}
