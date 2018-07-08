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
        Window                      window;
        vulkan::Instance            instance;
        vulkan::DebugReportCallback debugger;
        vulkan::Surface             surface;
        vulkan::PhysicalDevice      physical_device;
        vulkan::QueueFamilyIndices  queue_families;
        vulkan::Device              device;
        vulkan::Queue               graphics_queue;
        vulkan::Queue               present_queue;
        vulkan::DescriptorSetLayout descriptor_set_layout;
        vulkan::PipelineLayout      pipeline_layout;
        vulkan::CommandPool         command_pool;
        vulkan::SurfaceFormat       surface_format;

        vulkan::Instance            create_instance() const;
        vulkan::DebugReportCallback create_debugger() const;
        vulkan::PhysicalDevice      pick_physical_device() const;
        vulkan::Device              create_device() const;
        vulkan::DescriptorSetLayout create_descriptor_set_layout() const;
        vulkan::PipelineLayout      create_pipeline_layout() const;
        vulkan::CommandPool         create_command_pool() const;
        vulkan::SurfaceFormat       get_surface_format() const;
    };

}