#include "engine.hpp"
#include "config.hpp"
#include "audioController.hpp"
#include "eventDispatcher.hpp"
#include "assetManager.hpp"
#include "gameManager.hpp"
#include "random.hpp"
#include "timer.hpp"

Engine::Engine()
{
    assetManager = NULL;

    gameManager = new GameManager(this);
    audioController = new AudioController();
    eventDispatcher = new EventDispatcher();
    rng = new Random();
}

Engine::~Engine()
{
    delete gameManager;
    delete audioController;
    delete eventDispatcher;
    delete rng;
    if (assetManager != NULL) delete assetManager;
    
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);

    gameManager = NULL;
    audioController = NULL;
    eventDispatcher = NULL;
    rng = NULL;
    assetManager = NULL;
    renderer = NULL;
    window = NULL;
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

    assetManager = new AssetManager(renderer, audioController, rng);

    return true;
}

/// @brief Runs the games main loop. Processes events by calling handleEvents().
void Engine::mainLoop()
{
    Timer fpsTimer;
    Timer fpsCapTimer;
    float avgFPS;
    int countedFrames = 0;

    Uint64 currentTime = SDL_GetPerformanceCounter();
    Uint64 previousTime = 0;
    double deltaTime = 0;

    gameRunning = true;

    fpsTimer.start();

    //REMOVE ME
    gameManager->startGame();

    while (gameRunning)
    {
        fpsCapTimer.start();

        //Calculate delta time in seconds
        previousTime = currentTime;
        currentTime = SDL_GetPerformanceCounter();

        deltaTime = (double) ( (currentTime - previousTime) * 1000 / (double) SDL_GetPerformanceFrequency());
        deltaTime *= 0.001;

        //Cap delta time to the frame rate if it gets too big
        deltaTime = std::min(deltaTime, 1.0 / (double) Config::FPS);

        //Calculate FPS
        avgFPS = countedFrames / (fpsTimer.getTicks() / 1000.f);

        if (avgFPS > 2000000)
        {
            avgFPS = 0;
        }

        fps = avgFPS;

        handleEvents();

        #pragma region Update

        audioController->update();
        gameManager->update(deltaTime);

        #pragma endregion Update

        #pragma region Draw

        /* clear the window to the draw color. */
        SDL_RenderClear(renderer);

        //gameManager->drawGui();
        //gameManager->draw();

        /* put the newly-cleared rendering on the screen. */
        SDL_RenderPresent(renderer);

        #pragma endregion Draw

        //Cap frame rate
        int frameTicks = fpsCapTimer.getTicks();

        if (frameTicks < Config::TICKS_PER_FRAME)
        {
            SDL_Delay(Config::TICKS_PER_FRAME - frameTicks);
        }

        countedFrames++;
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

AudioController* Engine::getAudioController() const
{
    return audioController;
}

EventDispatcher* Engine::getEventDispatcher() const
{
    return eventDispatcher;
}

AssetManager* Engine::getAssetManager() const
{
    return assetManager;
}

Random* Engine::getRandomNumberGenerator() const
{
    return rng;
}