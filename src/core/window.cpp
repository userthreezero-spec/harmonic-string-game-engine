#include "core/window.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>

namespace hse {

void framebufferSizeCallback(GLFWwindow* window, int width, int height) {
    auto* win = static_cast<Window*>(glfwGetWindowUserPointer(window));
    if (win) {
        win->m_width = width;
        win->m_height = height;
        if (win->m_resizeCallback) {
            win->m_resizeCallback(width, height);
        }
    }
}

Window::Window(const WindowProps& props)
    : m_width(props.width), m_height(props.height)
{
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    m_window = glfwCreateWindow(m_width, m_height, props.title.c_str(), nullptr, nullptr);
    if (!m_window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return;
    }

    glfwMakeContextCurrent(m_window);
    glfwSetWindowUserPointer(m_window, this);
    glfwSetFramebufferSizeCallback(m_window, framebufferSizeCallback);

    if (props.vsync) {
        glfwSwapInterval(1);
    }
}

Window::~Window() {
    if (m_window) {
        glfwDestroyWindow(m_window);
        m_window = nullptr;
    }
    // Note: glfwTerminate() should be called once at process exit
    // For v0.1, we rely on OS cleanup
}

bool Window::shouldClose() const {
    if (!m_window) return true;
    return glfwWindowShouldClose(m_window);
}

void Window::pollEvents() {
    if (m_window) glfwPollEvents();
}

void Window::swapBuffers() {
    if (m_window) glfwSwapBuffers(m_window);
}

void Window::setResizeCallback(std::function<void(int, int)> callback) {
    m_resizeCallback = callback;
}

float Window::getDeltaTime() {
    double currentTime = glfwGetTime();
    float dt = static_cast<float>(currentTime - m_lastFrameTime);
    m_lastFrameTime = currentTime;
    return dt;
}

} // namespace hse
