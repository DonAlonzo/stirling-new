#pragma once

#include "deleter.hpp"
#include "vulkan_structs.hpp"

#include <vulkan/vulkan.h>

namespace stirling { namespace vulkan {

    struct Buffer {
        Buffer(
            const BufferCreateInfo& create_info,
            VkDevice                device);

        inline operator const VkBuffer() const { return buffer; }

    private:
        Deleter<VkBuffer> buffer;
    };

}}