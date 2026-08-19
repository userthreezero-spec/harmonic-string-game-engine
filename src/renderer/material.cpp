#include "renderer/material.h"
#include <GL/glew.h>

namespace hse {

Material::Material(const std::string& name) : m_name(name) {}

void Material::apply(unsigned int shaderProgram) const {
    GLint albedoLoc = glGetUniformLocation(shaderProgram, "uMaterial.albedo");
    GLint roughnessLoc = glGetUniformLocation(shaderProgram, "uMaterial.roughness");
    GLint metallicLoc = glGetUniformLocation(shaderProgram, "uMaterial.metallic");
    GLint hasMapLoc = glGetUniformLocation(shaderProgram, "uMaterial.hasAlbedoMap");
    GLint mapLoc = glGetUniformLocation(shaderProgram, "uMaterial.albedoMap");

    glUniform3f(albedoLoc, m_albedo.x, m_albedo.y, m_albedo.z);
    glUniform1f(roughnessLoc, m_roughness);
    glUniform1f(metallicLoc, m_metallic);

    if (hasAlbedoMap()) {
        m_albedoMap->bind(0);
        glUniform1i(hasMapLoc, 1);
        glUniform1i(mapLoc, 0);
    } else {
        glUniform1i(hasMapLoc, 0);
    }
}

} // namespace hse
