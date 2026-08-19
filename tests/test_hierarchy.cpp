#include <cassert>
#include <iostream>
#include <memory>
#include "scene/scene.h"
#include "scene/primitive.h"

void test_root_has_no_parent() {
    auto prim = std::make_shared<hse::Primitive>(hse::PrimitiveType::Cube);
    assert(prim->getParent() == nullptr);
    assert(prim->getChildren().empty());
    std::cout << "PASS: root has no parent" << std::endl;
}

void test_parent_child_creation() {
    auto parent = std::make_shared<hse::Primitive>(hse::PrimitiveType::Cube);
    auto child = std::make_shared<hse::Primitive>(hse::PrimitiveType::Quad);
    child->setParent(parent);
    assert(child->getParent() == parent);
    assert(parent->getChildren().size() == 1);
    assert(parent->getChildren()[0] == child);
    std::cout << "PASS: parent child creation" << std::endl;
}

void test_world_matrix_root() {
    auto root = std::make_shared<hse::Primitive>(hse::PrimitiveType::Cube);
    root->setPosition({1.0f, 2.0f, 3.0f});
    root->computeWorldMatrix();
    const auto& wm = root->getWorldMatrix();
    assert(wm.data[12] == 1.0f);
    assert(wm.data[13] == 2.0f);
    assert(wm.data[14] == 3.0f);
    std::cout << "PASS: world matrix root" << std::endl;
}

void test_world_matrix_child() {
    auto parent = std::make_shared<hse::Primitive>(hse::PrimitiveType::Cube);
    parent->setPosition({10.0f, 0.0f, 0.0f});
    auto child = std::make_shared<hse::Primitive>(hse::PrimitiveType::Quad);
    child->setPosition({1.0f, 0.0f, 0.0f});
    child->setParent(parent);
    parent->computeWorldMatrix();
    const auto& wm = child->getWorldMatrix();
    assert(wm.data[12] == 11.0f);
    assert(wm.data[13] == 0.0f);
    assert(wm.data[14] == 0.0f);
    std::cout << "PASS: world matrix child" << std::endl;
}

void test_reparent() {
    auto parentA = std::make_shared<hse::Primitive>(hse::PrimitiveType::Cube);
    auto parentB = std::make_shared<hse::Primitive>(hse::PrimitiveType::Cube);
    auto child = std::make_shared<hse::Primitive>(hse::PrimitiveType::Quad);
    child->setParent(parentA);
    assert(parentA->getChildren().size() == 1);
    assert(parentB->getChildren().empty());
    child->setParent(parentB);
    assert(parentA->getChildren().empty());
    assert(parentB->getChildren().size() == 1);
    assert(child->getParent() == parentB);
    std::cout << "PASS: reparent" << std::endl;
}

void test_get_roots() {
    auto scene = std::make_shared<hse::Scene>("Test");
    auto root1 = std::make_shared<hse::Primitive>(hse::PrimitiveType::Cube);
    auto root2 = std::make_shared<hse::Primitive>(hse::PrimitiveType::Cube);
    auto child = std::make_shared<hse::Primitive>(hse::PrimitiveType::Quad);
    root1->addChild(child);
    scene->addPrimitive(root1);
    scene->addPrimitive(root2);
    scene->addPrimitive(child);
    auto roots = scene->getRoots();
    assert(roots.size() == 2);
    std::cout << "PASS: get roots" << std::endl;
}

void test_compute_all_world_matrices() {
    auto scene = std::make_shared<hse::Scene>("Test");
    auto root = std::make_shared<hse::Primitive>(hse::PrimitiveType::Cube);
    root->setPosition({5.0f, 0.0f, 0.0f});
    auto child = std::make_shared<hse::Primitive>(hse::PrimitiveType::Quad);
    child->setPosition({1.0f, 0.0f, 0.0f});
    child->setParent(root);
    scene->addPrimitive(root);
    scene->addPrimitive(child);
    scene->computeAllWorldMatrices();
    assert(child->getWorldMatrix().data[12] == 6.0f);
    std::cout << "PASS: compute all world matrices" << std::endl;
}

void test_scene_reparent() {
    auto scene = std::make_shared<hse::Scene>("Test");
    auto parentA = std::make_shared<hse::Primitive>(hse::PrimitiveType::Cube);
    auto parentB = std::make_shared<hse::Primitive>(hse::PrimitiveType::Cube);
    auto child = std::make_shared<hse::Primitive>(hse::PrimitiveType::Quad);
    scene->addPrimitive(parentA);
    scene->addPrimitive(parentB);
    scene->addPrimitive(child);
    bool ok = scene->reparent(child->getID(), parentB->getID());
    assert(ok);
    assert(child->getParent() == parentB);
    assert(parentA->getChildren().empty());
    assert(parentB->getChildren().size() == 1);
    std::cout << "PASS: scene reparent" << std::endl;
}

void test_multi_level_hierarchy() {
    auto grandparent = std::make_shared<hse::Primitive>(hse::PrimitiveType::Cube);
    grandparent->setPosition({100.0f, 0.0f, 0.0f});
    auto parent = std::make_shared<hse::Primitive>(hse::PrimitiveType::Cube);
    parent->setPosition({10.0f, 0.0f, 0.0f});
    auto child = std::make_shared<hse::Primitive>(hse::PrimitiveType::Quad);
    child->setPosition({1.0f, 0.0f, 0.0f});
    child->setParent(parent);
    parent->setParent(grandparent);
    grandparent->computeWorldMatrix();
    assert(child->getWorldMatrix().data[12] == 111.0f);
    assert(parent->getWorldMatrix().data[12] == 110.0f);
    assert(grandparent->getWorldMatrix().data[12] == 100.0f);
    std::cout << "PASS: multi level hierarchy" << std::endl;
}

int main() {
    test_root_has_no_parent();
    test_parent_child_creation();
    test_world_matrix_root();
    test_world_matrix_child();
    test_reparent();
    test_get_roots();
    test_compute_all_world_matrices();
    test_scene_reparent();
    test_multi_level_hierarchy();
    std::cout << "All hierarchy tests passed!" << std::endl;
    return 0;
}
