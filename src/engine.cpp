#include "engine.hpp"
#include "config.hpp"
#include "audioController.hpp"

Engine::Engine()
{
    audioController = new AudioController();
}

Engine::~Engine()
{
    delete audioController;
    audioController = NULL;
}

/// @brief Initializes the window and renderer.
/// @return bool True on success, false if creating the window and renderer fails.  
bool Engine::initializeEngine()
{
    SDL_Log("Initializing Engine...");

    if (!SDL_CreateWindowAndRenderer(Config::GAME_TITLE, Config::SCREEN_WIDTH, Config::SCREEN_HEIGHT, SDL_WINDOW_RESIZABLE, &window, &renderer)) 
    {
        SDL_Log("Couldn't create window/renderer: %s", SDL_GetError());

        return false;
    }
    
    SDL_SetRenderLogicalPresentation(renderer, Config::SCREEN_WIDTH, Config::SCREEN_HEIGHT, SDL_LOGICAL_PRESENTATION_LETTERBOX);

    if (!audioController->initialize())
    {
        return false;
    }

    return true;
}

/// @brief Runs the games main loop. Processes events by calling handleEvents().
void Engine::mainLoop()
{
    gameRunning = true;

    while (gameRunning)
    {
        handleEvents();

        audioController->update();

        const double now = ((double)SDL_GetTicks()) / 1000.0;  /* convert from milliseconds to seconds. */
        /* choose the color for the frame we will draw. The sine wave trick makes it fade between colors smoothly. */
        const float red = (float) (0.5 + 0.5 * SDL_sin(now));
        const float green = (float) (0.5 + 0.5 * SDL_sin(now + SDL_PI_D * 2 / 3));
        const float blue = (float) (0.5 + 0.5 * SDL_sin(now + SDL_PI_D * 4 / 3));
        SDL_SetRenderDrawColorFloat(renderer, red, green, blue, SDL_ALPHA_OPAQUE_FLOAT);  /* new color, full alpha. */

        /* clear the window to the draw color. */
        SDL_RenderClear(renderer);

        /* put the newly-cleared rendering on the screen. */
        SDL_RenderPresent(renderer);

        //TODO
        SDL_Delay(16);
    }
}

void Engine::endGame()
{
    gameRunning = false;
}

/// @brief Handles SDL events.
void Engine::handleEvents()
{
    SDL_Event e;

    while (SDL_PollEvent(&e) != 0)
    {
        handleEvent(e);
    }
}

/// @brief Handles a specific SDL event.
void Engine::handleEvent(SDL_Event e)
{
    if (e.type == SDL_EVENT_QUIT)
    {
        endGame();
    }
    else 
    {
        //TODO
        //Input events
    } 
}

AudioController* Engine::getAudioController()
{
    return audioController;
}