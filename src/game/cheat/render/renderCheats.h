#pragma once

#include "../logic/cheat.h"
#include "../../../core/render/renderer.h"

namespace game::cheat::render
{
    using namespace core::render;

    class CheatsRender
    {
    public:
        CheatsRender() {};
        CheatsRender(logic::CheatsLogic* CheatsLogic) : cheatsLogic(CheatsLogic) {};

        void drawCheatsUI(core::XY offsets);
    private:
        bool showEdgeTiles = false;
        core::XY offsets;
        logic::CheatsLogic* cheatsLogic;

        void drawEdgeTiles();
        RGB getChanceTextColor(int8_t chance);
    };
}