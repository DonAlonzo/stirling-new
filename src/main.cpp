#include "deleter.hpp"
#include "vulkan_assert.hpp"

#include <vulkan/vulkan.h>

#include <GLFW/glfw3.h>

#include <algorithm>
#include <vector>

int main() {
    const int width = 800;
    const int height = 600;

    // Create window
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    //glfwWindowHint(GLFW_DECORATED, GLFW_FALSE);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    auto window = glfwCreateWindow(width, height, "Stirling Engine", nullptr/*glfwGetPrimaryMonitor()*/, nullptr);
    glfwSetWindowPos(window, 0, 0);
    //glfwSetWindowUserPointer(window, this);
    //glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // Create application information
    VkApplicationInfo application_info = {};
    application_info.sType              = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    application_info.pApplicationName   = "Hello Triangle";
    application_info.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    application_info.pEngineName        = "No Engine";
    application_info.engineVersion      = VK_MAKE_VERSION(1, 0, 0);
    application_info.apiVersion         = VK_API_VERSION_1_0;

    // Get required extensions for GLFW
    uint32_t required_glfw_extensions_count = 0;
    auto required_glfw_extensions = glfwGetRequiredInstanceExtensions(&required_glfw_extensions_count);

    // Set enabled extensions
    std::vector<const char*> enabled_extensions = {};
    for (uint32_t i = 0; i < required_glfw_extensions_count; ++i)
        enabled_extensions.emplace_back(required_glfw_extensions[i]);
    //enabled_extensions.emplace_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);
    
    // Create instance
    VkInstanceCreateInfo instance_create_info = {};
    instance_create_info.sType                   = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    instance_create_info.pApplicationInfo        = &application_info;
    instance_create_info.ppEnabledExtensionNames = enabled_extensions.data();
    instance_create_info.enabledExtensionCount   = enabled_extensions.size();

    const auto instance = vulkan_create<VkInstance>(
        vkCreateInstance,
        vkDestroyInstance,
        "Failed to create instance.",
        &instance_create_info,
        nullptr
    );

    // Create window surface
    Deleter<VkSurfaceKHR> surface{vkDestroySurfaceKHR, instance};
    vulkan_assert(glfwCreateWindowSurface(instance, window, nullptr, surface.replace()), "Failed to create surface.");

    // Get number of physical devices
    uint32_t physical_device_count = 0;
    vulkan_assert(vkEnumeratePhysicalDevices(instance, &physical_device_count, nullptr), "Failed to get number of physical devices.");
    if (physical_device_count == 0) throw "No physical devices found.";

    // Get physical devices
    std::vector<VkPhysicalDevice> physical_devices{ physical_device_count };
    vulkan_assert(vkEnumeratePhysicalDevices(instance, &physical_device_count, physical_devices.data()), "Failed to get physical devices.");

    // Just pick the first device god damn it
    auto physical_device = physical_devices[0];

    // Get number of queue families on device
    uint32_t queue_family_count = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(physical_device, &queue_family_count, nullptr);

    // Get queue families on device
    std::vector<VkQueueFamilyProperties> queue_families{queue_family_count};
    vkGetPhysicalDeviceQueueFamilyProperties(physical_device, &queue_family_count, queue_families.data());

    // Find queue families
    uint32_t graphics_queue_family = -1;
    uint32_t present_queue_family = -1;
    for (uint32_t i = 0; i < queue_families.size(); ++i) {
        if (queue_families[i].queueCount > 0) {
            // Check if graphics queue
            if (queue_families[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
                graphics_queue_family = i;
            }

            // Check if present queue
            VkBool32 present_support;
            vulkan_assert(vkGetPhysicalDeviceSurfaceSupportKHR(physical_device, i, surface, &present_support), "Failed to get surface present support.");
            if (present_support) present_queue_family = i;
        }
    }
    if (graphics_queue_family == -1 || present_queue_family == -1)
        throw "Failed to find all queue families.";

    // Prioritize queue families
    float queue_priorities[] = { 1.0f };

    // Create graphics device queue
    VkDeviceQueueCreateInfo graphics_queue_create_info = {};
    graphics_queue_create_info.sType            = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    graphics_queue_create_info.queueFamilyIndex = graphics_queue_family;
    graphics_queue_create_info.queueCount       = 1;
    graphics_queue_create_info.pQueuePriorities = queue_priorities;
    
    // Create present device queue
    VkDeviceQueueCreateInfo present_queue_create_info = {};
    present_queue_create_info.sType            = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    present_queue_create_info.queueFamilyIndex = present_queue_family;
    present_queue_create_info.queueCount       = 1;
    present_queue_create_info.pQueuePriorities = queue_priorities;

    const std::vector<VkDeviceQueueCreateInfo> queue_create_infos = {
        graphics_queue_create_info,
        present_queue_create_info
    };

    // Specify physical device features
    VkPhysicalDeviceFeatures device_features = {};
    
    // Create device
    VkDeviceCreateInfo device_create_info = {};
    device_create_info.sType                 = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    device_create_info.pQueueCreateInfos     = queue_create_infos.data(),
    device_create_info.queueCreateInfoCount  = queue_create_infos.size(),
    device_create_info.pEnabledFeatures      = &device_features;
    device_create_info.enabledExtensionCount = 0;
    device_create_info.enabledLayerCount     = 0;

    const auto device = vulkan_create<VkDevice>(
        vkCreateDevice,
        vkDestroyDevice,
        "Failed to create device.",
        physical_device,
        &device_create_info,
        nullptr
    );

    // Retrieve queue handles
    VkQueue graphics_queue;
    VkQueue present_queue;
    vkGetDeviceQueue(device, graphics_queue_family, 0, &graphics_queue);
    vkGetDeviceQueue(device, present_queue_family, 0, &present_queue);
    
    // Get supported surface format count
    uint32_t surface_format_count;
    vkGetPhysicalDeviceSurfaceFormatsKHR(physical_device, surface, &surface_format_count, nullptr);

    // Get supported surface formats
    std::vector<VkSurfaceFormatKHR> surface_formats;
    if (surface_format_count != 0) {
        surface_formats.resize(surface_format_count);
        vkGetPhysicalDeviceSurfaceFormatsKHR(physical_device, surface, &surface_format_count, surface_formats.data());
    }

    // Get supported presentation mode count
    uint32_t present_mode_count;
    vkGetPhysicalDeviceSurfacePresentModesKHR(physical_device, surface, &present_mode_count, nullptr);

    // Get supported presentation modes
    std::vector<VkPresentModeKHR> present_modes;
    if (present_mode_count != 0) {
        present_modes.resize(present_mode_count);
        vkGetPhysicalDeviceSurfacePresentModesKHR(physical_device, surface, &present_mode_count, present_modes.data());
    }

    // Get surface capabilities
    VkSurfaceCapabilitiesKHR surface_capabilities;
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physical_device, surface, &surface_capabilities);

    // Get swap images count
    uint32_t swap_image_count = surface_capabilities.minImageCount + 1;
    if (surface_capabilities.maxImageCount > 0) {
        swap_image_count = std::min(swap_image_count, surface_capabilities.maxImageCount);
    }

    // Get swap surface format
    auto surface_format = [&]() -> VkSurfaceFormatKHR {
        if (surface_formats.size() == 1 && surface_formats[0].format == VK_FORMAT_UNDEFINED) {
            return { VK_FORMAT_B8G8R8A8_UNORM, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR };
        }

        for (const auto& surface_format : surface_formats) {
            if (surface_format.format == VK_FORMAT_B8G8R8A8_UNORM && surface_format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
                return surface_format;
            }
        }

        return surface_formats[0];
    }();

    // Get swap surface extent
    VkExtent2D surface_extent;
    if (surface_capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
        surface_extent = surface_capabilities.currentExtent;
    } else {
        surface_extent = { width, height};
        surface_extent.width = std::max(surface_capabilities.minImageExtent.width, std::min(surface_capabilities.maxImageExtent.width, surface_extent.width));
        surface_extent.height = std::max(surface_capabilities.minImageExtent.height, std::min(surface_capabilities.maxImageExtent.height, surface_extent.height));
    }

    // Create swap chain
    uint32_t queue_family_indices[] = { graphics_queue_family, present_queue_family };
    VkSwapchainCreateInfoKHR swapchain_create_info = {};
    swapchain_create_info.sType            = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    swapchain_create_info.surface          = surface;
    swapchain_create_info.minImageCount    = swap_image_count;
    swapchain_create_info.imageFormat      = surface_format.format;
    swapchain_create_info.imageColorSpace  = surface_format.colorSpace;
    swapchain_create_info.imageExtent      = surface_extent;
    swapchain_create_info.imageArrayLayers = 1;
    swapchain_create_info.imageUsage       = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    if (graphics_queue_family != present_queue_family) {
        swapchain_create_info.imageSharingMode      = VK_SHARING_MODE_CONCURRENT;
        swapchain_create_info.queueFamilyIndexCount = 2;
        swapchain_create_info.pQueueFamilyIndices   = queue_family_indices;
    } else {
        swapchain_create_info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    }

    const auto swapchain = vulkan_create<VkSwapchainKHR>(
        vkCreateSwapchainKHR,
        vkDestroySwapchainKHR,
        device,
        "Failed to create swapchain.",
        &swapchain_create_info,
        nullptr
    );

    // Get swap chain image count
    vkGetSwapchainImagesKHR(device, swapchain, &swap_image_count, nullptr);

    // Get swap chain images
    std::vector<VkImage> swapchain_images{swap_image_count};
    vkGetSwapchainImagesKHR(device, swapchain, &swap_image_count, swapchain_images.data());

    // Presentation > Image views

    // Loop
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
    }
    return 0;
}
