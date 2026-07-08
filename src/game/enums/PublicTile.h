#pragma once

// PublicTile won't tell what is under a hidden tile

namespace game::enums
{
    enum class PublicTile
    {
        Zero = 0,
        One = 1,
        Two = 2,
        Three = 3,
        Four = 4,
        Five = 5,
        Six = 6,
        Seven = 7,
        Eight = 8,
        Mine = 9,
        Hidden = 10,
        //Revealed = 11, // PublicTile can never be Revealed
        Flag = 12,
        QMark = 13,
        MisplacedFlag = 14,
        ExplodedMine = 15,
        Border = 16,
        OOB = 17
    };
}