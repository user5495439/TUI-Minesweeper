#pragma once

namespace game::logic
{
    enum class TileState
    {
        Hidden,
        Revealed,
        Flag,
        QMark,
        MisplacedFlag,
        ExplodedMine,
        Border,
        OOB
    };
}