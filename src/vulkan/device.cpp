#include "device.hpp"
#include "vulkan.hpp"

namespace stirling { namespace vulkan {

    inline Deleter<VkDevice> create_device(
        const DeviceCreateInfo& create_info,
        VkPhysicalDevice        physical_device) {
        
        const VkDeviceCreateInfo vk_create_info {
            .sType                   = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
            .queueCreateInfoCount    = static_cast<uint32_t>(create_info.queues.size()),
            .pQueueCreateInfos       = cast_vector<const VkDeviceQueueCreateInfo*>(create_info.queues),
            .enabledExtensionCount   = static_cast<uint32_t>(create_info.enabled_extensions.size()),
            .ppEnabledExtensionNames = create_info.enabled_extensions.data(),
            .pEnabledFeatures        = &create_info.enabled_features
        };

        return create<VkDevice>(
            vkCreateDevice,
            vkDestroyDevice,
            "Failed to create device.",
            physical_device,
            &vk_create_info
        );
    }

    Device::Device(
        const DeviceCreateInfo& create_info,
        VkPhysicalDevice        physical_device) :

        device (create_device(create_info, physical_device)) {
    }

    VkMemoryRequirements Device::get_buffer_memory_requirements(VkBuffer buffer) const {
        return vulkan::get_buffer_memory_requirements(device, buffer);
    }

    DeviceMemory Device::allocate_memory(const MemoryAllocateInfo& allocate_info) const {
        return {allocate_info, device};
    }

    Buffer Device::create_buffer(const BufferCreateInfo& create_info) const {
        return {create_info, device};
    }

    CommandPool Device::create_command_pool(const CommandPoolCreateInfo& create_info) const {
        return {create_info, device};
    }
    
    DescriptorPool Device::create_descriptor_pool(const DescriptorPoolCreateInfo& create_info) const {
        return {create_info, device};
    }
    
    Deleter<VkDescriptorSetLayout> Device::create_descriptor_set_layout(
        const DescriptorSetLayoutCreateInfo& create_info) const {

        const VkDescriptorSetLayoutCreateInfo vk_create_info {
            .sType        = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
            .flags        = create_info.flags,
            .bindingCount = static_cast<uint32_t>(create_info.bindings.size()),
            .pBindings    = create_info.bindings.data()
        };

        return create<VkDescriptorSetLayout>(
            vkCreateDescriptorSetLayout,
            vkDestroyDescriptorSetLayout,
            device,
            "Failed to create descriptor set layout.",
            &vk_create_info
        );
    }

    Deleter<VkPipelineLayout> Device::create_pipeline_layout(
        const PipelineLayoutCreateInfo& create_info) const {

        const VkPipelineLayoutCreateInfo vk_create_info {
            .sType                  = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
            .setLayoutCount         = static_cast<uint32_t>(create_info.set_layouts.size()),
            .pSetLayouts            = create_info.set_layouts.data(),
            .pushConstantRangeCount = static_cast<uint32_t>(create_info.push_constant_ranges.size()),
            .pPushConstantRanges    = create_info.push_constant_ranges.data()
        };

        return create<VkPipelineLayout>(
            vkCreatePipelineLayout,
            vkDestroyPipelineLayout,
            device,
            "Failed to create pipeline layout.",
            &vk_create_info
        );
    }

    Swapchain Device::create_swapchain(const SwapchainCreateInfo& create_info) const {
        const VkSwapchainCreateInfoKHR vk_create_info {
            .sType                 = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
            .surface               = create_info.surface,
            .minImageCount         = create_info.min_image_count,
            .imageFormat           = create_info.image_format,
            .imageColorSpace       = create_info.image_color_space,
            .imageExtent           = create_info.image_extent,
            .imageArrayLayers      = create_info.image_array_layers,
            .imageUsage            = create_info.image_usage,
            .imageSharingMode      = create_info.image_sharing_mode,
            .queueFamilyIndexCount = static_cast<uint32_t>(create_info.queue_family_indices.size()),
            .pQueueFamilyIndices   = create_info.queue_family_indices.data(),
            .preTransform          = create_info.pre_transform,
            .compositeAlpha        = create_info.composite_alpha,
            .presentMode           = create_info.present_mode,
            .clipped               = create_info.clipped,
            .oldSwapchain          = create_info.old_swapchain
        };

        return {
            create<VkSwapchainKHR>(
                vkCreateSwapchainKHR,
                vkDestroySwapchainKHR,
                device,
                "Failed to create swapchain.",
                &vk_create_info
            ),
            device
        };
    }

    Deleter<VkImageView> Device::create_image_view(
        const ImageViewCreateInfo& create_info) const {

        const VkImageViewCreateInfo vk_create_info {
            .sType            = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
            .image            = create_info.image,
            .viewType         = create_info.view_type,
            .format           = create_info.format,
            .components       = create_info.components,
            .subresourceRange = create_info.subresource_range
        };

        return create<VkImageView>(
            vkCreateImageView,
            vkDestroyImageView,
            device,
            "Failed to create image view.",
            &vk_create_info
        );
    }

    Deleter<VkRenderPass> Device::create_render_pass(
        const RenderPassCreateInfo& create_info) const {

        const VkRenderPassCreateInfo vk_create_info {
            .sType           = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
            .attachmentCount = static_cast<uint32_t>(create_info.attachments.size()),
            .pAttachments    = cast_vector<const VkAttachmentDescription*>(create_info.attachments),
            .subpassCount    = static_cast<uint32_t>(create_info.subpasses.size()),
            .pSubpasses      = cast_vector<const VkSubpassDescription*>(create_info.subpasses),
            .dependencyCount = static_cast<uint32_t>(create_info.dependencies.size()),
            .pDependencies   = cast_vector<const VkSubpassDependency*>(create_info.dependencies)
        };

        return create<VkRenderPass>(
            vkCreateRenderPass,
            vkDestroyRenderPass,
            device,
            "Failed to create render pass.",
            &vk_create_info
        );
    }

    Pipeline Device::create_pipeline(
        const GraphicsPipelineCreateInfo& create_info,
        VkPipelineCache                   pipeline_cache) const {

        return {create_info, pipeline_cache, device};
    }

    Queue Device::get_queue(uint32_t queue_family, uint32_t queue_index) const {
        return vulkan::get_queue(device, queue_family, queue_index);
    }

    void Device::wait_idle() const {
        vkDeviceWaitIdle(device);
    }

}}