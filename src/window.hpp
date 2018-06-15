#pragma once

#include <GLFW/glfw3.h>

struct Window {
    Window(uint32_t width, uint32_t height, const char* title);
    ~Window();

    Window(const Window&) = delete;
    Window(Window&&);
    Window& operator=(const Window&) = delete;
    Window& operator=(Window&&);

    operator GLFWwindow*() const;

private:
    GLFWwindow* window;
};