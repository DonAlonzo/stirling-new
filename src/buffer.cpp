#include "buffer.hpp"
#include "vulkan.hpp"
#include "vulkan_create.hpp"

#include <memory>

namespace stirling { namespace vulkan {

    Buffer::Buffer(
        const BufferCreateInfo& create_info,
        VkDevice                device,
        VkPhysicalDevice        physical_device) :

        buffer (create_buffer(create_info, device)) {
    };

}}