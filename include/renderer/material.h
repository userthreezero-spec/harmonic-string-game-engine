#pragma once
#include "math/vec3.h"
#include "renderer/texture.h"
#include <string>
#include <memory>

namespace hse {

class Material {
public:
    Material(const std::string& name);

    const std::string& getName() const { return m_name; }

    void setAlbedo(const Vec3& color) { m_albedo = color; }
    const Vec3& getAlbedo() const { return m_albedo; }

    void setAlbedoMap(std::shared_ptr<Texture> texture) { m_albedoMap = texture; }
    std::shared_ptr<Texture> getAlbedoMap() const { return m_albedoMap; }
    bool hasAlbedoMap() const { return m_albedoMap != nullptr; }

    void setRoughness(float r) { m_roughness = r; }
    float getRoughness() const { return m_roughness; }

    void setMetallic(float m) { m_metallic = m; }
    float getMetallic() const { return m_metallic; }

    // Upload material properties as uniforms to the current shader
    void apply(unsigned int shaderProgram) const;

private:
    std::string m_name;
    Vec3 m_albedo{1.0f, 1.0f, 1.0f};
    std::shared_ptr<Texture> m_albedoMap = nullptr;
    float m_roughness = 0.5f;
    float m_metallic = 0.0f;
};

} // namespace hse
