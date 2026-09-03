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

class UIRenderer {
public:
    UIRenderer();
    ~UIRenderer();

    bool initialize();
    void shutdown();

    void begin2D(int windowWidth, int windowHeight);
    void end2D();

    void drawRect(float x, float y, float w, float h, const Vec3& color, float alpha = 1.0f);
    void drawRectOutline(float x, float y, float w, float h, const Vec3& color, float thickness = 1.5f);
    void drawText(const std::string& text, float x, float y, float scale = 1.0f, const Vec3& color = {1.0f, 1.0f, 1.0f}, float alpha = 1.0f);

private:
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
