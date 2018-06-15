#pragma once

#include <GLFW/glfw3.h>

#include <memory>
#include <vector>

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

inline std::vector<const char*> vulkan_get_required_instance_extensions() {
    // Get required extensions for GLFW
    uint32_t required_glfw_extensions_count = 0;
    auto required_glfw_extensions = glfwGetRequiredInstanceExtensions(&required_glfw_extensions_count);
    return {required_glfw_extensions, required_glfw_extensions + required_glfw_extensions_count};
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