#pragma once

#include <vulkan/vulkan.h>

#include <vector>

namespace stirling { namespace vulkan {

    template<typename To, typename From>
    inline To cast_vector(const std::vector<From>& from) {
        return from.size() > 0 ? std::vector<To>{from.begin(), from.end()}[0] : nullptr;
    }

    inline void vulkan_assert(VkResult result, const char* assertion_message) {
        switch (result) {
        case VK_SUCCESS: return;
        default: throw assertion_message;
        }
    }

}}