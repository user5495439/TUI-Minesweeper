#pragma once

namespace game::enums
{
    enum class RevealResult
    {
        Neutral,    // no action
        Safe,       // tile was safe
        Won,        // all non mine tiles revealed
        Lost        // tile was a mine
    };
}