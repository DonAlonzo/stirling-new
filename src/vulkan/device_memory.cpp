#include "device_memory.hpp"
#include "vulkan.hpp"

#include <cstring>

namespace stirling { namespace vulkan {

    inline Deleter<VkDeviceMemory> allocate_memory(
        const MemoryAllocateInfo& allocate_info,
        VkDevice                  device) {

        return create<VkDeviceMemory>(
            vkAllocateMemory,
            vkFreeMemory,
            device,
            "Failed to allocate memory.",
            allocate_info
        );
    }

    DeviceMemory::DeviceMemory(
        const MemoryAllocateInfo& allocate_info,
        VkDevice                  device) :

        memory (allocate_memory(allocate_info, device)),
        device (device) {
    }

    DeviceMemoryMapping DeviceMemory::map(VkDeviceSize offset, VkDeviceSize size) const {
        return {device, memory, offset, size};
    }

    DeviceMemoryMapping::DeviceMemoryMapping(
        VkDevice       device,
        VkDeviceMemory memory,
        VkDeviceSize   offset,
        VkDeviceSize   size) :

        deleter ([device, memory]() { vkUnmapMemory(device, memory); }) {

        vkMapMemory(device, memory, offset, size, 0, &data);
    }

    DeviceMemoryMapping::~DeviceMemoryMapping() {
        if (data) deleter();
    }

    void DeviceMemoryMapping::copy(const void* src, size_t size) {
        memcpy(data, src, size);
    }

}}