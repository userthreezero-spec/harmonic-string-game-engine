#pragma once
#include "math/ray.h"
#include "scene/scene.h"
#include "scene/camera.h"

namespace hse {

class Picker {
public:
    static Ray screenToRay(double x, double y, const Camera& camera, int width, int height);
    static Intersection pick(const Ray& ray, const Scene& scene);

private:
    static Intersection intersectPrimitive(const Ray& ray, std::shared_ptr<Primitive> primitive);
    static Intersection intersectCube(const Ray& ray, std::shared_ptr<Primitive> primitive);
    static Intersection intersectQuad(const Ray& ray, std::shared_ptr<Primitive> primitive);
    static Intersection intersectTriangle(const Ray& ray, std::shared_ptr<Primitive> primitive);
};

} // namespace hse
