#include "instance.hpp"

namespace stirling { namespace vulkan {

    inline Deleter<VkInstance> create_instance(
        const InstanceCreateInfo& create_info) {
        
        return create<VkInstance>(
            vkCreateInstance,
            vkDestroyInstance,
            "Failed to create instance.",
            create_info
        );
    }
    
    Instance::Instance(const InstanceCreateInfo& create_info) :
        instance (vulkan::create_instance(create_info)) {
    }

    Deleter<VkDebugReportCallbackEXT> Instance::create_debug_report_callback(
        const DebugReportCallbackCreateInfoEXT& create_info) const {
        
        return vulkan::create_debug_report_callback(create_info, instance);
    }

    Deleter<VkSurfaceKHR> Instance::create_surface(
        GLFWwindow* window) const {

        return vulkan::create_surface(instance, window);
    }

    std::vector<PhysicalDevice> Instance::get_physical_devices() const {
        const auto physical_devices = vulkan::get_physical_devices(instance);
        return std::vector<PhysicalDevice>{physical_devices.begin(), physical_devices.end()};
    }
}}