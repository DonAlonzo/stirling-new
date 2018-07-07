#include "swapchain.hpp"
#include "vulkan.hpp"

namespace stirling { namespace vulkan {

    Swapchain::Swapchain(Deleter<VkSwapchainKHR>&& swapchain, VkDevice device) :
        swapchain (swapchain),
        device    (device) {
    }

    std::vector<VkImage> Swapchain::get_images() const {
        return vulkan::get_swapchain_images(device, swapchain);
    }

}}