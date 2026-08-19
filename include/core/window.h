#pragma once
#include <string>
#include <functional>

struct GLFWwindow;

namespace hse {

struct WindowProps {
    std::string title = "Harmonic String Engine";
    int width = 1280;
    int height = 720;
    bool vsync = true;
};

class Window {
public:
    Window(const WindowProps& props = WindowProps{});
    ~Window();

    Window(const Window&) = delete;
    Window& operator=(const Window&) = delete;
    Window(Window&&) = delete;
    Window& operator=(Window&&) = delete;

    bool isValid() const { return m_window != nullptr; }
    bool shouldClose() const;
    void pollEvents();
    void swapBuffers();
    float getDeltaTime();

    int getWidth() const { return m_width; }
    int getHeight() const { return m_height; }
    GLFWwindow* getNative() const { return m_window; }

    bool isMouseButtonPressed(int button) const;
    bool isKeyPressed(int key) const;
    void getMousePosition(double& x, double& y) const;
    float getScrollX() const { return m_scrollX; }
    float getScrollY() const { return m_scrollY; }
    void resetScroll() { m_scrollX = 0; m_scrollY = 0; }

    void setResizeCallback(std::function<void(int, int)> callback);

private:
    friend void framebufferSizeCallback(GLFWwindow* window, int width, int height);
    friend void scrollCallback(GLFWwindow* window, double xoffset, double yoffset);

    GLFWwindow* m_window = nullptr;
    int m_width = 0;
    int m_height = 0;
    float m_scrollX = 0;
    float m_scrollY = 0;
    double m_lastFrameTime = 0.0;
    std::function<void(int, int)> m_resizeCallback;
};

} // namespace hse
