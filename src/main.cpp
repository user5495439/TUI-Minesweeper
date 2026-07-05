// ideas for this project: make it oo in certain places
//                         make an abstraction layer or idk for mouse input

#include "core/render/renderer.h"
#include "game/game.h"

using namespace core::render;
using namespace game;

int main()
{
    Renderer::bufferSetClearOnWrite(true);

    Renderer::bufferInitialize();

    struct sigaction sa{};
    sa.sa_handler = Game::resizeGame;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;

    sigaction(SIGWINCH, &sa, nullptr);

    Game::gameStart();

    return 0;
}