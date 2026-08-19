#include <cassert>
#include <iostream>
#include <memory>
#include "scene/scene.h"
#include "scene/primitive.h"
#include "math/bounding_box.h"

void test_aabb_intersection() {
    hse::BoundingBox b1({0, 0, 0}, {1, 1, 1});
    hse::BoundingBox b2({0.5, 0.5, 0.5}, {1.5, 1.5, 1.5});
    hse::BoundingBox b3({2, 2, 2}, {3, 3, 3});

    assert(b1.intersects(b2));
    assert(b2.intersects(b1));
    assert(!b1.intersects(b3));
    assert(!b3.intersects(b1));

    std::cout << "PASS: aabb intersection math" << std::endl;
}

void test_scene_collision() {
    auto scene = std::make_shared<hse::Scene>();

    auto p1 = std::make_shared<hse::Primitive>(hse::PrimitiveType::Cube);
    p1->setPosition({0, 0, 0});
    p1->setScale({1, 1, 1});
    scene->addPrimitive(p1);

    auto p2 = std::make_shared<hse::Primitive>(hse::PrimitiveType::Cube);
    p2->setPosition({0.8, 0, 0}); // Overlaps p1 (which goes -0.5 to 0.5, p2 goes 0.3 to 1.3)
    p2->setScale({1, 1, 1});
    scene->addPrimitive(p2);

    auto p3 = std::make_shared<hse::Primitive>(hse::PrimitiveType::Cube);
    p3->setPosition({2.0, 0, 0}); // No overlap
    p3->setScale({1, 1, 1});
    scene->addPrimitive(p3);

    assert(scene->checkCollision(p1->getID(), p2->getID()));
    assert(!scene->checkCollision(p1->getID(), p3->getID()));

    auto collisions = scene->getCollisions(p1->getID());
    assert(collisions.size() == 1);
    assert(collisions[0] == p2->getID());

    std::cout << "PASS: scene collision query" << std::endl;
}

int main() {
    test_aabb_intersection();
    test_scene_collision();
    std::cout << "All collision tests passed!" << std::endl;
    return 0;
}
