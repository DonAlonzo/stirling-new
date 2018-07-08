#pragma once

#include "vulkan/instance.hpp"
#include "window.hpp"

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>

namespace stirling {

    struct Vertex {
        glm::vec3 position;
        glm::vec3 color;
    };

    struct UniformBufferObject {
        glm::mat4 model;
        glm::mat4 view;
        glm::mat4 projection;
    };

    struct StirlingInstance {
        StirlingInstance(uint32_t width, uint32_t height);

    private:
        Window                           window;
        vulkan::Instance                 instance;
        vulkan::DebugReportCallback      debugger;
        vulkan::PhysicalDevice           physical_device;
        vulkan::Surface                  surface;
        vulkan::SurfaceFormat            surface_format;
        vulkan::Extent2D                 surface_extent;
        vulkan::QueueFamilyIndices       surface_queues;
        vulkan::Device                   device;
        vulkan::Queue                    graphics_queue;
        vulkan::Queue                    present_queue;
        vulkan::DescriptorSetLayout      descriptor_set_layout;
        vulkan::PipelineLayout           pipeline_layout;
        vulkan::CommandPool              command_pool;
        vulkan::Swapchain                swapchain;
        std::vector<vulkan::ImageView>   image_views;
        vulkan::RenderPass               render_pass;
        vulkan::Pipeline                 pipeline;    
        std::vector<vulkan::Framebuffer> framebuffers;

        vulkan::Instance                 create_instance() const;
        vulkan::DebugReportCallback      create_debugger() const;
        vulkan::PhysicalDevice           pick_physical_device() const;
        vulkan::Device                   create_device() const;
        vulkan::DescriptorSetLayout      create_descriptor_set_layout() const;
        vulkan::PipelineLayout           create_pipeline_layout() const;
        vulkan::CommandPool              create_command_pool() const;
        vulkan::SurfaceFormat            get_surface_format() const;
        vulkan::Extent2D                 get_surface_extent(uint32_t width, uint32_t height) const;
        vulkan::Swapchain                create_swapchain() const;
        std::vector<vulkan::ImageView>   create_image_views() const;
        vulkan::RenderPass               create_render_pass() const;
        vulkan::Pipeline                 create_pipeline() const;    
        std::vector<vulkan::Framebuffer> create_framebuffers() const;
    };

}