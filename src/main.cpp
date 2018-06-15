#include "deleter.hpp"
#include "vulkan_assert.hpp"

#include <vulkan/vulkan.h>

#include <GLFW/glfw3.h>

#include <algorithm>
#include <iostream>
#include <vector>

int main() {
    try {
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

        // Get required extensions for GLFW
        uint32_t required_glfw_extensions_count = 0;
        auto required_glfw_extensions = glfwGetRequiredInstanceExtensions(&required_glfw_extensions_count);

        // Set enabled extensions
        std::vector<const char*> enabled_extensions = {};
        for (uint32_t i = 0; i < required_glfw_extensions_count; ++i)
            enabled_extensions.emplace_back(required_glfw_extensions[i]);
        //enabled_extensions.emplace_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);    

        // Create instance
        const auto instance = vulkan_create_instance({
            .sType                   = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
            .pNext                   = nullptr,
            .flags                   = 0,
            .pApplicationInfo        = Wrapper<VkApplicationInfo>{{
                .sType              = VK_STRUCTURE_TYPE_APPLICATION_INFO,
                .pNext              = nullptr,
                .pApplicationName   = "Hello Triangle",
                .applicationVersion = VK_MAKE_VERSION(1, 0, 0),
                .pEngineName        = "No Engine",
                .engineVersion      = VK_MAKE_VERSION(1, 0, 0),
                .apiVersion         = VK_API_VERSION_1_0
            }},
            .enabledLayerCount       = 0,
            .ppEnabledLayerNames     = nullptr,
            .enabledExtensionCount   = enabled_extensions.size(),
            .ppEnabledExtensionNames = enabled_extensions.data()
        });

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

        // Create device queues
        const std::vector<VkDeviceQueueCreateInfo> queue_create_infos = {
            // Graphics queue
            {
                .sType            = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
                .pNext            = nullptr,
                .flags            = 0,
                .queueFamilyIndex = graphics_queue_family,
                .queueCount       = 1,
                .pQueuePriorities = queue_priorities
            },
            // Present queue
            {
                .sType            = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
                .pNext            = nullptr,
                .flags            = 0,
                .queueFamilyIndex = present_queue_family,
                .queueCount       = 1,
                .pQueuePriorities = queue_priorities
            }
        };

        // Specify physical device features
        VkPhysicalDeviceFeatures device_features = {};
        
        // Create device
        const auto device = vulkan_create_device({
            .sType                   = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
            .pNext                   = nullptr,
            .flags                   = 0,
            .queueCreateInfoCount    = queue_create_infos.size(),
            .pQueueCreateInfos       = queue_create_infos.data(),
            .enabledLayerCount       = 0,
            .ppEnabledLayerNames     = nullptr,
            .enabledExtensionCount   = 0,
            .ppEnabledExtensionNames = nullptr,
            .pEnabledFeatures        = &device_features
        }, physical_device);

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

        const bool concurrent = graphics_queue_family != present_queue_family;
        const auto swapchain = vulkan_create_swapchain({
            .sType                 = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
            .pNext                 = nullptr,
            .flags                 = 0,
            .surface               = surface,
            .minImageCount         = swap_image_count,
            .imageFormat           = surface_format.format,
            .imageColorSpace       = surface_format.colorSpace,
            .imageExtent           = surface_extent,
            .imageArrayLayers      = 1,
            .imageUsage            = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
            .imageSharingMode      = concurrent ? VK_SHARING_MODE_CONCURRENT : VK_SHARING_MODE_EXCLUSIVE,
            .queueFamilyIndexCount = concurrent ? 2 : 0,
            .pQueueFamilyIndices   = concurrent ? queue_family_indices : nullptr,
        }, device);

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
    } catch (const char* message) {
        std::cout << message << '\n';
    }
    return 0;
}