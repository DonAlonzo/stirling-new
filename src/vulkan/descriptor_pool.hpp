#pragma once

#include "deleter.hpp"
#include "descriptor_set.hpp"
#include "vulkan_structs.hpp"

#include <vulkan/vulkan.h>

namespace stirling { namespace vulkan {

    struct DescriptorPoolCreateInfo {
        VkDescriptorPoolCreateFlags       flags;
        uint32_t                          max_sets;
        std::vector<VkDescriptorPoolSize> pool_sizes;
    };

    struct DescriptorSetAllocateInfo {
        VkDescriptorPool                   descriptor_pool;
        std::vector<VkDescriptorSetLayout> set_layouts;
    };

    struct DescriptorPool {
        DescriptorPool(
            const DescriptorPoolCreateInfo& create_info,
            VkDevice                        device);

        inline operator const VkDescriptorPool() const { return descriptor_pool; }

        std::vector<DescriptorSet> allocate_descriptor_sets(
            const DescriptorSetAllocateInfo& allocate_info) const;

    private:
        Deleter<VkDescriptorPool> descriptor_pool;
        VkDevice                  device;
    };

}}