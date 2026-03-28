// ============================================================
//  BoxingMod — Beat Saber Quest 1.37.0_9064817954
//  Turns sabers into boxing gloves; scores by punch speed.
//  No-Arrow modifier is required when Boxing mode is active.
// ============================================================

#include "beatsaber-hook/shared/utils/logging.hpp"
#include "beatsaber-hook/shared/utils/hooking.hpp"
#include "beatsaber-hook/shared/utils/il2cpp-utils.hpp"
#include "beatsaber-hook/shared/config/rapidjson-utils.hpp"

#include "custom-types/shared/register.hpp"
#include "questui/shared/QuestUI.hpp"

#include "GlobalNamespace/SaberModelController.hpp"
#include "GlobalNamespace/Saber.hpp"
#include "GlobalNamespace/SaberManager.hpp"
#include "GlobalNamespace/NoteController.hpp"
#include "GlobalNamespace/NoteCutInfo.hpp"
#include "GlobalNamespace/ScoreModel.hpp"
#include "GlobalNamespace/CutScoreBuffer.hpp"
#include "GlobalNamespace/GameplayModifiers.hpp"
#include "GlobalNamespace/StandardLevelScenesTransitionSetupDataSO.hpp"
#include "GlobalNamespace/PlayerSpecificSettings.hpp"
#include "GlobalNamespace/BeatmapObjectSpawnController.hpp"
#include "UnityEngine/GameObject.hpp"
#include "UnityEngine/Transform.hpp"
#include "UnityEngine/Vector3.hpp"
#include "UnityEngine/Time.hpp"
#include "UnityEngine/Rigidbody.hpp"

#include "BoxingConfig.hpp"
#include "GloveManager.hpp"
#include "BoxingScoreController.hpp"
#include "NoArrowEnforcer.hpp"
#include "UIManager.hpp"

static ModInfo modInfo{MOD_ID, VERSION};

Logger& getLogger() {
    static Logger* logger = new Logger(modInfo);
    return *logger;
}

// ─── Saber initialisation ────────────────────────────────────────────────────
// Called when each Saber is created in a level.
MAKE_HOOK_MATCH(Hook_SaberStart,
    &GlobalNamespace::Saber::Start,
    void, GlobalNamespace::Saber* self)
{
    Hook_SaberStart(self);

    if (!getBoxingConfig().BoxingEnabled.GetValue()) return;

    bool isLeft = (self->get_saberType().value == 0);
    BoxingMod::GloveManager::ApplyGlove(self, isLeft);
    getLogger().info("BoxingMod: Glove applied (%s)", isLeft ? "Left" : "Right");
}

// ─── Saber speed tracking ─────────────────────────────────────────────────────
// Hook SaberMovementData::AddNewData which is called every frame with tip pos.
#include "GlobalNamespace/SaberMovementData.hpp"
MAKE_HOOK_MATCH(Hook_AddNewData,
    &GlobalNamespace::SaberMovementData::AddNewData,
    void, GlobalNamespace::SaberMovementData* self,
    UnityEngine::Vector3 topPos,
    UnityEngine::Vector3 bottomPos,
    float time)
{
    Hook_AddNewData(self, topPos, bottomPos, time);

    // Estimate saber speed from tip displacement / delta-time
    auto lastTopPos = self->get_topPos();
    float dt = UnityEngine::Time::get_deltaTime();
    if (dt <= 0.0f) return;

    float speed = UnityEngine::Vector3::Distance(topPos, lastTopPos) / dt;

    // Route to the correct hand slot
    // SaberMovementData is owned by a Saber; we identify by checking parent
    // We store both and the note-cut hook picks the hitting hand.
    // Simple approach: store max of both into a shared "last hit speed" variable.
    BoxingMod::BoxingScoreController::rightSaberSpeed =
        std::max(BoxingMod::BoxingScoreController::rightSaberSpeed * 0.9f, speed);
    BoxingMod::BoxingScoreController::leftSaberSpeed  =
        std::max(BoxingMod::BoxingScoreController::leftSaberSpeed  * 0.9f, speed);
}

// ─── Score replacement ────────────────────────────────────────────────────────
// ScoreModel::GetSwingRatingScore is the vanilla per-swing score.
// We intercept CutScoreBuffer::HandleSaberSwingRatingCounterDidChange
// which finalises the per-note score.
#include "GlobalNamespace/CutScoreBuffer.hpp"
MAKE_HOOK_MATCH(Hook_CutScoreFinish,
    &GlobalNamespace::CutScoreBuffer::HandleSaberSwingRatingCounterDidChange,
    void, GlobalNamespace::CutScoreBuffer* self,
    GlobalNamespace::ISaberSwingRatingCounter* counter,
    float rating)
{
    if (!getBoxingConfig().BoxingEnabled.GetValue()) {
        Hook_CutScoreFinish(self, counter, rating);
        return;
    }

    // Determine which saber cut this note (use the higher speed reading)
    float speed = std::max(
        BoxingMod::BoxingScoreController::leftSaberSpeed,
        BoxingMod::BoxingScoreController::rightSaberSpeed);

    int punchScore = BoxingMod::BoxingScoreController::ScoreForSpeed(speed);

    // Overwrite the three sub-scores that compose the final note score:
    //   beforeCutRawScore (max 70) + accuracyRawScore (max 10) + afterCutRawScore (max 30) = 110
    // We spread our 0-115 score across these fields proportionally.
    int before   = (int)std::round(punchScore * 70.0f / 115.0f);
    int accuracy = (int)std::round(punchScore * 10.0f / 115.0f);
    int after    = punchScore - before - accuracy;

    // Use il2cpp field setters to override internal score values
    il2cpp_utils::SetFieldValue(self, "_beforeCutRawScore",   before);
    il2cpp_utils::SetFieldValue(self, "_accuracyRawScore",    accuracy);
    il2cpp_utils::SetFieldValue(self, "_afterCutRawScore",    after);

    // Let vanilla finalise and fire events with our patched values
    Hook_CutScoreFinish(self, counter, rating);

    // Reset speed decay after each hit
    BoxingMod::BoxingScoreController::leftSaberSpeed  = 0.0f;
    BoxingMod::BoxingScoreController::rightSaberSpeed = 0.0f;

    getLogger().debug("BoxingMod: punch speed=%.2f score=%d", speed, punchScore);
}

// ─── No-Arrow enforcement ─────────────────────────────────────────────────────
// We hook StandardLevelScenesTransitionSetupDataSO::Init which is called just
// before a level starts. If Boxing is enabled and NoArrow is NOT set, we log
// and override the modifier flag.
MAKE_HOOK_MATCH(Hook_LevelTransitionInit,
    &GlobalNamespace::StandardLevelScenesTransitionSetupDataSO::InitColorSchemeSaberTypes,
    void, GlobalNamespace::StandardLevelScenesTransitionSetupDataSO* self,
    /* remaining params omitted — use variadic trampoline via raw hook below */)
{
    Hook_LevelTransitionInit(self);
}

// Simpler: hook GameplayModifiers getter used by score multiplier
// Force noArrows=true when boxing is on.
MAKE_HOOK_MATCH(Hook_GetGameplayModifiers,
    &GlobalNamespace::GameplayModifiers::get_noArrows,
    bool, GlobalNamespace::GameplayModifiers* self)
{
    bool vanilla = Hook_GetGameplayModifiers(self);
    if (getBoxingConfig().BoxingEnabled.GetValue()) {
        return true; // Always report NoArrows active
    }
    return vanilla;
}

// ─── Mod entry point ──────────────────────────────────────────────────────────
extern "C" void setup(ModInfo& info) {
    info.id      = MOD_ID;
    info.version = VERSION;
    modInfo      = info;
    getLogger().info("BoxingMod setup called");
    getBoxingConfig().Init(info);
}

extern "C" void load() {
    il2cpp_functions::Init();

    getLogger().info("BoxingMod load called — installing hooks");

    // Register custom-types (UI view controller)
    custom_types::Register::AutoRegister();

    // Register QuestUI settings panel
    QuestUI::Init();
    QuestUI::Register::RegisterModSettingsViewController<BoxingMod::BoxingViewController*>(
        modInfo, "Boxing Mod");

    // Install hooks
    INSTALL_HOOK(getLogger(), Hook_SaberStart);
    INSTALL_HOOK(getLogger(), Hook_AddNewData);
    INSTALL_HOOK(getLogger(), Hook_CutScoreFinish);
    INSTALL_HOOK(getLogger(), Hook_GetGameplayModifiers);

    getLogger().info("BoxingMod: all hooks installed");
}
