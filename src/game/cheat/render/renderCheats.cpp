#include "renderCheats.h"

namespace game::cheat::render
{
//public:
    void CheatsRender::drawCheatsUI(core::XY offsets)
    {
        this->offsets = offsets;

        if (true)
            drawEdgeTiles();
    }

//private:
    void CheatsRender::drawEdgeTiles()
    {
        core::XY sizes = cheatsLogic->getSizes();

        core::XY boardStartPos = cheatsLogic->getBoardStartPos();

        for (core::XY xy = {0, 0}; xy.x < sizes.x; xy.x++) // width, x axis
            for (xy.y = 0; xy.y < sizes.y; xy.y++)               // height, y axis
            {
                logic::CheatsLogic::ChanceTile* chanceTile = cheatsLogic->getChanceTilePtr(xy);

                //if (chanceTile->edgeTile == false)
                    //continue;

                int8_t chance = chanceTile->chance;

                int chanceNumber = chanceTile->chance;
                chanceNumber = chanceNumber > 99 ? 99 : chanceNumber;

                std::string stringToWrite = std::to_string(chanceNumber);

                core::XY conXY = xy + boardStartPos;
                conXY.x *= 2;
                conXY += offsets;

                if (stringToWrite.size() == 1) stringToWrite += '0';

                Renderer::setCursorPos(conXY);
                Renderer::setColor(getChanceTextColor(chance), true);
                Renderer::WriteOOB(stringToWrite);
            }
    }

    RGB CheatsRender::getChanceTextColor(int8_t chance)
    {
        if (chance >= 100)
            return RGB::fromColors(colors::DarkRed);

        if (chance >= 75)
            return RGB::fromColors(colors::DarkYellow);

        if (chance >= 50)
            return RGB::fromColors(colors::Yellow);

        if (chance >= 25)
            return RGB::fromColors(colors::DarkCyan);

        if (chance >= 1)
            return RGB::fromColors(colors::DarkGreen);

        if (chance == 0)
            return RGB::fromColors(colors::DarkGray);

        if (chance <= -1)
            return RGB::fromColors(colors::DarkGray);
    }
}