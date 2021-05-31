#include "Events.h"
#include "SDL.h"

namespace RxCore
{
    void Events::startup()
    {
        SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
    }

    void Events::shutdown()
    {
        SDL_Quit();
    }

    void Events::pollEvents(std::function<void(SDL_Event * ev)> f)
    {
        SDL_Event event;

        while (SDL_PollEvent(&event)) {

            f(&event);
#if 0
            switch (event.type) {
            case SDL_KEYDOWN:
                break;
            case SDL_KEYUP:
                break;
            case SDL_QUIT:
                break;
            case SDL_MOUSEMOTION:
                break;
            case SDL_MOUSEBUTTONDOWN:
                break;
            case SDL_MOUSEBUTTONUP:
                break;
            case SDL_MOUSEWHEEL:
                break;
            }
#endif
        }
    }

    SDL_Keymod Events::getKeyboardMods()
    {
        SDL_Keymod km =  SDL_GetModState();
        return km;
    }
}
