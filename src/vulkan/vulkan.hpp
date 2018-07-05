#pragma once

#include "vulkan_create.hpp"
#include "vulkan_helpers.hpp"
#include "vulkan_structs.hpp"

#include <glm/glm.hpp>

#include <cassert>
#include <memory>
#include <optional>
#include <vector>

namespace stirling { namespace vulkan {

    inline std::vector<VkDescriptorSet> allocate_descriptor_sets(
        const DescriptorSetAllocateInfo& allocate_info,
        VkDevice                         device) {

        std::vector<VkDescriptorSet> descriptor_sets{allocate_info.data().descriptorSetCount};
        vulkan_assert(
            vkAllocateDescriptorSets(device, allocate_info, descriptor_sets.data()),
            "Failed to alocate descriptor sets."
        );
        return descriptor_sets;
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

    inline VkBool32 get_surface_present_support(
        VkPhysicalDevice physical_device,
        uint32_t         queue_family_index,
        VkSurfaceKHR     surface) {

        VkBool32 present_support;
        vulkan_assert(
            vkGetPhysicalDeviceSurfaceSupportKHR(physical_device, queue_family_index, surface, &present_support),
            "Failed to get surface present support."
        );
        return present_support;
    }

    inline std::vector<VkSurfaceFormatKHR> get_surface_formats(
        VkPhysicalDevice physical_device,
        VkSurfaceKHR     surface) {

        // Get supported surface format count
        uint32_t surface_format_count;
        vulkan_assert(
            vkGetPhysicalDeviceSurfaceFormatsKHR(physical_device, surface, &surface_format_count, nullptr),
            "Failed to get number of surface formats."
        );

        // Get supported surface formats
        std::vector<VkSurfaceFormatKHR> surface_formats{surface_format_count};
        vulkan_assert(
            vkGetPhysicalDeviceSurfaceFormatsKHR(physical_device, surface, &surface_format_count, surface_formats.data()),
            "Failed to get surface formats."
        );
        return surface_formats;
    }

    inline std::vector<VkPresentModeKHR> get_surface_present_modes(
        VkPhysicalDevice physical_device,
        VkSurfaceKHR     surface) {

        // Get surface present mode count
        uint32_t present_mode_count;
        vulkan_assert(
            vkGetPhysicalDeviceSurfacePresentModesKHR(physical_device, surface, &present_mode_count, nullptr),
            "Failed to get number of surface present modes."
        );

        // Get surface present modes
        std::vector<VkPresentModeKHR> present_modes{present_mode_count};
        vulkan_assert(
            vkGetPhysicalDeviceSurfacePresentModesKHR(physical_device, surface, &present_mode_count, present_modes.data()),
            "Failed to get surface present modes."
        );
        return present_modes;
    }

    inline VkSurfaceCapabilitiesKHR get_surface_capabilities(
        VkPhysicalDevice physical_device,
        VkSurfaceKHR     surface) {

        // Get surface capabilities
        VkSurfaceCapabilitiesKHR surface_capabilities;
        vulkan_assert(
            vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physical_device, surface, &surface_capabilities),
            "Failed to get surface capabilities."
        );
        return surface_capabilities;
    }

    inline VkExtent2D get_surface_extent(
        VkSurfaceCapabilitiesKHR surface_capabilities,
        uint32_t                 width,
        uint32_t                 height) {

        VkExtent2D surface_extent;
        if (surface_capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
            surface_extent = surface_capabilities.currentExtent;
        } else {
            surface_extent.width = std::max(surface_capabilities.minImageExtent.width, std::min(surface_capabilities.maxImageExtent.width, width));
            surface_extent.height = std::max(surface_capabilities.minImageExtent.height, std::min(surface_capabilities.maxImageExtent.height, height));
        }
        return surface_extent;
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
        VkSurfaceKHR     surface) {

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
                if (get_surface_present_support(physical_device, i, surface)) {
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