#pragma once

#include "deleter.hpp"
#include "vulkan_structs.hpp"

#include <vulkan/vulkan.h>

namespace stirling { namespace vulkan {

    struct Fence {
        Fence(Deleter<VkFence>&& fence, VkDevice device);

        inline operator const VkFence() const { return fence; }

        void wait() const;
        void reset() const;

    private:
        Deleter<VkFence> fence;
        VkDevice device;
    };

}}