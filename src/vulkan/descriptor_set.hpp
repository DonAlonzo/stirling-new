#pragma once

#include "deleter.hpp"
#include "vulkan_structs.hpp"

#include <vulkan/vulkan.h>

namespace stirling { namespace vulkan {

    struct DescriptorSet {
        DescriptorSet(Deleter<VkDescriptorSet>&& descriptor_set);

        inline operator const VkDescriptorSet() const { return descriptor_set; }

    private:
        Deleter<VkDescriptorSet> descriptor_set;
    };

}}