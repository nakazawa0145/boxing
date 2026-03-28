#pragma once

#include "questui/shared/QuestUI.hpp"
#include "questui/shared/BeatSaberUI.hpp"
#include "custom-types/shared/register.hpp"
#include "UnityEngine/MonoBehaviour.hpp"
#include "HMUI/ViewController.hpp"

DECLARE_CLASS_CODEGEN(BoxingMod, BoxingViewController, HMUI::ViewController,
    DECLARE_OVERRIDE_METHOD(void, DidActivate,
        il2cpp_utils::FindMethodUnsafe("HMUI", "ViewController", "DidActivate", 3),
        bool firstActivation, bool addedToHierarchy, bool screenSystemEnabling);
)

namespace BoxingMod {
    void RegisterMenuButton();
    void SetupModMenu();
}
