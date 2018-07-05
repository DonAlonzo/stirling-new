#include "instance.hpp"
#include "vulkan.hpp"

namespace stirling { namespace vulkan {

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

}}