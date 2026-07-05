#pragma once

namespace game::enums
{
    enum class RevealResult
    {
        Neutral,    // no action
        Safe,       // tile was safe
        Lost,       // tile was a mine
        Won         // all non mine tiles revealed
    };
}