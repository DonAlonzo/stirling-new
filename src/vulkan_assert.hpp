#pragma once

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