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

}}