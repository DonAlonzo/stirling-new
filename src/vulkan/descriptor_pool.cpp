#include "descriptor_pool.hpp"
#include "vulkan.hpp"

namespace stirling { namespace vulkan {

    inline Deleter<VkDescriptorPool> create_descriptor_pool(
        const DescriptorPoolCreateInfo& create_info,
        VkDevice                        device) {

        const VkDescriptorPoolCreateInfo vk_create_info {
            .sType         = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
            .flags         = create_info.flags,
            .maxSets       = create_info.max_sets,
            .poolSizeCount = static_cast<uint32_t>(create_info.pool_sizes.size()),
            .pPoolSizes    = create_info.pool_sizes.data()
        };

        return create<VkDescriptorPool>(
            vkCreateDescriptorPool,
            vkDestroyDescriptorPool,
            device,
            "Failed to create descriptor pool.",
            &vk_create_info
        );
    }

    DescriptorPool::DescriptorPool(
        const DescriptorPoolCreateInfo& create_info,
        VkDevice                        device) :

        descriptor_pool (create_descriptor_pool(create_info, device)),
        device          (device) {
    }

    std::vector<DescriptorSet> DescriptorPool::allocate_descriptor_sets(
        const DescriptorSetAllocateInfo& allocate_info) const {
        
        const VkDescriptorSetAllocateInfo vk_allocate_info {
            .sType              = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
            .descriptorPool     = descriptor_pool,
            .descriptorSetCount = static_cast<uint32_t>(allocate_info.set_layouts.size()),
            .pSetLayouts        = allocate_info.set_layouts.data()
        };

        std::vector<VkDescriptorSet> vk_descriptor_sets{vk_allocate_info.descriptorSetCount};
        vulkan_assert(
            vkAllocateDescriptorSets(
                device,
                &vk_allocate_info,
                vk_descriptor_sets.data()
            ),
            "Failed to alocate descriptor sets."
        );

        std::vector<DescriptorSet> descriptor_sets;
        descriptor_sets.reserve(vk_allocate_info.descriptorSetCount);
        std::transform(
            vk_descriptor_sets.begin(),
            vk_descriptor_sets.end(),
            std::back_inserter(descriptor_sets),
            [this](VkDescriptorSet descriptor_set) -> Deleter<VkDescriptorSet> {
                return {
                    std::bind(
                        vulkan::free_descriptor_set,
                        device,
                        descriptor_pool,
                        std::placeholders::_1
                    ),
                    descriptor_set
                };
            }
        );
        return descriptor_sets;
    }

}}