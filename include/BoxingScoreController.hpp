#pragma once

#include "beatsaber-hook/shared/utils/hooking.hpp"
#include "GlobalNamespace/NoteController.hpp"
#include "GlobalNamespace/NoteCutInfo.hpp"
#include "GlobalNamespace/ScoreController.hpp"
#include "GlobalNamespace/CutScoreBuffer.hpp"
#include "UnityEngine/Vector3.hpp"

namespace BoxingMod {

class BoxingScoreController {
public:
    // Called when a note is cut — replaces vanilla scoring
    static int CalculatePunchScore(float saberSpeed);

    // Velocity tracker per saber (updated every frame via hooks)
    static float leftSaberSpeed;
    static float rightSaberSpeed;

    // Hook targets
    static void OnNoteWasCut(
        GlobalNamespace::NoteController* noteController,
        GlobalNamespace::NoteCutInfo noteCutInfo);

    static int ScoreForSpeed(float speed);
};

} // namespace BoxingMod
