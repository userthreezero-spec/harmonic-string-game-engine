#include <cassert>
#include <iostream>
#include <memory>
#include "renderer/material.h"
#include "scene/scene.h"
#include "scene/primitive.h"

void test_material_creation() {
    hse::Material mat("test_mat");
    assert(mat.getName() == "test_mat");
    std::cout << "PASS: material creation" << std::endl;
}

void test_material_albedo() {
    hse::Material mat("test");
    mat.setAlbedo({0.5f, 0.3f, 0.8f});
    assert(mat.getAlbedo().x == 0.5f);
    assert(mat.getAlbedo().y == 0.3f);
    assert(mat.getAlbedo().z == 0.8f);
    std::cout << "PASS: material albedo" << std::endl;
}

void test_material_roughness() {
    hse::Material mat("test");
    mat.setRoughness(0.75f);
    assert(mat.getRoughness() == 0.75f);
    std::cout << "PASS: material roughness" << std::endl;
}

void test_material_metallic() {
    hse::Material mat("test");
    mat.setMetallic(1.0f);
    assert(mat.getMetallic() == 1.0f);
    std::cout << "PASS: material metallic" << std::endl;
}

void test_material_albedo_map_null() {
    hse::Material mat("test");
    assert(!mat.hasAlbedoMap());
    assert(mat.getAlbedoMap() == nullptr);
    std::cout << "PASS: material albedo map null" << std::endl;
}

void test_material_defaults() {
    hse::Material mat("test");
    assert(mat.getAlbedo().x == 1.0f);
    assert(mat.getAlbedo().y == 1.0f);
    assert(mat.getAlbedo().z == 1.0f);
    assert(mat.getRoughness() == 0.5f);
    assert(mat.getMetallic() == 0.0f);
    std::cout << "PASS: material defaults" << std::endl;
}

void test_material_sharing() {
    auto mat = std::make_shared<hse::Material>("shared");
    auto prim1 = std::make_shared<hse::Primitive>(hse::PrimitiveType::Quad);
    auto prim2 = std::make_shared<hse::Primitive>(hse::PrimitiveType::Quad);
    prim1->setMaterial(mat);
    prim2->setMaterial(mat);
    assert(prim1->getMaterial() == prim2->getMaterial());
    assert(prim1->getMaterial()->getName() == "shared");
    std::cout << "PASS: material sharing" << std::endl;
}

void test_material_scene_lookup() {
    auto scene = std::make_shared<hse::Scene>("Test");
    auto mat = std::make_shared<hse::Material>("floor_mat");
    mat->setAlbedo({0.4f, 0.3f, 0.2f});
    scene->addMaterial(mat);
    auto found = scene->getMaterial("floor_mat");
    assert(found != nullptr);
    assert(found->getAlbedo().x == 0.4f);
    assert(scene->getMaterial("nonexistent") == nullptr);
    std::cout << "PASS: material scene lookup" << std::endl;
}

void test_material_fallback() {
    auto prim = std::make_shared<hse::Primitive>(hse::PrimitiveType::Triangle);
    assert(prim->getMaterial() == nullptr);
    std::cout << "PASS: material fallback (null)" << std::endl;
}

void test_material_properties_combined() {
    hse::Material mat("gold");
    mat.setAlbedo({1.0f, 0.8f, 0.0f});
    mat.setRoughness(0.1f);
    mat.setMetallic(1.0f);
    assert(mat.getName() == "gold");
    assert(mat.getAlbedo().x == 1.0f);
    assert(mat.getAlbedo().y == 0.8f);
    assert(mat.getAlbedo().z == 0.0f);
    assert(mat.getRoughness() == 0.1f);
    assert(mat.getMetallic() == 1.0f);
    assert(!mat.hasAlbedoMap());
    std::cout << "PASS: material properties combined" << std::endl;
}

int main() {
    test_material_creation();
    test_material_albedo();
    test_material_roughness();
    test_material_metallic();
    test_material_albedo_map_null();
    test_material_defaults();
    test_material_sharing();
    test_material_scene_lookup();
    test_material_fallback();
    test_material_properties_combined();
    std::cout << "All material tests passed!" << std::endl;
    return 0;
}
