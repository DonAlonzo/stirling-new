#pragma once

#include "deleter.hpp"
#include "file.hpp"
#include "vulkan_helpers.hpp"
#include "vulkan_structs.hpp"

#include <vulkan/vulkan.h>

#include <GLFW/glfw3.h>

namespace stirling { namespace vulkan {

    template<typename Handle, typename Create, typename Destroy, typename... Args>
    inline Deleter<Handle> create(
        Create      create,
        Destroy     destroy,
        const char* assertion_message,
        Args&&...   args) {

        Deleter<Handle> handle{destroy};
        vulkan_assert(create(std::forward<Args>(args)..., nullptr, handle.replace()), assertion_message);
        return handle;
    }

    template<typename Handle, typename Create, typename Destroy, typename Parent, typename... Args>
    inline Deleter<Handle> create(
        Create      create,
        Destroy     destroy,
        Parent      parent,
        const char* assertion_message,
        Args&&...   args) {

        Deleter<Handle> handle{destroy, parent};
        vulkan_assert(create(parent, std::forward<Args>(args)..., nullptr, handle.replace()), assertion_message);
        return handle;
    }

    inline Deleter<VkDebugReportCallbackEXT> create_debug_report_callback(
        const DebugReportCallbackCreateInfoEXT& create_info,
        VkInstance                              instance) {

        const auto create_function = reinterpret_cast<PFN_vkCreateDebugReportCallbackEXT>(vkGetInstanceProcAddr(instance, "vkCreateDebugReportCallbackEXT"));
        const auto destroy_function = reinterpret_cast<PFN_vkDestroyDebugReportCallbackEXT>(vkGetInstanceProcAddr(instance, "vkDestroyDebugReportCallbackEXT"));
        if (create_function == nullptr || destroy_function == nullptr) throw "Debug report callback extension not present.";
        return create<VkDebugReportCallbackEXT>(
            create_function,
            destroy_function,
            instance,
            "Failed to create debug report callback.",
            create_info
        );
    }

    inline Deleter<VkSurfaceKHR> create_surface(
        VkInstance  instance,
        GLFWwindow* window) {

        return create<VkSurfaceKHR>(
            glfwCreateWindowSurface,
            vkDestroySurfaceKHR,
            instance,
            "Failed to create surface.",
            window
        );
    }

    inline Deleter<VkShaderModule> create_shader_module(
        const VkShaderModuleCreateInfo& create_info,
        VkDevice                        device) {

        return create<VkShaderModule>(
            vkCreateShaderModule,
            vkDestroyShaderModule,
            device,
            "Failed to create shader module.",
            &create_info
        );
    }

    inline Deleter<VkShaderModule> create_shader_module(
        const char* file_name,
        VkDevice    device) {

        const auto code = read_file(file_name);
        return create_shader_module({
            .sType    = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
            .codeSize = code.size(),
            .pCode    = reinterpret_cast<const uint32_t*>(code.data())
        }, device);
    }

    inline Deleter<VkFramebuffer> create_framebuffer(
        const FramebufferCreateInfo& create_info,
        VkDevice                     device) {

        return create<VkFramebuffer>(
            vkCreateFramebuffer,
            vkDestroyFramebuffer,
            device,
            "Failed to create framebuffer.",
            create_info
        );
    }

    inline Deleter<VkSemaphore> create_semaphore(
        const VkSemaphoreCreateInfo& create_info,
        VkDevice                     device) {

        return create<VkSemaphore>(
            vkCreateSemaphore,
            vkDestroySemaphore,
            device,
            "Failed to create semaphore.",
            &create_info
        );
    }

    inline Deleter<VkSemaphore> create_semaphore(
        VkDevice device) {

        return create_semaphore({
            .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
        }, device);
    }

    inline std::vector<Deleter<VkSemaphore>> create_semaphores(
        VkDevice device,
        size_t   count) {

        std::vector<Deleter<VkSemaphore>> semaphores{count};
        for (size_t i = 0; i < count; ++i) {
            semaphores[i] = create_semaphore(device);
        }
        return semaphores;
    }

    inline Deleter<VkFence> create_fence(
        const VkFenceCreateInfo& create_info,
        VkDevice                 device) {

        return create<VkFence>(
            vkCreateFence,
            vkDestroyFence,
            device,
            "Failed to create fence.",
            &create_info
        );
    }

    inline Deleter<VkFence> create_fence(
        VkDevice device,
        bool     signaled = false) {

        return create_fence({
            .sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
            .flags = signaled ? VK_FENCE_CREATE_SIGNALED_BIT : 0u
        }, device);
    }

    inline std::vector<Deleter<VkFence>> create_fences(
        VkDevice device,
        size_t   count,
        bool     signaled = false) {

        std::vector<Deleter<VkFence>> fences{count};
        for (size_t i = 0; i < count; ++i) {
            fences[i] = create_fence(device, signaled);
        }
        return fences;
    }

}}