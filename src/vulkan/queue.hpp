#pragma once

#include "vulkan_structs.hpp"
#include "vulkan.hpp"

#include <vulkan/vulkan.h>

namespace stirling { namespace vulkan {

    struct Queue {
        Queue(VkQueue queue);

        inline operator const VkQueue() const {
            return queue;
        }
    
        void submit(const std::vector<SubmitInfo>& submit_infos, VkFence fence = VK_NULL_HANDLE) const;
        void present(const PresentInfoKHR& present_info) const;
        void wait_idle() const;

    private:
        VkQueue queue;
    };

}}