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
        uint32_t format_count;
        vulkan_assert(
            vkGetPhysicalDeviceSurfaceFormatsKHR(physical_device, surface, &format_count, nullptr),
            "Failed to get number of surface formats."
        );

        // Get supported surface formats
        std::vector<VkSurfaceFormatKHR> formats{format_count};
        vulkan_assert(
            vkGetPhysicalDeviceSurfaceFormatsKHR(physical_device, surface, &format_count, formats.data()),
            "Failed to get surface formats."
        );
        return formats;
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
        VkSurfaceCapabilitiesKHR capabilities;
        vulkan_assert(
            vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physical_device, surface, &capabilities),
            "Failed to get surface capabilities."
        );
        return capabilities;
    }

}}