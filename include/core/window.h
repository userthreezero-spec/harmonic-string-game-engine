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

    int getWidth() const { return m_width; }
    int getHeight() const { return m_height; }
    GLFWwindow* getNative() const { return m_window; }

    bool isKeyPressed(int key) const;
    bool isMouseButtonPressed(int button) const;

    float getDeltaTime();
    void getMousePosition(double& x, double& y) const;
    float getScrollY() const { return m_scrollY; }
    void resetScroll() { m_scrollY = 0.0f; }

    using KeyCallback = std::function<void(int key, int scancode, int action, int mods)>;
    using MouseButtonCallback = std::function<void(int button, int action, int mods)>;
    using CursorPosCallback = std::function<void(double xpos, double ypos)>;
    using ScrollCallback = std::function<void(double xoffset, double yoffset)>;
    using CharCallback = std::function<void(unsigned int codepoint)>;

    void setKeyCallback(KeyCallback callback) { m_keyCallback = callback; }
    void setMouseButtonCallback(MouseButtonCallback callback) { m_mouseButtonCallback = callback; }
    void setCursorPosCallback(CursorPosCallback callback) { m_cursorPosCallback = callback; }
    void setScrollCallback(ScrollCallback callback) { m_scrollCallback = callback; }
    void setCharCallback(CharCallback callback) { m_charCallback = callback; }
    void setResizeCallback(std::function<void(int, int)> callback) { m_resizeCallback = callback; }

private:
    friend void framebufferSizeCallback(GLFWwindow* window, int width, int height);

    GLFWwindow* m_window = nullptr;
    int m_width = 0;
    int m_height = 0;
    double m_lastFrameTime = 0.0;
    float m_scrollY = 0.0f;

    KeyCallback m_keyCallback;
    MouseButtonCallback m_mouseButtonCallback;
    CursorPosCallback m_cursorPosCallback;
    ScrollCallback m_scrollCallback;
    CharCallback m_charCallback;
    std::function<void(int, int)> m_resizeCallback;
};

} // namespace hse
