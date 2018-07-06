#include "queue.hpp"

namespace stirling { namespace vulkan {

    Queue::Queue(VkQueue queue) :
        queue (queue) {
    }

    void Queue::submit(const std::vector<SubmitInfo>& submit_infos, VkFence fence) const {
        queue_submit(submit_infos, queue, fence);
    }

    void Queue::present(const PresentInfoKHR& present_info) const {
        queue_present(present_info, queue);
    }

    void Queue::wait_idle() const {
        vkQueueWaitIdle(queue);
    }

}}