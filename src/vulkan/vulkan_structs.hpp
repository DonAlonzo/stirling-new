#pragma once

#include "deleter.hpp"
#include "vulkan_helpers.hpp"

#include <vulkan/vulkan.h>

#include <cassert>
#include <vector>

namespace stirling { namespace vulkan {

    struct QueueFamilyIndices {
        uint32_t graphics_queue = -1;
        uint32_t present_queue = -1;
    };

    template<typename From, typename To>
    struct Wrapper {
        Wrapper() : to (From{}) {}
        Wrapper(From&& from) : to (from) {}

        inline operator const To*() const { return &to; }
        inline const To& data() const { return to; }

    private:
        To to;
    };

    struct ApplicationInfoData {
        const char* application_name;
        uint32_t    application_version;
        const char* engine_name;
        uint32_t    engine_version;
        uint32_t    api_version;

        inline operator const VkApplicationInfo() const {
            return {
                .sType              = VK_STRUCTURE_TYPE_APPLICATION_INFO,
                .pApplicationName   = application_name,
                .applicationVersion = application_version,
                .pEngineName        = engine_name,
                .engineVersion      = engine_version,
                .apiVersion         = api_version
            };
        };
    };
    typedef Wrapper<ApplicationInfoData, VkApplicationInfo> ApplicationInfo;

    struct DebugReportCallbackCreateInfoEXTData {
        VkDebugReportFlagsEXT        flags;
        PFN_vkDebugReportCallbackEXT callback;
        void*                        user_data;

        inline operator const VkDebugReportCallbackCreateInfoEXT() const {
            return {
                .sType       = VK_STRUCTURE_TYPE_DEBUG_REPORT_CALLBACK_CREATE_INFO_EXT,
                .flags       = flags,
                .pfnCallback = callback,
                .pUserData   = user_data
            };
        }
    };
    typedef Wrapper<DebugReportCallbackCreateInfoEXTData, VkDebugReportCallbackCreateInfoEXT> DebugReportCallbackCreateInfoEXT;

    struct DeviceQueueCreateInfoData {
        uint32_t           queue_family_index;
        std::vector<float> queue_priorities;

        inline operator const VkDeviceQueueCreateInfo() const {
            return {
                .sType            = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
                .queueFamilyIndex = queue_family_index,
                .queueCount       = static_cast<uint32_t>(queue_priorities.size()),
                .pQueuePriorities = queue_priorities.data()
            };
        }
    }; 
    typedef Wrapper<DeviceQueueCreateInfoData, VkDeviceQueueCreateInfo> DeviceQueueCreateInfo; 

    struct AttachmentDescriptionData {
        VkAttachmentDescriptionFlags flags;
        VkFormat                     format;
        VkSampleCountFlagBits        samples;
        VkAttachmentLoadOp           load_op;
        VkAttachmentStoreOp          store_op;
        VkAttachmentLoadOp           stencil_load_op;
        VkAttachmentStoreOp          stencil_store_op;
        VkImageLayout                initial_layout;
        VkImageLayout                final_layout;

        inline operator const VkAttachmentDescription() const {
            return {
                .flags          = flags,
                .format         = format,
                .samples        = samples,
                .loadOp         = load_op,
                .storeOp        = store_op,
                .stencilLoadOp  = stencil_load_op,
                .stencilStoreOp = stencil_store_op,
                .initialLayout  = initial_layout,
                .finalLayout    = final_layout
            };
        }
    };
    typedef Wrapper<AttachmentDescriptionData, VkAttachmentDescription> AttachmentDescription;

    struct SubpassDescriptionData {
        VkSubpassDescriptionFlags          flags;
        VkPipelineBindPoint                pipeline_bind_point;
        std::vector<VkAttachmentReference> input_attachments;
        std::vector<VkAttachmentReference> color_attachments;
        std::vector<VkAttachmentReference> resolve_attachments;
        VkAttachmentReference              depth_stencil_attachment = { VK_ATTACHMENT_UNUSED };
        std::vector<uint32_t>              preserve_attachments;

        inline operator const VkSubpassDescription() const {
            return {
                .flags                   = flags,
                .pipelineBindPoint       = pipeline_bind_point,
                .inputAttachmentCount    = static_cast<uint32_t>(input_attachments.size()),
                .pInputAttachments       = input_attachments.data(),
                .colorAttachmentCount    = static_cast<uint32_t>(color_attachments.size()),
                .pColorAttachments       = color_attachments.data(),
                .pResolveAttachments     = resolve_attachments.data(),
                .pDepthStencilAttachment = &depth_stencil_attachment,
                .preserveAttachmentCount = static_cast<uint32_t>(preserve_attachments.size()),
                .pPreserveAttachments    = preserve_attachments.data()
            };
        }
    };
    typedef Wrapper<SubpassDescriptionData, VkSubpassDescription> SubpassDescription;

    struct SubpassDependencyData {
        uint32_t             src_subpass;
        uint32_t             dst_subpass;
        VkPipelineStageFlags src_stage_mask;
        VkPipelineStageFlags dst_stage_mask;
        VkAccessFlags        src_access_mask;
        VkAccessFlags        dst_access_mask;
        VkDependencyFlags    dependency_flags;

        inline operator const VkSubpassDependency() const {
            return {
                .srcSubpass      = src_subpass,
                .dstSubpass      = dst_subpass,
                .srcStageMask    = src_stage_mask,
                .dstStageMask    = dst_stage_mask,
                .srcAccessMask   = src_access_mask,
                .dstAccessMask   = dst_access_mask,
                .dependencyFlags = dependency_flags
            };
        }
    };
    typedef Wrapper<SubpassDependencyData, VkSubpassDependency> SubpassDependency;

    struct FramebufferCreateInfoData {
        VkRenderPass             render_pass;
        std::vector<VkImageView> attachments;
        uint32_t                 width;
        uint32_t                 height;
        uint32_t                 layers;

        inline operator const VkFramebufferCreateInfo() const {
            return {
                .sType           = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
                .renderPass      = render_pass,
                .attachmentCount = static_cast<uint32_t>(attachments.size()),
                .pAttachments    = attachments.data(),
                .width           = width,
                .height          = height,
                .layers          = layers
            };
        }
    };
    typedef Wrapper<FramebufferCreateInfoData, VkFramebufferCreateInfo> FramebufferCreateInfo;

    struct RenderPassBeginInfoData {
        VkRenderPass              render_pass;
        VkFramebuffer             framebuffer;
        VkRect2D                  render_area;
        std::vector<VkClearValue> clear_values;

        inline operator const VkRenderPassBeginInfo() const {
            return {
                .sType           = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
                .renderPass      = render_pass,
                .framebuffer     = framebuffer,
                .renderArea      = render_area,
                .clearValueCount = static_cast<uint32_t>(clear_values.size()),
                .pClearValues    = clear_values.data()
            };
        }
    };
    typedef Wrapper<RenderPassBeginInfoData, VkRenderPassBeginInfo> RenderPassBeginInfo;

    struct CommandBufferInheritanceInfoData {
        VkRenderPass                  render_pass;
        uint32_t                      subpass;
        VkFramebuffer                 framebuffer;
        VkBool32                      occlusion_query_enable;
        VkQueryControlFlags           query_flags;
        VkQueryPipelineStatisticFlags pipeline_statistics;

        inline operator const VkCommandBufferInheritanceInfo() const {
            return {
                .sType                = VK_STRUCTURE_TYPE_COMMAND_BUFFER_INHERITANCE_INFO,
                .renderPass           = render_pass,
                .subpass              = subpass,
                .framebuffer          = framebuffer,
                .occlusionQueryEnable = occlusion_query_enable,
                .queryFlags           = query_flags,
                .pipelineStatistics   = pipeline_statistics
            };
        }
    };
    typedef Wrapper<CommandBufferInheritanceInfoData, VkCommandBufferInheritanceInfo> CommandBufferInheritanceInfo;

    struct CommandBufferBeginInfoData {
        VkCommandBufferUsageFlags    flags;
        CommandBufferInheritanceInfo inheritance_info;

        inline operator const VkCommandBufferBeginInfo() const {
            return {
                .sType            = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
                .flags            = flags,
                .pInheritanceInfo = inheritance_info
            };
        }
    };
    typedef Wrapper<CommandBufferBeginInfoData, VkCommandBufferBeginInfo> CommandBufferBeginInfo;

    struct SubmitInfoData {
        std::vector<VkSemaphore>     wait_semaphores;
        VkPipelineStageFlags         wait_dst_stage_mask;
        std::vector<VkCommandBuffer> command_buffers;
        std::vector<VkSemaphore>     signal_semaphores;

        inline operator const VkSubmitInfo() const {
            return {
                .sType                = VK_STRUCTURE_TYPE_SUBMIT_INFO,
                .waitSemaphoreCount   = static_cast<uint32_t>(wait_semaphores.size()),
                .pWaitSemaphores      = wait_semaphores.data(),
                .pWaitDstStageMask    = &wait_dst_stage_mask,
                .commandBufferCount   = static_cast<uint32_t>(command_buffers.size()),
                .pCommandBuffers      = command_buffers.data(),
                .signalSemaphoreCount = static_cast<uint32_t>(signal_semaphores.size()),
                .pSignalSemaphores    = signal_semaphores.data(),
            };
        }
    };
    typedef Wrapper<SubmitInfoData, VkSubmitInfo> SubmitInfo;

    struct PresentInfoKHRData {
        std::vector<VkSemaphore>    wait_semaphores;
        std::vector<VkSwapchainKHR> swapchains;
        std::vector<uint32_t>       image_indices;

        inline operator const VkPresentInfoKHR() const {
            assert(image_indices.size() == swapchains.size());
            return {
                .sType              = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
                .waitSemaphoreCount = static_cast<uint32_t>(wait_semaphores.size()),
                .pWaitSemaphores    = wait_semaphores.data(),
                .swapchainCount     = static_cast<uint32_t>(swapchains.size()),
                .pSwapchains        = swapchains.data(),
                .pImageIndices      = image_indices.data(),
            };
        }
    };
    typedef Wrapper<PresentInfoKHRData, VkPresentInfoKHR> PresentInfoKHR;

    struct WriteDescriptorSetData {
        VkDescriptorSet        dst_set;
        uint32_t               dst_binding;
        uint32_t               dst_array_element;
        uint32_t               descriptor_count;
        VkDescriptorType       descriptor_type;
        VkDescriptorImageInfo  image_info;
        VkDescriptorBufferInfo buffer_info;
        VkBufferView*          texel_buffer_view;

        inline operator const VkWriteDescriptorSet() const {
            return {
                .sType            = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
                .dstSet           = dst_set,
                .dstBinding       = dst_binding,
                .dstArrayElement  = dst_array_element,
                .descriptorCount  = descriptor_count,
                .descriptorType   = descriptor_type,
                .pImageInfo       = &image_info,
                .pBufferInfo      = &buffer_info,
                .pTexelBufferView = texel_buffer_view
            };
        }
    };
    typedef Wrapper<WriteDescriptorSetData, VkWriteDescriptorSet> WriteDescriptorSet;

    struct CopyDescriptorSetData {
        VkDescriptorSet src_set;
        uint32_t        src_binding;
        uint32_t        src_array_element;
        VkDescriptorSet dst_set;
        uint32_t        dst_binding;
        uint32_t        dst_array_element;
        uint32_t        descriptor_count;

        inline operator const VkCopyDescriptorSet() const {
            return {
                .sType           = VK_STRUCTURE_TYPE_COPY_DESCRIPTOR_SET,
                .srcSet          = src_set,
                .srcBinding      = src_binding,
                .srcArrayElement = src_array_element,
                .dstSet          = dst_set,
                .dstBinding      = dst_binding,
                .dstArrayElement = dst_array_element,
                .descriptorCount = descriptor_count
            };
        }
    };
    typedef Wrapper<CopyDescriptorSetData, VkCopyDescriptorSet> CopyDescriptorSet;

}}