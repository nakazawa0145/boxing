// UIManager.cpp
// In-game settings panel via QuestUI.
// Shows: [Toggle] Boxing Mode

#include "UIManager.hpp"
#include "BoxingConfig.hpp"
#include "GloveManager.hpp"
#include "beatsaber-hook/shared/utils/logging.hpp"

#include "questui/shared/BeatSaberUI.hpp"
#include "questui/shared/CustomTypes/Components/MainThreadScheduler.hpp"

#include "UnityEngine/RectTransform.hpp"
#include "UnityEngine/UI/Toggle.hpp"
#include "TMPro/TextMeshProUGUI.hpp"

DEFINE_TYPE(BoxingMod, BoxingViewController);

namespace BoxingMod {

void BoxingViewController::DidActivate(
    bool firstActivation,
    bool addedToHierarchy,
    bool screenSystemEnabling)
{
    if (!firstActivation) return;

    // ── Title ─────────────────────────────────────────────────────────────────
    auto title = QuestUI::BeatSaberUI::CreateText(
        get_transform(),
        "🥊 Boxing Mod",
        UnityEngine::Vector2(0.0f, 35.0f));
    title->set_fontSize(8.0f);
    title->set_alignment(TMPro::TextAlignmentOptions::Center);

    // ── Boxing Mode Toggle ────────────────────────────────────────────────────
    bool currentValue = getBoxingConfig().BoxingEnabled.GetValue();

    QuestUI::BeatSaberUI::CreateToggle(
        get_transform(),
        "Boxing Mode",
        currentValue,
        UnityEngine::Vector2(0.0f, 15.0f),
        [](bool value) {
            getBoxingConfig().BoxingEnabled.SetValue(value);
            getLogger().info("BoxingMod: Boxing mode %s", value ? "ENABLED" : "DISABLED");
        });

    // ── Info text ─────────────────────────────────────────────────────────────
    auto info = QuestUI::BeatSaberUI::CreateText(
        get_transform(),
        "When enabled:\n"
        "• Sabers become boxing gloves\n"
        "• Score = punch speed (max 115 pts)\n"
        "• No Arrow modifier is auto-applied\n\n"
        "Perfect punch: ~8 m/s = 115 pts\n"
        "Slower punches score proportionally less.",
        UnityEngine::Vector2(0.0f, -10.0f));
    info->set_fontSize(4.0f);
    info->set_alignment(TMPro::TextAlignmentOptions::Center);
}

} // namespace BoxingMod
