#pragma once

#include "deleter.hpp"
#include "vulkan_structs.hpp"

#include <vulkan/vulkan.h>

#include <GLFW/glfw3.h>

namespace stirling { namespace vulkan {

    struct Instance {
        Instance(const InstanceCreateInfo& create_info);

        inline operator const VkInstance() const { return instance; }
        
        Deleter<VkDebugReportCallbackEXT> create_debug_report_callback(
            const DebugReportCallbackCreateInfoEXT& create_info) const;
        
        Deleter<VkSurfaceKHR> create_surface(
            GLFWwindow* window) const;

    private:
        Deleter<VkInstance> instance;
    };

}}