#pragma once

#include "beatsaber-hook/shared/utils/hooking.hpp"
#include "GlobalNamespace/SaberModelController.hpp"
#include "GlobalNamespace/Saber.hpp"
#include "UnityEngine/GameObject.hpp"
#include "UnityEngine/Transform.hpp"
#include "UnityEngine/MeshRenderer.hpp"
#include "UnityEngine/PrimitiveType.hpp"
#include "UnityEngine/Color.hpp"
#include "UnityEngine/Vector3.hpp"

namespace BoxingMod {

class GloveManager {
public:
    static void ApplyGlove(GlobalNamespace::Saber* saber, bool isLeft);
    static void RemoveGlove(GlobalNamespace::Saber* saber);
    static void UpdateAllSabers();

private:
    static UnityEngine::GameObject* CreateGloveMesh(bool isLeft, UnityEngine::Color color);
    static void HideSaberBlade(GlobalNamespace::Saber* saber);
    static void ShowSaberBlade(GlobalNamespace::Saber* saber);
};

} // namespace BoxingMod
