#pragma once
#include <vector>
#include <memory>
#include <string>

namespace hse {

class Primitive;
class Camera;

class Scene {
public:
    Scene(const std::string& name = "Untitled");
    ~Scene();

    const std::string& getName() const { return m_name; }
    size_t getPrimitiveCount() const { return m_primitives.size(); }
    size_t getCameraCount() const { return m_cameras.size(); }

    void addPrimitive(std::shared_ptr<Primitive> primitive);
    void removePrimitive(int index);
    void update(float deltaTime);

    const std::vector<std::shared_ptr<Primitive>>& getPrimitives() const {
        return m_primitives;
    }

    void addCamera(std::shared_ptr<Camera> camera);
    void setActiveCamera(int index);
    std::shared_ptr<Camera> getActiveCamera() const;

private:
    std::string m_name;
    std::vector<std::shared_ptr<Primitive>> m_primitives;
    std::vector<std::shared_ptr<Camera>> m_cameras;
    int m_activeCameraIndex = 0;
};

} // namespace hse
