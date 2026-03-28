#pragma once

#include "beatsaber-hook/shared/utils/hooking.hpp"
#include "GlobalNamespace/GameplayModifiers.hpp"
#include "GlobalNamespace/StandardLevelDetailView.hpp"
#include "GlobalNamespace/LevelCompletionResults.hpp"

namespace BoxingMod {

class NoArrowEnforcer {
public:
    // Patches the modifier panel to force-enable No Arrow when Boxing is active
    static void ForceNoArrow(GlobalNamespace::GameplayModifiers* modifiers);

    // Returns true if NoArrow is currently active
    static bool IsNoArrowActive(GlobalNamespace::GameplayModifiers* modifiers);

    // Hook: intercepts game start, blocks play if NoArrow is not enabled
    static bool OnLevelStart(GlobalNamespace::GameplayModifiers* modifiers);
};

} // namespace BoxingMod
