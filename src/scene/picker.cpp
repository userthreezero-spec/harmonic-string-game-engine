#include "scene/picker.h"
#include "scene/primitive.h"
#include <algorithm>
#include <limits>

namespace hse {

Ray Picker::screenToRay(double x, double y, const Camera& camera, int width, int height) {
    if (width <= 0) width = 1280;
    if (height <= 0) height = 720;
    float nx = (2.0f * (float)x) / width - 1.0f;
    float ny = 1.0f - (2.0f * (float)y) / height;

    Mat4 invPV = (camera.getProjectionMatrix() * camera.getViewMatrix()).inverse();

    Vec3 nearPos = invPV * Vec3(nx, ny, -1.0f);
    Vec3 farPos = invPV * Vec3(nx, ny, 1.0f);

    return Ray(nearPos, (farPos - nearPos).normalized());
}

Intersection Picker::pick(const Ray& ray, const Scene& scene) {
    Intersection closest;
    for (auto& prim : scene.getPrimitives()) {
        Intersection hit = intersectPrimitive(ray, prim);
        if (hit.hit && hit.distance < closest.distance) {
            closest = hit;
        }
    }
    return closest;
}

Intersection Picker::intersectPrimitive(const Ray& ray, std::shared_ptr<Primitive> primitive) {
    Mat4 invModel = primitive->getWorldMatrix().inverse();

    Vec3 localOrigin = invModel * ray.origin;
    Vec3 localTarget = invModel * (ray.origin + ray.direction);
    Vec3 localDir = (localTarget - localOrigin).normalized();
    Ray localRay(localOrigin, localDir);

    Intersection hit;
    switch (primitive->getType()) {
        case PrimitiveType::Cube:     hit = intersectCube(localRay, primitive); break;
        case PrimitiveType::Quad:     hit = intersectQuad(localRay, primitive); break;
        case PrimitiveType::Triangle: hit = intersectTriangle(localRay, primitive); break;
        default: break;
    }

    if (hit.hit) {
        // Transform local hit point back to world space to get true distance
        Vec3 worldHitPoint = primitive->getWorldMatrix() * hit.point;
        hit.distance = (worldHitPoint - ray.origin).length();
        hit.point = worldHitPoint;
        hit.objectID = primitive->getID();
    }

    return hit;
}

Intersection Picker::intersectCube(const Ray& ray, std::shared_ptr<Primitive> primitive) {
    // AABB is [-0.5, 0.5] for all axes
    float tmin = -std::numeric_limits<float>::infinity();
    float tmax = std::numeric_limits<float>::infinity();

    const float min_bound = -0.5f;
    const float max_bound = 0.5f;

    float p[3] = {ray.origin.x, ray.origin.y, ray.origin.z};
    float d[3] = {ray.direction.x, ray.direction.y, ray.direction.z};

    for (int i = 0; i < 3; i++) {
        if (std::abs(d[i]) < 1e-6f) {
            if (p[i] < min_bound || p[i] > max_bound) return Intersection();
        } else {
            float t1 = (min_bound - p[i]) / d[i];
            float t2 = (max_bound - p[i]) / d[i];
            if (t1 > t2) std::swap(t1, t2);
            tmin = std::max(tmin, t1);
            tmax = std::min(tmax, t2);
            if (tmin > tmax) return Intersection();
        }
    }

    if (tmax < 0) return Intersection();

    Intersection hit;
    hit.hit = true;
    hit.distance = (tmin < 0) ? tmax : tmin;
    hit.point = ray.origin + ray.direction * hit.distance;
    return hit;
}

Intersection Picker::intersectQuad(const Ray& ray, std::shared_ptr<Primitive> primitive) {
    // Quad is XY plane, Z=0, bounds [-0.5, 0.5]
    if (std::abs(ray.direction.z) < 1e-6f) return Intersection();

    float t = -ray.origin.z / ray.direction.z;
    if (t < 0) return Intersection();

    Vec3 p = ray.origin + ray.direction * t;
    if (p.x >= -0.5f && p.x <= 0.5f && p.y >= -0.5f && p.y <= 0.5f) {
        Intersection hit;
        hit.hit = true;
        hit.distance = t;
        hit.point = p;
        return hit;
    }

    return Intersection();
}

Intersection Picker::intersectTriangle(const Ray& ray, std::shared_ptr<Primitive> primitive) {
    // Triangle vertices: (-0.5, -0.5, 0), (0.5, -0.5, 0), (0, 0.5, 0)
    if (std::abs(ray.direction.z) < 1e-6f) return Intersection();

    float t = -ray.origin.z / ray.direction.z;
    if (t < 0) return Intersection();

    Vec3 p = ray.origin + ray.direction * t;

    // Barycentric-lite for this specific triangle
    // V0=(-0.5, -0.5), V1=(0.5, -0.5), V2=(0, 0.5)
    // Simplified checks for these bounds
    if (p.y < -0.5f || p.y > 0.5f) return Intersection();

    // Slopes of sides:
    // Left: from (-0.5, -0.5) to (0, 0.5) -> y = 2x + 0.5 -> x = (y - 0.5)/2
    // Right: from (0.5, -0.5) to (0, 0.5) -> y = -2x + 0.5 -> x = -(y - 0.5)/2
    float x_min = (p.y - 0.5f) / 2.0f;
    float x_max = -(p.y - 0.5f) / 2.0f;

    if (p.x >= x_min && p.x <= x_max) {
        Intersection hit;
        hit.hit = true;
        hit.distance = t;
        hit.point = p;
        return hit;
    }

    return Intersection();
}

} // namespace hse
