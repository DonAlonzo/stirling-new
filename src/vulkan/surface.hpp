#pragma once

#include "deleter.hpp"
#include "vulkan_structs.hpp"

#include <vulkan/vulkan.h>

#include <vector>

namespace stirling { namespace vulkan {

    struct Surface {
        Surface(Deleter<VkSurfaceKHR>&& surface);

        inline operator const VkSurfaceKHR() const { return surface; }

        VkBool32 get_present_support(VkPhysicalDevice physical_device, uint32_t queue_family_index) const;
        std::vector<VkSurfaceFormatKHR> get_formats(VkPhysicalDevice physical_device) const;
        std::vector<VkPresentModeKHR> get_present_modes(VkPhysicalDevice physical_device) const;
        VkSurfaceCapabilitiesKHR get_capabilities(VkPhysicalDevice physical_device) const;

    private:
        Deleter<VkSurfaceKHR> surface;
    };

}}