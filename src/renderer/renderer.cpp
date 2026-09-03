#include "renderer/renderer.h"
#include "renderer/material.h"
#include "scene/scene.h"
#include "scene/camera.h"
#include "scene/primitive.h"
#include "scene/light.h"
#include <GL/glew.h>
#include <iostream>
#include <fstream>
#include <sstream>

namespace hse {

static const int MAX_LIGHTS = 8;

static const char* vertexShaderSource = R"(
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
uniform mat4 uModel;
uniform mat4 uView;
uniform mat4 uProjection;
out vec3 vWorldPos;
out vec3 vNormal;
void main() {
    vec4 worldPos = uModel * vec4(aPos, 1.0);
    vWorldPos = worldPos.xyz;
    vNormal = mat3(uModel) * aNormal;
    gl_Position = uProjection * uView * worldPos;
}
)";

static const char* fragmentShaderSource = R"(
#version 330 core
in vec3 vWorldPos;
in vec3 vNormal;
out vec4 FragColor;
uniform vec3 uColor;
uniform vec3 uLightPositions[8];
uniform vec3 uLightColors[8];
uniform int uLightTypes[8];
uniform int uNumLights;
uniform float uAmbientStrength;
void main() {
    vec3 norm = normalize(vNormal);
    vec3 result = uAmbientStrength * uColor;
    for (int i = 0; i < uNumLights; i++) {
        vec3 lightDir;
        if (uLightTypes[i] == 0) {
            lightDir = normalize(uLightPositions[i] - vWorldPos);
        } else {
            lightDir = normalize(-uLightPositions[i]);
        }
        float diff = max(dot(norm, lightDir), 0.0);
        result += diff * uLightColors[i] * uColor;
    }
    FragColor = vec4(result, 1.0);
}
)";

struct Renderer::RendererState {
    unsigned int shaderProgram = 0;
    float clearColorR = 0.1f;
    float clearColorG = 0.1f;
    float clearColorB = 0.15f;
    size_t frameCount = 0;
    int locView = -1;
    int locProj = -1;
    int locModel = -1;
    int locColor = -1;
    int locNumLights = -1;
    int locAmbient = -1;
    int locLightPos[8] = {-1,-1,-1,-1,-1,-1,-1,-1};
    int locLightColor[8] = {-1,-1,-1,-1,-1,-1,-1,-1};
    int locLightType[8] = {-1,-1,-1,-1,-1,-1,-1,-1};
    bool uniformsCached = false;
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

    m_state->locView = glGetUniformLocation(m_state->shaderProgram, "uView");
    m_state->locProj = glGetUniformLocation(m_state->shaderProgram, "uProjection");
    m_state->locModel = glGetUniformLocation(m_state->shaderProgram, "uModel");
    m_state->locColor = glGetUniformLocation(m_state->shaderProgram, "uColor");
    m_state->locNumLights = glGetUniformLocation(m_state->shaderProgram, "uNumLights");
    m_state->locAmbient = glGetUniformLocation(m_state->shaderProgram, "uAmbientStrength");
    for (int i = 0; i < MAX_LIGHTS; ++i) {
        m_state->locLightPos[i] = glGetUniformLocation(m_state->shaderProgram, (std::string("uLightPositions[") + std::to_string(i) + "]").c_str());
        m_state->locLightColor[i] = glGetUniformLocation(m_state->shaderProgram, (std::string("uLightColors[") + std::to_string(i) + "]").c_str());
        m_state->locLightType[i] = glGetUniformLocation(m_state->shaderProgram, (std::string("uLightTypes[") + std::to_string(i) + "]").c_str());
    }
    m_state->uniformsCached = true;

    std::cout << "Renderer initialized successfully (uniforms cached)" << std::endl;
    return true;
}

void Renderer::shutdown() {
    if (m_state->shaderProgram) {
        glDeleteProgram(m_state->shaderProgram);
        m_state->shaderProgram = 0;
    }
}

void Renderer::beginFrame() {
    if (m_state) m_state->frameCount++;
    glClearColor(m_state->clearColorR, m_state->clearColorG, m_state->clearColorB, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Renderer::renderScene(const Scene& scene, const Camera& camera, uint64_t selectedID) {
    glUseProgram(m_state->shaderProgram);

    Mat4 view = camera.getViewMatrix();
    Mat4 proj = camera.getProjectionMatrix();

    glUniformMatrix4fv(m_state->locView, 1, GL_FALSE, view.ptr());
    glUniformMatrix4fv(m_state->locProj, 1, GL_FALSE, proj.ptr());

    const auto& lights = scene.getLights();
    int numLights = static_cast<int>(lights.size());
    if (numLights > MAX_LIGHTS) numLights = MAX_LIGHTS;
    glUniform1i(m_state->locNumLights, numLights);

    float minAmbient = 1.0f;
    for (int i = 0; i < numLights; i++) {
        glUniform3f(m_state->locLightPos[i], lights[i].position.x, lights[i].position.y, lights[i].position.z);
        glUniform3f(m_state->locLightColor[i], lights[i].color.x, lights[i].color.y, lights[i].color.z);
        glUniform1i(m_state->locLightType[i], static_cast<int>(lights[i].type));

        if (lights[i].ambient < minAmbient) minAmbient = lights[i].ambient;
    }
    if (numLights == 0) minAmbient = 0.2f;
    glUniform1f(m_state->locAmbient, minAmbient);

    for (const auto& prim : scene.getPrimitives()) {
        const Mat4& model = prim->getWorldTransform();
        glUniformMatrix4fv(m_state->locModel, 1, GL_FALSE, model.ptr());

        Vec3 c = prim->getColor();
        if (prim->getMaterial()) {
            c = prim->getMaterial()->getAlbedo();
        }

        // Selection highlight: blend bright cyan tint when object is selected
        if (selectedID != 0 && prim->getID() == selectedID) {
            c = c * 0.4f + Vec3{0.0f, 0.6f, 0.8f};
        }

        glUniform3f(m_state->locColor, c.x, c.y, c.z);

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
    if (m_state) { r = m_state->clearColorR; g = m_state->clearColorG; b = m_state->clearColorB; }
    else { r = 0; g = 0; b = 0; }
}

size_t Renderer::getFrameCount() const {
    return m_state ? m_state->frameCount : 0;
}

void Renderer::incrementFrameCount() {
    if (m_state) m_state->frameCount++;
}

bool Renderer::readPixels(std::vector<unsigned char>& rgba, int& w, int& h) {
    GLint viewport[4];
    glGetIntegerv(GL_VIEWPORT, viewport);
    w = viewport[2];
    h = viewport[3];
    if (w <= 0 || h <= 0) return false;
    rgba.resize(w * h * 4);
    glReadPixels(0, 0, w, h, GL_RGBA, GL_UNSIGNED_BYTE, rgba.data());
    return true;
}

void Renderer::setViewport(int x, int y, int width, int height) {
    glViewport(x, y, width, height);
}

} // namespace hse
