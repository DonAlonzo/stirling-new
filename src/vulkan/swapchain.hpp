#pragma once

#include "deleter.hpp"
#include "vulkan_structs.hpp"

#include <vulkan/vulkan.h>

namespace stirling { namespace vulkan {

    struct SwapchainCreateInfo {
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
    };

    struct Swapchain {
        Swapchain(Deleter<VkSwapchainKHR>&& swapchain, VkDevice device);

        inline operator const VkSwapchainKHR() const { return swapchain; }

        std::vector<VkImage> get_images() const;
        uint32_t acquire_next_image(
            VkSemaphore semaphore = VK_NULL_HANDLE,
            VkFence     fence = VK_NULL_HANDLE,
            uint64_t    timeout = std::numeric_limits<uint64_t>::max()) const;

    private:
        Deleter<VkSwapchainKHR> swapchain;
        VkDevice device;
    };

}}