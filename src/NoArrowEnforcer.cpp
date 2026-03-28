// NoArrowEnforcer.cpp
// Forces NoArrow modifier on when Boxing mode is active.
// The actual hook is in main.cpp (Hook_GetGameplayModifiers).
// This file provides helper logic used by the UI.

#include "NoArrowEnforcer.hpp"
#include "BoxingConfig.hpp"
#include "beatsaber-hook/shared/utils/logging.hpp"

namespace BoxingMod {

bool NoArrowEnforcer::IsNoArrowActive(GlobalNamespace::GameplayModifiers* modifiers) {
    if (!modifiers) return false;
    return modifiers->get_noArrows();
}

void NoArrowEnforcer::ForceNoArrow(GlobalNamespace::GameplayModifiers* modifiers) {
    if (!modifiers) return;
    // We override via the hook in main.cpp; this is a no-op helper.
    (void)modifiers;
}

bool NoArrowEnforcer::OnLevelStart(GlobalNamespace::GameplayModifiers* modifiers) {
    if (!getBoxingConfig().BoxingEnabled.GetValue()) return true;

    // The hook in main.cpp makes get_noArrows() always return true when
    // boxing is active, so this check will always pass in boxing mode.
    if (!IsNoArrowActive(modifiers)) {
        // Shouldn't reach here, but log just in case
        getLogger().warning("BoxingMod: NoArrow not active — blocking start");
        return false;
    }
    return true;
}

} // namespace BoxingMod
