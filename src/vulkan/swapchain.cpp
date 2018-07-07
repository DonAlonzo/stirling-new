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

    uint32_t Swapchain::acquire_next_image(
        VkSemaphore semaphore,
        VkFence     fence,
        uint64_t    timeout) const {

        return vulkan::acquire_next_image(device, swapchain, semaphore, fence, timeout);
    }

}}