#pragma once

template<typename T>
struct Wrapper {
    Wrapper(T t) : t (t) {}
    operator T*() { return &t; }

private:
    T t;
};

inline void vulkan_assert(VkResult result, const char* assertion_message) {
    switch (result) {
    case VK_SUCCESS: return;
    default: throw assertion_message;
    }
}

template<typename Handle, typename Create, typename Destroy, typename... Args>
inline Deleter<Handle> vulkan_create(
    Create      create,
    Destroy     destroy,
    const char* assertion_message,
    Args...     args) {

    Deleter<Handle> handle{destroy};
    vulkan_assert(create(args..., handle.replace()), assertion_message);
    return handle;
}

template<typename Handle, typename Create, typename Destroy, typename Parent, typename... Args>
inline Deleter<Handle> vulkan_create(
    Create      create,
    Destroy     destroy,
    Parent      parent,
    const char* assertion_message,
    Args...     args) {

    Deleter<Handle> handle{destroy, parent};
    vulkan_assert(create(parent, args..., handle.replace()), assertion_message);
    return handle;
}

inline Deleter<VkInstance> vulkan_create_instance(const VkInstanceCreateInfo& create_info) {
    return vulkan_create<VkInstance>(
        vkCreateInstance,
        vkDestroyInstance,
        "Failed to create instance.",
        &create_info,
        nullptr
    );
}

inline Deleter<VkDevice> vulkan_create_device(const VkDeviceCreateInfo& create_info, VkPhysicalDevice physical_device) {
    return vulkan_create<VkDevice>(
        vkCreateDevice,
        vkDestroyDevice,
        "Failed to create device.",
        physical_device,
        &create_info,
        nullptr
    );
}

inline Deleter<VkSwapchainKHR> vulkan_create_swapchain(const VkSwapchainCreateInfoKHR& create_info, VkDevice device) {
    return vulkan_create<VkSwapchainKHR>(
        vkCreateSwapchainKHR,
        vkDestroySwapchainKHR,
        device,
        "Failed to create swapchain.",
        &create_info,
        nullptr
    );
}