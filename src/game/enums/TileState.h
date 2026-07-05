#pragma once

namespace game::enums
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