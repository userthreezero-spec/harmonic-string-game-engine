#pragma once
#include "math/vec3.h"
#include "math/mat4.h"
#include <string>
#include <vector>
#include <memory>
#include <cstdint>

namespace hse {

struct UIChatMessage {
    std::string sender; // "HUMAN", "WEBOS", "SYSTEM", "PROPOSAL"
    std::string text;
    Vec3 color{1.0f, 1.0f, 1.0f};
    std::string timestamp;
};

struct UIProposedChange {
    bool active = false;
    uint64_t targetObjectID = 0;
    std::string targetName;
    std::string propertyName;
    Vec3 oldColor;
    Vec3 newColor;
    std::string colorName;
    std::string reason;
};

struct UILayout {
    int winW = 1280;
    int winH = 720;
    int fbW = 1280;
    int fbH = 720;
    float dpiScale = 1.0f;

    float panelWidth = 400.0f;
    float panelX = 880.0f;

    float fontScale = 1.0f;
    float charW = 8.0f;
    float charH = 16.0f;
    float lineHeight = 20.0f;

    float padding = 12.0f;
    float inspHeight = 110.0f;
    float chipHeight = 26.0f;
    float inputHeight = 40.0f;

    void update(int windowW, int windowH, int framebufferW = 0, int framebufferH = 0) {
        winW = (windowW > 0) ? windowW : 1280;
        winH = (windowH > 0) ? windowH : 720;
        fbW = (framebufferW > 0) ? framebufferW : winW;
        fbH = (framebufferH > 0) ? framebufferH : winH;

        dpiScale = static_cast<float>(winH) / 720.0f;
        if (dpiScale < 1.0f) dpiScale = 1.0f;

        panelWidth = (winW * 0.32f > 380.0f) ? (winW * 0.32f) : 380.0f;
        if (panelWidth > 800.0f) panelWidth = 800.0f;
        panelX = winW - panelWidth;

        fontScale = (dpiScale < 1.25f) ? 1.0f : ((dpiScale < 1.75f) ? 1.4f : ((dpiScale < 2.5f) ? 1.8f : 2.4f));
        charW = 8.0f * fontScale;
        charH = 16.0f * fontScale;
        lineHeight = charH + 4.0f;

        padding = 12.0f * fontScale;
        inspHeight = 100.0f * fontScale;
        chipHeight = 26.0f * fontScale;
        inputHeight = 38.0f * fontScale;
    }
};

class UIRenderer {
public:
    UIRenderer();
    ~UIRenderer();

    bool initialize();
    void shutdown();

    void updateLayout(int winW, int winH, int fbW = 0, int fbH = 0) {
        m_layout.update(winW, winH, fbW, fbH);
    }
    const UILayout& getLayout() const { return m_layout; }

    void begin2D(int windowWidth, int windowHeight);
    void end2D();

    void drawRect(float x, float y, float w, float h, const Vec3& color, float alpha = 1.0f);
    void drawRectOutline(float x, float y, float w, float h, const Vec3& color, float thickness = 1.5f);
    void drawText(const std::string& text, float x, float y, float scale = 1.0f, const Vec3& color = {1.0f, 1.0f, 1.0f}, float alpha = 1.0f);

private:
    UILayout m_layout;
    unsigned int m_shaderProgram = 0;
    unsigned int m_fontTexture = 0;
    unsigned int m_vao = 0;
    unsigned int m_vbo = 0;
    int m_locOrtho = -1;
    int m_locColor = -1;
    int m_locAlpha = -1;
    int m_locUseTexture = -1;
    bool m_initialized = false;

    void initFontTexture();
};

} // namespace hse
