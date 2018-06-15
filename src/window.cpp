#include "window.hpp"

Window::Window(uint32_t width, uint32_t height, const char* title) {
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    //glfwWindowHint(GLFW_DECORATED, GLFW_FALSE);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    window = glfwCreateWindow(width, height, title, nullptr/*glfwGetPrimaryMonitor()*/, nullptr);
    if (!window) throw "Failed to create window.";
    glfwSetWindowPos(window, 0, 0);
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