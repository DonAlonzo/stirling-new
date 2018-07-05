#pragma once

#include "deleter.hpp"
#include "physical_device.hpp"
#include "vulkan_structs.hpp"
#include "vulkan.hpp"

#include <vulkan/vulkan.h>

#include <GLFW/glfw3.h>

#include <vector>

namespace stirling { namespace vulkan {

    struct InstanceCreateInfo {
        ApplicationInfo          application_info;
        std::vector<const char*> enabled_layers;
        std::vector<const char*> enabled_extensions;
    };

    struct Instance {
        Instance(const InstanceCreateInfo& create_info);

        inline operator const VkInstance() const {
            return instance;
        }
        
        Deleter<VkDebugReportCallbackEXT> create_debug_report_callback(const DebugReportCallbackCreateInfoEXT& create_info) const;
        Deleter<VkSurfaceKHR> create_surface(GLFWwindow* window) const;
        std::vector<PhysicalDevice> get_physical_devices() const;

    private:
        Deleter<VkInstance> instance;
    };

}}