// BoxingScoreController.cpp
// Score formula:
//   speed >= PerfectPunchSpeed  →  115  (perfect)
//   speed between Min and Perfect → linear interpolation 0→115
//   speed < MinPunchSpeed        →  0

#include "BoxingScoreController.hpp"
#include "BoxingConfig.hpp"
#include "beatsaber-hook/shared/utils/logging.hpp"
#include <cmath>
#include <algorithm>

namespace BoxingMod {

float BoxingScoreController::leftSaberSpeed  = 0.0f;
float BoxingScoreController::rightSaberSpeed = 0.0f;

int BoxingScoreController::ScoreForSpeed(float speed) {
    float maxScore     = getBoxingConfig().MaxPunchScore.GetValue();      // 115
    float perfectSpeed = getBoxingConfig().PerfectPunchSpeed.GetValue(); // 8.0 m/s
    float minSpeed     = getBoxingConfig().MinPunchSpeed.GetValue();     // 0.5 m/s

    if (speed >= perfectSpeed) {
        return static_cast<int>(maxScore);  // 115
    }
    if (speed <= minSpeed) {
        return 0;
    }

    // Linear interpolation in the range [minSpeed, perfectSpeed] → [0, 115]
    float t     = (speed - minSpeed) / (perfectSpeed - minSpeed);
    float score = t * maxScore;
    return static_cast<int>(std::round(score));
}

int BoxingScoreController::CalculatePunchScore(float saberSpeed) {
    return ScoreForSpeed(saberSpeed);
}

} // namespace BoxingMod
