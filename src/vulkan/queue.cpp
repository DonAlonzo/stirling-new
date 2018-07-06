#include "queue.hpp"

namespace stirling { namespace vulkan {

    Queue::Queue(VkQueue queue) :
        queue (queue) {
    }

    void Queue::queue_submit(const std::vector<SubmitInfo>& submit_infos, VkFence fence) const {
        vulkan::queue_submit(submit_infos, queue, fence);
    }

    void Queue::queue_present(const PresentInfoKHR& present_info) const {
        vulkan::queue_present(present_info, queue);
    }

    void Queue::wait_idle() const {
        vkQueueWaitIdle(queue);
    }

}}