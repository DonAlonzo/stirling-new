#pragma once

#include "buffer.hpp"
#include "command_pool.hpp"
#include "deleter.hpp"
#include "descriptor_pool.hpp"
#include "device_memory.hpp"
#include "fence.hpp"
#include "pipeline.hpp"
#include "swapchain.hpp"
#include "vulkan_structs.hpp"
#include "queue.hpp"

#include <vulkan/vulkan.h>

namespace stirling { namespace vulkan {

    struct DeviceCreateInfo {
        std::vector<DeviceQueueCreateInfo> queues;
        std::vector<const char*>           enabled_extensions;
        VkPhysicalDeviceFeatures           enabled_features;
    };

    struct DescriptorSetLayoutCreateInfo {
        VkDescriptorSetLayoutCreateFlags          flags;
        std::vector<VkDescriptorSetLayoutBinding> bindings;
    };

    struct PipelineLayoutCreateInfo {
        std::vector<VkDescriptorSetLayout> set_layouts;
        std::vector<VkPushConstantRange>   push_constant_ranges;
    };

    struct ImageViewCreateInfo {
        VkImage                 image;
        VkImageViewType         view_type;
        VkFormat                format;
        VkComponentMapping      components;
        VkImageSubresourceRange subresource_range;
    };

    struct RenderPassCreateInfo {
        std::vector<AttachmentDescription> attachments;
        std::vector<SubpassDescription>    subpasses;
        std::vector<SubpassDependency>     dependencies;
    };

    struct FramebufferCreateInfo {
        VkRenderPass             render_pass;
        std::vector<VkImageView> attachments;
        uint32_t                 width;
        uint32_t                 height;
        uint32_t                 layers;
    };

    struct Device {
        Device(
            const DeviceCreateInfo& create_info,
            VkPhysicalDevice        physical_device);

        inline operator const VkDevice() const { return device; }
        
        VkMemoryRequirements get_buffer_memory_requirements(VkBuffer buffer) const;
        Queue get_queue(uint32_t queue_family, uint32_t queue_index) const;
        
        DeviceMemory allocate_memory(const MemoryAllocateInfo& allocate_info) const;
        
        Buffer create_buffer(const BufferCreateInfo& create_info) const;
        CommandPool create_command_pool(const CommandPoolCreateInfo& create_info) const;
        DescriptorPool create_descriptor_pool(const DescriptorPoolCreateInfo& create_info) const;
        Swapchain create_swapchain(const SwapchainCreateInfo& create_info) const;
        Deleter<VkDescriptorSetLayout> create_descriptor_set_layout(
            const DescriptorSetLayoutCreateInfo& create_info) const;
        Deleter<VkPipelineLayout> create_pipeline_layout(const PipelineLayoutCreateInfo& create_info) const;
        Deleter<VkImageView> create_image_view(const ImageViewCreateInfo& create_info) const;
        RenderPass create_render_pass(const RenderPassCreateInfo& create_info) const;
        Pipeline create_pipeline(
            const GraphicsPipelineCreateInfo& create_info,
            VkPipelineCache                   pipeline_cache) const;
        Deleter<VkShaderModule> create_shader_module(const VkShaderModuleCreateInfo& create_info) const;
        Deleter<VkShaderModule> create_shader_module(const char* file_name) const;
        Deleter<VkFramebuffer> create_framebuffer(const FramebufferCreateInfo& create_info) const;
        Deleter<VkSemaphore> create_semaphore() const;
        std::vector<Deleter<VkSemaphore>> create_semaphores(size_t count) const;
        Fence create_fence(bool signaled = false) const;
        std::vector<Fence> create_fences(size_t count, bool signaled = false) const;

        void update_descriptor_sets(
            const std::vector<WriteDescriptorSet>& descriptor_writes,
            const std::vector<CopyDescriptorSet>&  descriptor_copies) const;

        void wait_idle() const;

    private:
        Deleter<VkDevice> device;
    };

}}