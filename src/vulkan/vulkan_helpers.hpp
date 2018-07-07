#pragma once

#include <vulkan/vulkan.h>

#include <functional>
#include <vector>

namespace stirling { namespace vulkan {

    template<typename To, typename From>
    inline To cast_vector(const std::vector<From>& from) {
        return from.size() > 0 ? std::vector<To>{from.begin(), from.end()}[0] : nullptr;
    }

    template<typename To, typename From, typename Transformer>
    inline std::vector<To> cast_vector(const std::vector<From>& from, Transformer transformer) {
        std::vector<To> to;
        to.reserve(from.size());
        std::transform(
            from.begin(),
            from.end(),
            std::back_inserter(to),
            transformer
        );
        return to;
    }

    inline void vulkan_assert(VkResult result, const char* assertion_message) {
        switch (result) {
        case VK_SUCCESS: return;
        default: throw assertion_message;
        }
    }

}}