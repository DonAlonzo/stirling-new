#include "instance.hpp"

namespace stirling { namespace vulkan {

    inline Deleter<VkInstance> create_instance(
        const InstanceCreateInfo& create_info) {
        
        const VkInstanceCreateInfo vk_create_info {
            .sType                   = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
            .pApplicationInfo        = create_info.application_info,
            .enabledLayerCount       = static_cast<uint32_t>(create_info.enabled_layers.size()),
            .ppEnabledLayerNames     = create_info.enabled_layers.data(),
            .enabledExtensionCount   = static_cast<uint32_t>(create_info.enabled_extensions.size()),
            .ppEnabledExtensionNames = create_info.enabled_extensions.data()
        };

        return create<VkInstance>(
            vkCreateInstance,
            vkDestroyInstance,
            "Failed to create instance.",
            &vk_create_info
        );
    }
    
    Instance::Instance(const InstanceCreateInfo& create_info) :
        instance (vulkan::create_instance(create_info)) {
    }

    Deleter<VkDebugReportCallbackEXT> Instance::create_debug_report_callback(
        const DebugReportCallbackCreateInfoEXT& create_info) const {
        
        return vulkan::create_debug_report_callback(create_info, instance);
    }

    Surface Instance::create_surface(
        GLFWwindow* window) const {

        return vulkan::create_surface(instance, window);
    }

    std::vector<PhysicalDevice> Instance::get_physical_devices() const {
        const auto physical_devices = vulkan::get_physical_devices(instance);
        return std::vector<PhysicalDevice>{physical_devices.begin(), physical_devices.end()};
    }
}}