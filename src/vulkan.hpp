#pragma once

#include <GLFW/glfw3.h>

#include <memory>
#include <vector>

template<typename T>
struct Wrapper {
    Wrapper(T t) : t (t) {}
    operator T*() { return &t; }
private:
    T t;
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
    vulkan_assert(create(args..., handle.replace()), assertion_message);
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
    vulkan_assert(create(parent, args..., handle.replace()), assertion_message);
    return handle;
}

inline Deleter<VkInstance> vulkan_create_instance(const VkInstanceCreateInfo& create_info) {
    return vulkan_create<VkInstance>(
        vkCreateInstance,
        vkDestroyInstance,
        "Failed to create instance.",
        &create_info,
        nullptr
    );
}

inline Deleter<VkSurfaceKHR> vulkan_create_surface(VkInstance instance, GLFWwindow* window) {
    return vulkan_create<VkSurfaceKHR>(
        glfwCreateWindowSurface,
        vkDestroySurfaceKHR,
        instance,
        "Failed to create surface.",
        window,
        nullptr
    );
}

inline Deleter<VkDevice> vulkan_create_device(const VkDeviceCreateInfo& create_info, VkPhysicalDevice physical_device) {
    return vulkan_create<VkDevice>(
        vkCreateDevice,
        vkDestroyDevice,
        "Failed to create device.",
        physical_device,
        &create_info,
        nullptr
    );
}

inline Deleter<VkSwapchainKHR> vulkan_create_swapchain(const VkSwapchainCreateInfoKHR& create_info, VkDevice device) {
    return vulkan_create<VkSwapchainKHR>(
        vkCreateSwapchainKHR,
        vkDestroySwapchainKHR,
        device,
        "Failed to create swapchain.",
        &create_info,
        nullptr
    );
}

inline Deleter<VkImageView> vulkan_create_image_view(const VkImageViewCreateInfo& create_info, VkDevice device) {
    return vulkan_create<VkImageView>(
        vkCreateImageView,
        vkDestroyImageView,
        device,
        "Failed to create image view.",
        &create_info,
        nullptr
    );
}

inline Deleter<VkShaderModule> vulkan_create_shader_module(const VkShaderModuleCreateInfo& create_info, VkDevice device) {
    return vulkan_create<VkShaderModule>(
        vkCreateShaderModule,
        vkDestroyShaderModule,
        device,
        "Failed to create shader module.",
        &create_info,
        nullptr
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

inline Deleter<VkRenderPass> vulkan_create_render_pass(const VkRenderPassCreateInfo& create_info, VkDevice device) {
    return vulkan_create<VkRenderPass>(
        vkCreateRenderPass,
        vkDestroyRenderPass,
        device,
        "Failed to create render pass.",
        &create_info,
        nullptr
    );
}

inline Deleter<VkPipelineLayout> vulkan_create_pipeline_layout(const VkPipelineLayoutCreateInfo& create_info, VkDevice device) {
    return vulkan_create<VkPipelineLayout>(
        vkCreatePipelineLayout,
        vkDestroyPipelineLayout,
        device,
        "Failed to create pipeline layout.",
        &create_info,
        nullptr
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

inline std::vector<VkQueueFamilyProperties> vulkan_get_queue_families(VkPhysicalDevice physical_device) {
    // Get number of queue families on physical device
    uint32_t queue_family_count;
    vkGetPhysicalDeviceQueueFamilyProperties(physical_device, &queue_family_count, nullptr);

    // Get queue families on physical device
    std::vector<VkQueueFamilyProperties> queue_families{queue_family_count};
    vkGetPhysicalDeviceQueueFamilyProperties(physical_device, &queue_family_count, queue_families.data());
    return queue_families;
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