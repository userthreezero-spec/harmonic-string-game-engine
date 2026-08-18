#pragma once
#include <memory>
#include <vector>
#include <cstdint>

namespace hse {

class Scene;
class Camera;

class Renderer {
public:
    Renderer();
    ~Renderer();

    bool initialize();
    void shutdown();

    void beginFrame();
    void renderScene(const Scene& scene, const Camera& camera);
    void endFrame();

    void setClearColor(float r, float g, float b, float a = 1.0f);
    void getClearColor(float& r, float& g, float& b) const;
    void setViewport(int x, int y, int width, int height);

    bool readPixels(std::vector<uint8_t>& rgba, int& width, int& height);
    uint64_t getFrameCount() const { return m_frameCount; }
    void incrementFrameCount() { m_frameCount++; }

private:
    struct RendererState;
    std::unique_ptr<RendererState> m_state;
    uint64_t m_frameCount = 0;
};

} // namespace hse
