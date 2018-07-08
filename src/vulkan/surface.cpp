#include "surface.hpp"
#include "vulkan.hpp"

namespace stirling { namespace vulkan {

    Surface::Surface(Deleter<VkSurfaceKHR>&& surface) :
        surface (surface) {
    }

    VkBool32 Surface::get_present_support(
        VkPhysicalDevice physical_device,
        uint32_t         queue_family_index) const {

        VkBool32 present_support;
        vulkan_assert(
            vkGetPhysicalDeviceSurfaceSupportKHR(physical_device, queue_family_index, surface, &present_support),
            "Failed to get surface present support."
        );
        return present_support;
    }

    std::vector<VkSurfaceFormatKHR> Surface::get_formats(
        VkPhysicalDevice physical_device) const {

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

    std::vector<VkPresentModeKHR> Surface::get_present_modes(
        VkPhysicalDevice physical_device) const {

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

    VkSurfaceCapabilitiesKHR Surface::get_capabilities(
        VkPhysicalDevice physical_device) const {

        // Get surface capabilities
        VkSurfaceCapabilitiesKHR surface_capabilities;
        vulkan_assert(
            vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physical_device, surface, &surface_capabilities),
            "Failed to get surface capabilities."
        );
        return surface_capabilities;
    }

    VkExtent2D Surface::get_extent(
        VkSurfaceCapabilitiesKHR surface_capabilities,
        uint32_t                 width,
        uint32_t                 height) const {

        VkExtent2D surface_extent;
        if (surface_capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
            surface_extent = surface_capabilities.currentExtent;
        } else {
            surface_extent.width = std::max(surface_capabilities.minImageExtent.width, std::min(surface_capabilities.maxImageExtent.width, width));
            surface_extent.height = std::max(surface_capabilities.minImageExtent.height, std::min(surface_capabilities.maxImageExtent.height, height));
        }
        return surface_extent;
    }

}}