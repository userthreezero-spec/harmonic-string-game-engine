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
    void renderScene(const Scene& scene, const Camera& camera, uint64_t selectedID = 0);
    void endFrame();

    void setClearColor(float r, float g, float b, float a = 1.0f);
    void getClearColor(float& r, float& g, float& b) const;
    void setViewport(int x, int y, int width, int height);

    size_t getFrameCount() const;
    void incrementFrameCount();
    bool readPixels(std::vector<unsigned char>& rgba, int& w, int& h);

private:
    struct RendererState;
    std::unique_ptr<RendererState> m_state;
};

} // namespace hse
