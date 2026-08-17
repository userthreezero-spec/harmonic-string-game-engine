#pragma once
#include <memory>

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
    void setViewport(int x, int y, int width, int height);

private:
    struct RendererState;
    std::unique_ptr<RendererState> m_state;
};

} // namespace hse
