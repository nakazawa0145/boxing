#pragma once

#include "config-utils/shared/config-utils.hpp"

DECLARE_CONFIG(BoxingConfig,
    CONFIG_VALUE(BoxingEnabled, bool, "BoxingEnabled", false);
    CONFIG_VALUE(MaxPunchScore, float, "MaxPunchScore", 115.0f);
    CONFIG_VALUE(PerfectPunchSpeed, float, "PerfectPunchSpeed", 8.0f); // m/s for 115pts
    CONFIG_VALUE(MinPunchSpeed, float, "MinPunchSpeed", 0.5f);         // below this = 0 pts
)
