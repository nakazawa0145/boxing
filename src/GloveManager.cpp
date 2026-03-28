// GloveManager.cpp
// Replaces the saber blade with a procedural boxing-glove mesh.
// The glove is built from Unity primitives (Cube + Sphere) parented to the saber.

#include "GloveManager.hpp"
#include "BoxingConfig.hpp"
#include "beatsaber-hook/shared/utils/logging.hpp"
#include "UnityEngine/GameObject.hpp"
#include "UnityEngine/Transform.hpp"
#include "UnityEngine/PrimitiveType.hpp"
#include "UnityEngine/MeshRenderer.hpp"
#include "UnityEngine/Material.hpp"
#include "UnityEngine/Color.hpp"
#include "UnityEngine/Vector3.hpp"
#include "UnityEngine/Quaternion.hpp"

using namespace UnityEngine;

namespace BoxingMod {

// ─── Helper: hide the vanilla saber blade ─────────────────────────────────────
void GloveManager::HideSaberBlade(GlobalNamespace::Saber* saber) {
    // The saber blade is on children named "SaberBlade" / "Trail"
    auto transform = saber->get_transform();
    int childCount = transform->get_childCount();
    for (int i = 0; i < childCount; i++) {
        auto child = transform->GetChild(i);
        auto go    = child->get_gameObject();
        auto name  = il2cpp_utils::RunMethod<StringW>(go, "get_name").value_or(StringW(""));
        std::string nameStr = static_cast<std::string>(name);

        if (nameStr.find("Blade") != std::string::npos ||
            nameStr.find("Trail") != std::string::npos ||
            nameStr.find("Glow")  != std::string::npos  ||
            nameStr.find("Inner") != std::string::npos)
        {
            go->SetActive(false);
        }
    }
}

void GloveManager::ShowSaberBlade(GlobalNamespace::Saber* saber) {
    auto transform = saber->get_transform();
    int childCount = transform->get_childCount();
    for (int i = 0; i < childCount; i++) {
        auto child = transform->GetChild(i);
        auto go    = child->get_gameObject();
        auto name  = il2cpp_utils::RunMethod<StringW>(go, "get_name").value_or(StringW(""));
        std::string nameStr = static_cast<std::string>(name);

        if (nameStr.find("Blade") != std::string::npos ||
            nameStr.find("Trail") != std::string::npos ||
            nameStr.find("Glow")  != std::string::npos  ||
            nameStr.find("Inner") != std::string::npos)
        {
            go->SetActive(true);
        }
    }
}

// ─── Build glove mesh from Unity primitives ───────────────────────────────────
//
//  Glove layout (local space, saber points +Z):
//
//      [knuckle spheres]   ← 4 small spheres across top
//       ___________
//      |           |  ← main fist cube  (0.12 x 0.16 x 0.18 m)
//      |___________|
//      |   wrist   |  ← wrist cylinder/cube
//      |___________|
//
GameObject* GloveManager::CreateGloveMesh(bool isLeft, Color color) {
    // Root empty
    auto root = GameObject::New_ctor(StringW(isLeft ? "BoxingGlove_L" : "BoxingGlove_R"));

    // ── Main fist ──────────────────────────────────────────────────────────────
    auto fist = GameObject::CreatePrimitive(PrimitiveType::Cube);
    fist->set_name(StringW("Fist"));
    fist->get_transform()->SetParent(root->get_transform(), false);
    fist->get_transform()->set_localPosition(Vector3(0.0f, 0.0f, 0.08f));
    fist->get_transform()->set_localScale(Vector3(0.12f, 0.16f, 0.18f));

    // ── Knuckle bumps (4 spheres) ──────────────────────────────────────────────
    float kx[] = {-0.045f, -0.015f, 0.015f, 0.045f};
    for (int i = 0; i < 4; i++) {
        auto knuckle = GameObject::CreatePrimitive(PrimitiveType::Sphere);
        knuckle->set_name(StringW("Knuckle"));
        knuckle->get_transform()->SetParent(root->get_transform(), false);
        knuckle->get_transform()->set_localPosition(Vector3(kx[i], 0.082f, 0.09f));
        knuckle->get_transform()->set_localScale(Vector3(0.04f, 0.04f, 0.04f));
    }

    // ── Thumb ──────────────────────────────────────────────────────────────────
    auto thumb = GameObject::CreatePrimitive(PrimitiveType::Sphere);
    thumb->set_name(StringW("Thumb"));
    thumb->get_transform()->SetParent(root->get_transform(), false);
    float thumbX = isLeft ? -0.07f : 0.07f;
    thumb->get_transform()->set_localPosition(Vector3(thumbX, 0.06f, 0.07f));
    thumb->get_transform()->set_localScale(Vector3(0.05f, 0.07f, 0.05f));

    // ── Wrist ──────────────────────────────────────────────────────────────────
    auto wrist = GameObject::CreatePrimitive(PrimitiveType::Cube);
    wrist->set_name(StringW("Wrist"));
    wrist->get_transform()->SetParent(root->get_transform(), false);
    wrist->get_transform()->set_localPosition(Vector3(0.0f, 0.0f, -0.04f));
    wrist->get_transform()->set_localScale(Vector3(0.10f, 0.14f, 0.10f));

    // ── Apply colour to all MeshRenderers ──────────────────────────────────────
    auto renderers = root->GetComponentsInChildren<MeshRenderer*>();
    for (int i = 0; i < renderers->get_Length(); i++) {
        auto mat = renderers->get_Item(i)->get_material();
        mat->set_color(color);
    }

    return root;
}

// ─── Public API ───────────────────────────────────────────────────────────────
void GloveManager::ApplyGlove(GlobalNamespace::Saber* saber, bool isLeft) {
    HideSaberBlade(saber);

    // Pick colour from saber colour scheme (red=right, blue=left)
    Color gloveColor = isLeft ? Color(0.1f, 0.3f, 1.0f, 1.0f)   // blue
                               : Color(1.0f, 0.15f, 0.15f, 1.0f); // red

    auto glove = CreateGloveMesh(isLeft, gloveColor);
    glove->get_transform()->SetParent(saber->get_transform(), false);

    // Orient: saber grip is at origin, tip points +Z
    glove->get_transform()->set_localPosition(Vector3(0.0f, 0.0f, 0.0f));
    glove->get_transform()->set_localRotation(Quaternion::get_identity());
}

void GloveManager::RemoveGlove(GlobalNamespace::Saber* saber) {
    auto transform = saber->get_transform();
    int childCount = transform->get_childCount();
    for (int i = childCount - 1; i >= 0; i--) {
        auto child = transform->GetChild(i);
        auto go    = child->get_gameObject();
        auto name  = il2cpp_utils::RunMethod<StringW>(go, "get_name").value_or(StringW(""));
        std::string nameStr = static_cast<std::string>(name);
        if (nameStr.find("BoxingGlove") != std::string::npos) {
            GameObject::Destroy(go);
        }
    }
    ShowSaberBlade(saber);
}

} // namespace BoxingMod
