#include "renderer/renderer.h"
#include "renderer/material.h"
#include "scene/scene.h"
#include "scene/camera.h"
#include "scene/primitive.h"
#include "scene/light.h"
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
layout (location = 2) in vec3 aNormal;

uniform mat4 uModel;
uniform mat4 uView;
uniform mat4 uProjection;

out vec2 vUV;
out vec3 vNormal;
out vec3 vFragPos;

void main() {
    vFragPos = vec3(uModel * vec4(aPos, 1.0));
    vNormal = mat3(transpose(inverse(uModel))) * aNormal;
    vUV = aUV;
    gl_Position = uProjection * uView * vec4(vFragPos, 1.0);
}
)";

static const char* fragmentShaderSource = R"(
#version 330 core
out vec4 FragColor;

in vec2 vUV;
in vec3 vNormal;
in vec3 vFragPos;

struct Material {
    vec3 albedo;
    float roughness;
    float metallic;
    bool hasAlbedoMap;
    sampler2D albedoMap;
};

struct Light {
    vec3 position;
    vec3 color;
    float intensity;
};

uniform Material uMaterial;
uniform Light uLight;
uniform vec3 uViewPos;
uniform bool uSelected;

void main() {
    vec3 albedo = uMaterial.albedo;
    if (uMaterial.hasAlbedoMap) {
        albedo *= texture(uMaterial.albedoMap, vUV).rgb;
    }

    // Ambient
    float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * uLight.color;

    // Diffuse
    vec3 norm = normalize(vNormal);
    vec3 lightDir = normalize(uLight.position - vFragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * uLight.color * uLight.intensity;

    // Specular (Blinn-Phong)
    vec3 viewDir = normalize(uViewPos - vFragPos);
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(norm, halfwayDir), 0.0), 32.0);
    vec3 specular = (1.0 - uMaterial.roughness) * spec * uLight.color;

    vec3 result = (ambient + diffuse + specular) * albedo;
    if (uSelected) {
        result = mix(result, vec3(1.0, 1.0, 0.0), 0.5);
    }
    FragColor = vec4(result, 1.0);
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

void Renderer::renderScene(const Scene& scene, const Camera& camera, uint64_t selectedID) {
    glUseProgram(m_state->shaderProgram);

    GLint viewLoc = glGetUniformLocation(m_state->shaderProgram, "uView");
    GLint projLoc = glGetUniformLocation(m_state->shaderProgram, "uProjection");
    GLint modelLoc = glGetUniformLocation(m_state->shaderProgram, "uModel");
    GLint viewPosLoc = glGetUniformLocation(m_state->shaderProgram, "uViewPos");
    GLint selectedLoc = glGetUniformLocation(m_state->shaderProgram, "uSelected");

    Mat4 view = camera.getViewMatrix();
    Mat4 proj = camera.getProjectionMatrix();
    Vec3 viewPos = camera.getPosition();

    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, view.ptr());
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, proj.ptr());
    glUniform3f(viewPosLoc, viewPos.x, viewPos.y, viewPos.z);

    // Lighting
    auto& lights = scene.getLights();
    if (!lights.empty()) {
        auto light = lights[0]; // Support 1 light for now
        glUniform3f(glGetUniformLocation(m_state->shaderProgram, "uLight.position"),
                   light->getPosition().x, light->getPosition().y, light->getPosition().z);
        glUniform3f(glGetUniformLocation(m_state->shaderProgram, "uLight.color"),
                   light->getColor().x, light->getColor().y, light->getColor().z);
        glUniform1f(glGetUniformLocation(m_state->shaderProgram, "uLight.intensity"), light->getIntensity());
    } else {
        // Fallback light
        glUniform3f(glGetUniformLocation(m_state->shaderProgram, "uLight.position"), 0, 10, 0);
        glUniform3f(glGetUniformLocation(m_state->shaderProgram, "uLight.color"), 1, 1, 1);
        glUniform1f(glGetUniformLocation(m_state->shaderProgram, "uLight.intensity"), 0.5f);
    }

    // Create a fallback material if primitive has none
    static auto defaultMaterial = std::make_shared<Material>("Default");

    // Compute all world matrices before rendering
    const_cast<Scene&>(scene).computeAllWorldMatrices();

    for (const auto& prim : scene.getPrimitives()) {
        const Mat4& model = prim->getWorldMatrix();
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, model.ptr());
        glUniform1i(selectedLoc, (prim->getID() == selectedID) ? 1 : 0);

        auto material = prim->getMaterial();
        if (!material) material = defaultMaterial;

        material->apply(m_state->shaderProgram);

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
