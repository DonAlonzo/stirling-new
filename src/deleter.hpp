#pragma once

#include <vulkan/vulkan.h>

#include <functional>
#include <memory>
#include <mutex>

struct ReferenceCounter {
    operator size_t();
    size_t operator++();
    size_t operator--();
private:
    std::mutex mutex;
    size_t count = 1;
};

template<typename Handle>
struct Deleter {
    Deleter() = default;

    Deleter(void (*deleter)(Handle, const VkAllocationCallbacks*)) :
        deleter ([=]() { deleter(this->handle, nullptr); }) {
    }

    template<typename Parent>
    Deleter(void (*deleter)(Parent, Handle, const VkAllocationCallbacks*), Parent parent) :
        deleter ([=]() { deleter(parent, this->handle, nullptr); }) {
    }

    template<typename Parent>
    Deleter(void (*deleter)(Parent, Handle, const VkAllocationCallbacks*), Deleter<Parent> parent) :
        deleter ([=]() { deleter(parent, this->handle, nullptr); }) {
    }

    Deleter(const Deleter& rhs) :
        handle (rhs.handle),
        reference_counter (rhs.reference_counter),
        deleter (rhs.deleter) {

        ++(*reference_counter);
    }

    Deleter& operator=(const Deleter& rhs) {
        check_delete();

        handle = rhs.handle;
        reference_counter = rhs.reference_counter;
        deleter = rhs.deleter;

        ++(*reference_counter);

        return *this;
    }

    Deleter(Deleter&& rhs) :
        handle (std::move(rhs.handle)),
        reference_counter (std::move(rhs.reference_counter)),
        deleter (std::move(rhs.deleter)) {

        rhs.handle = VK_NULL_HANDLE;
    }

    Deleter& operator=(Deleter&& rhs) {
        check_delete();
        
        handle = std::move(rhs.handle);
        reference_counter = std::move(rhs.reference_counter);
        deleter = (rhs.deleter);

        rhs.handle = VK_NULL_HANDLE;

        return *this;
    }

    ~Deleter() {
        check_delete();
    }

    void replace(Handle handle) {
        check_delete();
        this->handle = handle;
    }

    Handle* replace() {
        check_delete();
        return &handle;
    }

    operator Handle() const { return handle; }
    operator bool() const { return handle != nullptr; }

private:
    Handle handle = VK_NULL_HANDLE;
    std::shared_ptr<ReferenceCounter> reference_counter = std::make_shared<ReferenceCounter>();
    std::function<void()> deleter;

    void check_delete() {
        if (handle != VK_NULL_HANDLE && deleter && --(*reference_counter) == 0) {
            deleter();
            deleter = nullptr;
            handle = VK_NULL_HANDLE;
        }
    }
};