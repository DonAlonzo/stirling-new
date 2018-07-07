#pragma once

#include <vulkan/vulkan.h>

#include <functional>
#include <memory>
#include <iostream>

template<typename Handle>
struct Deleter {
    Deleter() = default;

    Deleter(
        std::function<void(Handle)> deleter,
        Handle handle = VK_NULL_HANDLE) :
        
        deleter ([=]() { deleter(handle); }), 
        handle  (handle) {
    }

    Deleter(
        void (*deleter)(Handle, const VkAllocationCallbacks*),
        Handle handle = VK_NULL_HANDLE) :
        
        deleter ([=]() { deleter(handle, nullptr); }), 
        handle  (handle) {
    }

    template<typename Parent>
    Deleter(
        void (*deleter)(Parent, Handle, const VkAllocationCallbacks*),
        Parent parent,
        Handle handle = VK_NULL_HANDLE) :
        
        deleter ([=]() { deleter(parent, handle, nullptr); }),
        handle  (handle) {
    }

    template<typename Parent>
    Deleter(
        void          (*deleter)(Parent, Handle, const VkAllocationCallbacks*),
        Deleter<Parent> parent,
        Handle          handle = VK_NULL_HANDLE) :
        
        deleter ([=]() { deleter(parent, handle, nullptr); }),
        handle  (handle) {
    }

    Deleter(const Deleter& rhs) :
        handle            (rhs.handle),
        reference_counter (rhs.reference_counter),
        deleter           (rhs.deleter) {
    }

    Deleter& operator=(const Deleter& rhs) {
        check_delete();

        handle = rhs.handle;
        reference_counter = rhs.reference_counter;
        deleter = rhs.deleter;

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
        handle = handle;
    }

    Handle* replace() {
        check_delete();
        return &handle;
    }

    operator Handle() const { return handle; }
    operator bool() const { return handle != VK_NULL_HANDLE; }

private:
    Handle handle = VK_NULL_HANDLE;
    std::shared_ptr<void> reference_counter{new uint8_t[0]};
    std::function<void()> deleter;

    void check_delete() {
        if (handle != VK_NULL_HANDLE && deleter && reference_counter.use_count() == 1) {
            // std::cout << typeid(Handle).name() << " destroyed\n";
            deleter();
            deleter = nullptr;
            handle = VK_NULL_HANDLE;
        }
    }
};