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
        
    private:
        VkQueue queue;
    };

}}