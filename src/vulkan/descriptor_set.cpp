#include "descriptor_set.hpp"
#include "vulkan.hpp"

namespace stirling { namespace vulkan {

    DescriptorSet::DescriptorSet(Deleter<VkDescriptorSet>&& descriptor_set) :
        descriptor_set (descriptor_set) {
    }

}}