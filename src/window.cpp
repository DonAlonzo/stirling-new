#include "window.hpp"

namespace stirling {

    Window::Window(uint32_t width, uint32_t height, const char* title) {
        glfwInit();
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        //glfwWindowHint(GLFW_DECORATED, GLFW_FALSE);
        glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
        glfwWindowHint(GLFW_FLOATING, GLFW_TRUE);

        const auto monitor = glfwGetPrimaryMonitor();
        const auto mode = glfwGetVideoMode(monitor);
        glfwWindowHint(GLFW_RED_BITS, mode->redBits);
        glfwWindowHint(GLFW_GREEN_BITS, mode->greenBits);
        glfwWindowHint(GLFW_BLUE_BITS, mode->blueBits);
        glfwWindowHint(GLFW_REFRESH_RATE, mode->refreshRate);

        window = glfwCreateWindow(width, height, title, nullptr/*monitor*/, nullptr);
        if (!window) throw "Failed to create window.";

        //glfwSetWindowPos(window, 0, 0);
        glfwSetWindowPos(window, (mode->width - width) / 2, (mode->height - height) / 2);

        //glfwSetWindowUserPointer(window, this);
        //glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    }

    Window::~Window() {
        if (window != nullptr) {
            glfwDestroyWindow(window);
            glfwTerminate();
        }
    }

    Window::Window(Window&& rhs) :
        window (rhs.window) {

        rhs.window = nullptr;
    }

    Window& Window::operator=(Window&& rhs) {
        window = rhs.window;
        rhs.window = nullptr;

        return *this;
    }

    Window::operator GLFWwindow*() const {
        return window;
    }

    std::vector<const char*> Window::get_required_instance_extensions() const {
        // Get required extensions for GLFW
        uint32_t required_glfw_extensions_count = 0;
        const auto required_glfw_extensions = glfwGetRequiredInstanceExtensions(&required_glfw_extensions_count);
        return {required_glfw_extensions, required_glfw_extensions + required_glfw_extensions_count};
    }

    bool Window::should_close() const {
        glfwPollEvents();
        return glfwWindowShouldClose(window);
    }

}