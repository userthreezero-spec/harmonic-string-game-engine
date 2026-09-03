#include "renderer/material.h"
#include <GL/glew.h>

namespace hse {

Material::Material(const std::string& name) : m_name(name) {}

void Material::apply(const Locations& locs) const {
    if (locs.albedo != -1) glUniform3f(locs.albedo, m_albedo.x, m_albedo.y, m_albedo.z);
    if (locs.roughness != -1) glUniform1f(locs.roughness, m_roughness);
    if (locs.metallic != -1) glUniform1f(locs.metallic, m_metallic);

    if (hasAlbedoMap()) {
        m_albedoMap->bind(0);
        if (locs.hasAlbedoMap != -1) glUniform1i(locs.hasAlbedoMap, 1);
        if (locs.albedoMap != -1) glUniform1i(locs.albedoMap, 0);
    } else {
        if (locs.hasAlbedoMap != -1) glUniform1i(locs.hasAlbedoMap, 0);
    }
}

void Material::apply(unsigned int shaderProgram) const {
    Locations locs;
    locs.albedo = glGetUniformLocation(shaderProgram, "uMaterial.albedo");
    locs.roughness = glGetUniformLocation(shaderProgram, "uMaterial.roughness");
    locs.metallic = glGetUniformLocation(shaderProgram, "uMaterial.metallic");
    locs.hasAlbedoMap = glGetUniformLocation(shaderProgram, "uMaterial.hasAlbedoMap");
    locs.albedoMap = glGetUniformLocation(shaderProgram, "uMaterial.albedoMap");
    apply(locs);
}

} // namespace hse
