#include "fence.hpp"
#include "vulkan.hpp"

namespace stirling { namespace vulkan {

    Fence::Fence(Deleter<VkFence>&& fence, VkDevice device) :
        fence  (fence),
        device (device) {
    }

    void Fence::wait() const {
        vulkan::wait_for_fence(device, fence);
    }

    void Fence::reset() const {
        vulkan::reset_fence(device, fence);
    }

}}