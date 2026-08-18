#include "renderer/renderer.h"
#include "scene/scene.h"
#include "scene/camera.h"
#include "scene/primitive.h"
#include "math/vec3.h"
#include <GL/glew.h>
#include <iostream>
#include <fstream>
#include <sstream>

namespace hse {

static const char* vertexShaderSource = R"(
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aUV;
uniform mat4 uModel;
uniform mat4 uView;
uniform mat4 uProjection;
out vec2 vUV;
void main() {
    gl_Position = uProjection * uView * uModel * vec4(aPos, 1.0);
    vUV = aUV;
}
)";

static const char* fragmentShaderSource = R"(
#version 330 core
out vec4 FragColor;
in vec2 vUV;
uniform vec3 uColor;
uniform sampler2D uTexture;
uniform bool uHasTexture;
void main() {
    if (uHasTexture) {
        FragColor = texture(uTexture, vUV) * vec4(uColor, 1.0);
    } else {
        FragColor = vec4(uColor, 1.0);
    }
}
)";

struct Renderer::RendererState {
    unsigned int shaderProgram = 0;
    float clearColorR = 0.1f;
    float clearColorG = 0.1f;
    float clearColorB = 0.15f;
};

Renderer::Renderer() : m_state(std::make_unique<RendererState>()) {}

Renderer::~Renderer() {
    shutdown();
}

bool Renderer::initialize() {
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        std::cerr << "Failed to initialize GLEW" << std::endl;
        return false;
    }

    glEnable(GL_DEPTH_TEST);
    glViewport(0, 0, 1280, 720);

    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, nullptr);
    glCompileShader(vertexShader);

    int success;
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetShaderInfoLog(vertexShader, 512, nullptr, infoLog);
        std::cerr << "Vertex shader compilation failed: " << infoLog << std::endl;
        glDeleteShader(vertexShader);
        return false;
    }

    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, nullptr);
    glCompileShader(fragmentShader);

    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetShaderInfoLog(fragmentShader, 512, nullptr, infoLog);
        std::cerr << "Fragment shader compilation failed: " << infoLog << std::endl;
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
        return false;
    }

    m_state->shaderProgram = glCreateProgram();
    glAttachShader(m_state->shaderProgram, vertexShader);
    glAttachShader(m_state->shaderProgram, fragmentShader);
    glLinkProgram(m_state->shaderProgram);

    glGetProgramiv(m_state->shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetProgramInfoLog(m_state->shaderProgram, 512, nullptr, infoLog);
        std::cerr << "Shader program linking failed: " << infoLog << std::endl;
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
        glDeleteProgram(m_state->shaderProgram);
        m_state->shaderProgram = 0;
        return false;
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    std::cout << "Renderer initialized successfully" << std::endl;
    return true;
}

void Renderer::shutdown() {
    if (m_state->shaderProgram) {
        glDeleteProgram(m_state->shaderProgram);
        m_state->shaderProgram = 0;
    }
}

void Renderer::beginFrame() {
    glClearColor(m_state->clearColorR, m_state->clearColorG, m_state->clearColorB, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Renderer::renderScene(const Scene& scene, const Camera& camera) {
    glUseProgram(m_state->shaderProgram);

    GLint viewLoc = glGetUniformLocation(m_state->shaderProgram, "uView");
    GLint projLoc = glGetUniformLocation(m_state->shaderProgram, "uProjection");
    GLint modelLoc = glGetUniformLocation(m_state->shaderProgram, "uModel");
    GLint colorLoc = glGetUniformLocation(m_state->shaderProgram, "uColor");
    GLint hasTexLoc = glGetUniformLocation(m_state->shaderProgram, "uHasTexture");
    GLint texLoc = glGetUniformLocation(m_state->shaderProgram, "uTexture");

    Mat4 view = camera.getViewMatrix();
    Mat4 proj = camera.getProjectionMatrix();

    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, view.ptr());
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, proj.ptr());

    for (const auto& prim : scene.getPrimitives()) {
        Mat4 model = Mat4::translate(prim->getPosition())
                   * Mat4::rotate(prim->getRotation().x, {1, 0, 0})
                   * Mat4::rotate(prim->getRotation().y, {0, 1, 0})
                   * Mat4::rotate(prim->getRotation().z, {0, 0, 1})
                   * Mat4::scale(prim->getScale());
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, model.ptr());
        Vec3 color = prim->getColor();
        glUniform3f(colorLoc, color.x, color.y, color.z);

        if (prim->hasTexture()) {
            prim->getTexture()->bind(0);
            glUniform1i(hasTexLoc, 1);
            glUniform1i(texLoc, 0);
        } else {
            glUniform1i(hasTexLoc, 0);
        }

        prim->bind();
        glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(prim->getIndexCount()), GL_UNSIGNED_INT, 0);
        prim->unbind();
    }
}

void Renderer::endFrame() {}

void Renderer::setClearColor(float r, float g, float b, float a) {
    m_state->clearColorR = r;
    m_state->clearColorG = g;
    m_state->clearColorB = b;
}

void Renderer::getClearColor(float& r, float& g, float& b) const {
    r = m_state->clearColorR;
    g = m_state->clearColorG;
    b = m_state->clearColorB;
}

void Renderer::setViewport(int x, int y, int width, int height) {
    glViewport(x, y, width, height);
}

bool Renderer::readPixels(std::vector<uint8_t>& rgba, int& width, int& height) {
    GLint viewport[4];
    glGetIntegerv(GL_VIEWPORT, viewport);
    width = viewport[2];
    height = viewport[3];

    if (width <= 0 || height <= 0) return false;

    size_t pixelCount = static_cast<size_t>(width) * static_cast<size_t>(height);
    rgba.resize(pixelCount * 4);

    glReadPixels(viewport[0], viewport[1], width, height, GL_RGBA, GL_UNSIGNED_BYTE, rgba.data());

    return !rgba.empty();
}

} // namespace hse
