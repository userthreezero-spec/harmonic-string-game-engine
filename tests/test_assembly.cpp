#include <cassert>
#include <iostream>
#include <memory>
#include <cmath>
#include "scene/scene.h"
#include "scene/primitive.h"
#include "math/bounding_box.h"

void test_hierarchical_transformation() {
    auto parent = std::make_shared<hse::Primitive>(hse::PrimitiveType::Group);
    parent->setName("Parent");
    parent->setPosition({10, 0, 0});

    auto child = std::make_shared<hse::Primitive>(hse::PrimitiveType::Cube);
    child->setName("Child");
    child->setPosition({1, 0, 0});
    child->setParent(parent);

    parent->computeWorldMatrix();

    // World position of child should be 11, 0, 0
    // Mat4 is row-major internally? Let's check indices.
    // data[12] is x-translation in standard row-major representation (4th column if M*v)
    const auto& wm = child->getWorldMatrix();
    assert(std::abs(wm.data[12] - 11.0f) < 0.001f);

    // Rotate parent 90 deg around Y
    parent->setRotation({0, 90, 0});
    parent->computeWorldMatrix();

    // Child local (1,0,0) rotated 90 deg around Y becomes (0,0,-1) if column-major convention...
    // Wait, let's re-verify rotation direction.
    // rotate(90, Y) on (1,0,0) -> (cos 90, 0, -sin 90) = (0, 0, -1)
    // World position = ParentPos (10,0,0) + RotatedChildLocal (0,0,-1) = (10, 0, -1)

    const auto& wm2 = child->getWorldMatrix();
    std::cout << "Child World X: " << wm2.data[12] << " Z: " << wm2.data[14] << std::endl;
    assert(std::abs(wm2.data[12] - 10.0f) < 0.001f);
    assert(std::abs(wm2.data[14] - (-1.0f)) < 0.001f);

    std::cout << "PASS: hierarchical transformation" << std::endl;
}

void test_aggregate_bounds() {
    auto parent = std::make_shared<hse::Primitive>(hse::PrimitiveType::Group);
    parent->setPosition({10, 0, 0});

    auto child1 = std::make_shared<hse::Primitive>(hse::PrimitiveType::Cube);
    child1->setPosition({-1, 0, 0}); // world 9
    child1->setScale({1, 1, 1});
    child1->setParent(parent);

    auto child2 = std::make_shared<hse::Primitive>(hse::PrimitiveType::Cube);
    child2->setPosition({1, 0, 0}); // world 11
    child2->setScale({1, 1, 1});
    child2->setParent(parent);

    parent->computeWorldMatrix();

    hse::BoundingBox abb = parent->getAggregateBoundingBox();
    // child1 bounds: [8.5, 9.5]
    // child2 bounds: [10.5, 11.5]
    // aggregate: [8.5, 11.5]

    assert(std::abs(abb.min.x - 8.5f) < 0.001f);
    assert(std::abs(abb.max.x - 11.5f) < 0.001f);

    std::cout << "PASS: aggregate bounds" << std::endl;
}

void test_explosion() {
    auto parent = std::make_shared<hse::Primitive>(hse::PrimitiveType::Group);
    parent->setPosition({0, 0, 0});

    auto child = std::make_shared<hse::Primitive>(hse::PrimitiveType::Cube);
    child->setPosition({1, 0, 0});
    child->setParent(parent);

    parent->setExplosionFactor(1.0f); // Double the distance
    parent->computeWorldMatrix();

    const auto& wm = child->getWorldMatrix();
    // pos = (1,0,0) * (1+1) = (2,0,0)
    assert(std::abs(wm.data[12] - 2.0f) < 0.001f);

    std::cout << "PASS: exploded view factor" << std::endl;
}

int main() {
    test_hierarchical_transformation();
    test_aggregate_bounds();
    test_explosion();
    std::cout << "All assembly tests passed!" << std::endl;
    return 0;
}
