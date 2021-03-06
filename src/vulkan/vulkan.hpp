#pragma once

#include "surface.hpp"
#include "vulkan_create.hpp"
#include "vulkan_helpers.hpp"
#include "vulkan_structs.hpp"

#include <glm/glm.hpp>

#include <cassert>
#include <iostream>
#include <memory>
#include <optional>
#include <vector>

namespace stirling { namespace vulkan {

    using DebugReportCallback = Deleter<VkDebugReportCallbackEXT>;
    using DescriptorSetLayout = Deleter<VkDescriptorSetLayout>;
    using PipelineLayout = Deleter<VkPipelineLayout>;
    using ImageView = Deleter<VkImageView>;
    using Framebuffer = Deleter<VkFramebuffer>;
    using SurfaceFormat = VkSurfaceFormatKHR;
    using Extent2D = VkExtent2D;
    using RenderPass = Deleter<VkRenderPass>;
    using MemoryRequirements = VkMemoryRequirements;

    inline void free_command_buffer(
        VkDevice        device,
        VkCommandPool   command_pool,
        VkCommandBuffer command_buffer) {
        
        vkFreeCommandBuffers(device, command_pool, 1, &command_buffer);
    }

    inline void free_descriptor_set(
        VkDevice         device,
        VkDescriptorPool descriptor_pool,
        VkDescriptorSet  descriptor_set) {

        vkFreeDescriptorSets(device, descriptor_pool, 1, &descriptor_set);
    }

    inline void update_descriptor_sets(
        VkDevice                               device,
        const std::vector<WriteDescriptorSet>& descriptor_writes,
        const std::vector<CopyDescriptorSet>&  descriptor_copies) {

        vkUpdateDescriptorSets(
            device,
            static_cast<uint32_t>(descriptor_writes.size()),
            cast_vector<const VkWriteDescriptorSet*>(descriptor_writes),
            static_cast<uint32_t>(descriptor_copies.size()),
            cast_vector<const VkCopyDescriptorSet*>(descriptor_copies)
        );
    }

    inline void cmd_copy_buffer(
        VkCommandBuffer                  command_buffer,
        VkBuffer                         src_buffer,
        VkBuffer                         dst_buffer,
        const std::vector<VkBufferCopy>& regions) {

        vkCmdCopyBuffer(
            command_buffer,
            src_buffer,
            dst_buffer,
            regions.size(),
            regions.data()
        );
    }

    inline void cmd_bind_vertex_buffers(
        VkCommandBuffer                  command_buffer,
        uint32_t                         first_binding,
        const std::vector<VkBuffer>&     buffers,
        const std::vector<VkDeviceSize>& offsets) {

        assert(buffers.size() == offsets.size());
        vkCmdBindVertexBuffers(
            command_buffer,
            first_binding,
            buffers.size(),
            buffers.data(),
            offsets.data()
        );
    }

    inline void cmd_bind_index_buffer(
        VkCommandBuffer command_buffer,
        VkBuffer        buffer,
        VkDeviceSize    offset,
        VkIndexType     index_type) {

        vkCmdBindIndexBuffer(
            command_buffer,
            buffer,
            offset,
            index_type
        );
    }

    inline void cmd_bind_descriptor_sets(
        VkCommandBuffer              command_buffer,
        VkPipelineBindPoint          pipeline_bind_point,
        VkPipelineLayout             layout,
        uint32_t                     first_set,
        std::vector<VkDescriptorSet> descriptor_sets,
        std::vector<uint32_t>        dynamic_offsets) {

        vkCmdBindDescriptorSets(
            command_buffer,
            pipeline_bind_point,
            layout,
            first_set,
            static_cast<uint32_t>(descriptor_sets.size()),
            descriptor_sets.data(),
            static_cast<uint32_t>(dynamic_offsets.size()),
            dynamic_offsets.data()
        );
    }

    inline void begin_command_buffer(
        const CommandBufferBeginInfo& begin_info,
        VkCommandBuffer               command_buffer) {
        
        vulkan_assert(
            vkBeginCommandBuffer(command_buffer, begin_info),
            "Failed to begin command buffer."
        );
    }

    inline void end_command_buffer(
        VkCommandBuffer command_buffer) {
        
        vulkan_assert(
            vkEndCommandBuffer(command_buffer),
            "Failed to end command buffer."
        );
    }

    inline void begin_render_pass(
        const RenderPassBeginInfo& begin_info,
        VkCommandBuffer            command_buffer,
        VkSubpassContents          contents) {
        
        vkCmdBeginRenderPass(command_buffer, begin_info, contents);
    }

    inline void wait_for_fence(
        VkDevice             device,
        std::vector<VkFence> fences,
        VkBool32             wait_all = VK_TRUE,
        uint64_t             timeout = std::numeric_limits<uint64_t>::max()) {

        vulkan_assert(
            vkWaitForFences(device, fences.size(), fences.data(), wait_all, timeout),
            "Failed to wait for fences."
        );
    }

    inline void wait_for_fence(
        VkDevice device,
        VkFence  fence,
        VkBool32 wait_all = VK_TRUE,
        uint64_t timeout = std::numeric_limits<uint64_t>::max()) {

        vulkan_assert(
            vkWaitForFences(device, 1, &fence, wait_all, timeout),
            "Failed to wait for fence."
        );
    }

    inline void reset_fence(
        VkDevice             device,
        std::vector<VkFence> fences) {

        vulkan_assert(
            vkResetFences(device, fences.size(), fences.data()),
            "Failed to reset fences."
        );
    }

    inline void reset_fence(
        VkDevice device,
        VkFence  fence) {

        vulkan_assert(
            vkResetFences(device, 1, &fence),
            "Failed to reset fence."
        );
    }

    inline uint32_t acquire_next_image(
        VkDevice       device,
        VkSwapchainKHR swapchain,
        VkSemaphore    semaphore = VK_NULL_HANDLE,
        VkFence        fence = VK_NULL_HANDLE,
        uint64_t       timeout = std::numeric_limits<uint64_t>::max()) {

        uint32_t image_index;
        vkAcquireNextImageKHR(device, swapchain, timeout, semaphore, fence, &image_index);
        return image_index;
    }

    inline void queue_submit(
        const std::vector<SubmitInfo>& submit_infos,
        VkQueue                        queue,
        VkFence                        fence = VK_NULL_HANDLE) {
        
        vulkan_assert(
            vkQueueSubmit(
                queue,
                submit_infos.size(),
                cast_vector<const VkSubmitInfo*>(submit_infos),
                fence
            ),
            "Failed to submit to queue."
        );
    }

    inline void queue_present(
        const PresentInfoKHR& present_info,
        VkQueue               queue) {

        vulkan_assert(vkQueuePresentKHR(queue, present_info), "Failed to present to queue.");
    }

    inline std::vector<VkPhysicalDevice> get_physical_devices(
        VkInstance instance) {

        // Get number of physical devices
        uint32_t physical_device_count = 0;
        vulkan_assert(
            vkEnumeratePhysicalDevices(instance, &physical_device_count, nullptr),
            "Failed to get number of physical devices."
        );

        // Get physical devices
        std::vector<VkPhysicalDevice> physical_devices{physical_device_count};
        vulkan_assert(
            vkEnumeratePhysicalDevices(instance, &physical_device_count, physical_devices.data()),
            "Failed to get physical devices."
        );
        return physical_devices;
    }

    inline VkPhysicalDeviceProperties get_physical_device_properties(
        VkPhysicalDevice physical_device) {

        VkPhysicalDeviceProperties properties;
        vkGetPhysicalDeviceProperties(physical_device, &properties);
        return properties;
    }

    inline VkPhysicalDeviceFeatures get_physical_device_features(
        VkPhysicalDevice physical_device) {

        VkPhysicalDeviceFeatures features;
        vkGetPhysicalDeviceFeatures(physical_device, &features);
        return features;
    }

    inline std::vector<VkQueueFamilyProperties> get_queue_family_properties(
        VkPhysicalDevice physical_device) {
        
        // Get number of queue families on physical device
        uint32_t queue_family_property_count;
        vkGetPhysicalDeviceQueueFamilyProperties(physical_device, &queue_family_property_count, nullptr);

        // Get queue families on physical device
        std::vector<VkQueueFamilyProperties> queue_family_properties{queue_family_property_count};
        vkGetPhysicalDeviceQueueFamilyProperties(physical_device, &queue_family_property_count, queue_family_properties.data());
        return queue_family_properties;
    }

    inline QueueFamilyIndices get_queue_families(
        VkPhysicalDevice physical_device,
        const Surface&   surface) {

        const auto queue_family_properties = get_queue_family_properties(physical_device);

        // Find queue family indices
        QueueFamilyIndices queue_family_indices;
        for (uint32_t i = 0; i < queue_family_properties.size(); ++i) {
            if (queue_family_properties[i].queueCount > 0) {
                // Check if graphics queue
                if (queue_family_properties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
                    queue_family_indices.graphics_queue = i;
                }

                // Check if present queue
                if (surface.get_present_support(physical_device, i)) {
                    queue_family_indices.present_queue = i;
                }
            }
        }
        if (queue_family_indices.graphics_queue == -1 || queue_family_indices.present_queue == -1) {
            throw "Failed to find all queue families.";
        }

        return queue_family_indices;
    }

    inline std::vector<VkImage> get_swapchain_images(
        VkDevice       device,
        VkSwapchainKHR swapchain) {

        // Get swapchain image count
        uint32_t swapchain_image_count;
        vulkan_assert(
            vkGetSwapchainImagesKHR(device, swapchain, &swapchain_image_count, nullptr),
            "Failed to get number of swapchain images."
        );

        // Get swapchain images
        std::vector<VkImage> swapchain_images{swapchain_image_count};
        vulkan_assert(
            vkGetSwapchainImagesKHR(device, swapchain, &swapchain_image_count, swapchain_images.data()),
            "Failed to get swapchain images."
        );
        return swapchain_images;
    }

    inline VkQueue get_queue(
        VkDevice device,
        uint32_t queue_family,
        uint32_t queue_index) {

        VkQueue queue;
        vkGetDeviceQueue(device, queue_family, queue_index, &queue);
        return queue;
    }

    inline VkMemoryRequirements get_buffer_memory_requirements(
        VkDevice device,
        VkBuffer buffer) {

        VkMemoryRequirements memory_requirements;
        vkGetBufferMemoryRequirements(device, buffer, &memory_requirements);
        return memory_requirements;
    }

    inline VkPhysicalDeviceMemoryProperties get_physical_device_memory_properties(
        VkPhysicalDevice physical_device) {

        VkPhysicalDeviceMemoryProperties memory_properties;
        vkGetPhysicalDeviceMemoryProperties(physical_device, &memory_properties);
        return memory_properties;
    }

    inline uint32_t find_memory_type(
        VkPhysicalDevice      physical_device,
        uint32_t              type_filter,
        VkMemoryPropertyFlags properties) {

        const auto memory_properties = get_physical_device_memory_properties(physical_device);

        for (uint32_t i = 0; i < memory_properties.memoryTypeCount; ++i) {
            if (type_filter & (1 << i) &&
               (memory_properties.memoryTypes[i].propertyFlags & properties) == properties) {
                return i;
            }
        }

        throw "Failed to find suitable memory type.";
    };

}}