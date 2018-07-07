#pragma once

#include <GLFW/glfw3.h>

#include <vector>

namespace stirling {

    struct Window {
        Window(uint32_t width, uint32_t height, const char* title);
        ~Window();

        Window(const Window&) = delete;
        Window(Window&&);
        Window& operator=(const Window&) = delete;
        Window& operator=(Window&&);

        operator GLFWwindow*() const;

        std::vector<const char*> get_required_instance_extensions() const;
        bool should_close() const;

    private:
        GLFWwindow* window;
    };

};