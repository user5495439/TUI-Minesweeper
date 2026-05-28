// chatgpt came up with the name for this enum / file, i mean whatever i guess it's good, chatgpt also came up with the identifier names for this enum

namespace MineSweeper
{
    public enum TileState
    {
        Hidden = 0,
        Revealed = 1,
        Flag = 2,
        QMark = 3,
        MisplacedFlag = 4,
        ExplodedMine = 5
    }
}