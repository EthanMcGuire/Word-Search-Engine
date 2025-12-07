#include "engine.hpp"
#include "config.hpp"
#include "audioController.hpp"
#include "eventDispatcher.hpp"
#include "assetManager.hpp"
#include "gameManager.hpp"
#include "random.hpp"
#include "timer.hpp"
#include "SDL3_ttf/SDL_ttf.h"

/// @brief Initializes the window and renderer.
/// @return bool True on success, false if creating the window and renderer fails.  
bool Engine::initializeEngine()
{
    SDL_Log("Engine: Initializing Engine...");

    if (!SDL_Init(Config::SDL_FLAGS)) 
    {
        SDL_Log("Engine: Failed to initialize SDL: %s", SDL_GetError());

        return false;
    }

    if (!SDL_CreateWindowAndRenderer(Config::GAME_TITLE, Config::SCREEN_WIDTH, Config::SCREEN_HEIGHT, SDL_WINDOW_RESIZABLE, &window, &renderer)) 
    {
        SDL_Log("Engine: Failed to create window/renderer: %s", SDL_GetError());

        return false;
    }
    
    SDL_SetRenderLogicalPresentation(renderer, Config::SCREEN_WIDTH, Config::SCREEN_HEIGHT, SDL_LOGICAL_PRESENTATION_LETTERBOX);

    if (!TTF_Init())
    {
        SDL_Log("Engine: Failed to initialize TTF: %s", SDL_GetError());

        return false;
    }

    rng = new Random();

    //Audio
    audioController = new AudioController();

    if (!audioController->initialize())
    {
        return false;
    }

    //Assets
    assetManager = new AssetManager(renderer, audioController, rng);

    if (!assetManager->loadAssets(Config::ASSET_PATH))
    {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Engine: Failed to load assets!");

        return false;
    }

    assetManager->displayAssetInfo();

    //Set up events
    eventDispatcher = new EventDispatcher();

    eventDispatcher->addSDLListener(SDL_EVENT_QUIT, [this](SDL_Event &e) {
        this->sdlQuitCallback(e);
    });

    //Game manager
    gameManager = new GameManager(this);

    SDL_Log("Engine: Engine successfully initialized.");

    return true;
}

void Engine::closeEngine()
{
    if (gameManager != NULL)
    {
	   delete gameManager;
    } 

    //Remove event listeners
    if (eventDispatcher != NULL)
    {
        delete eventDispatcher;
    }

    //Free assets
    if (assetManager != NULL)
    {
        assetManager->clearAssets();
        assetManager->displayAssetInfo();

        delete assetManager;
    }
    
    if (audioController != NULL)
    {
        delete audioController;
    }

    if (rng != NULL)
    {
	delete rng;
    }

    if (renderer != NULL)
    {
        SDL_DestroyRenderer(renderer);
    }

    if (window != NULL)
    {
        SDL_DestroyWindow(window);
    }

    SDL_Quit();
    TTF_Quit();
}

/// @brief Starts the game by initializing the GameManager and loading the starting room.
void Engine::startGame()
{
    SDL_Log("Engine: Initializing the game manager and starting the game.");

    gameManager->initializeGameManager();
    gameManager->setRoomToLoad(Config::STARTING_ROOM);

    gameRunning = true;
}

/// @brief Ends the game.
void Engine::endGame()
{
    gameRunning = false;
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

    fpsTimer.start();

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

        if (!audioController->update())
        {
            SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Engine: AudioController update failed. Ending game.");

            endGame();
        }

        if (!gameManager->update(deltaTime))
        {
            SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Engine: GameManager update failed. Ending game.");

            endGame();
        }

        #pragma endregion Update

        #pragma region Draw

        //Clear the screen
        SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
        SDL_RenderClear(renderer);

        gameManager->draw(renderer);
        gameManager->drawGui(renderer);

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

/// @brief Handles SDL events.
void Engine::handleEvents()
{
    SDL_Event e;

    while (SDL_PollEvent(&e) != 0)
    {
        eventDispatcher->dispatchEvent(e);
    }
}

/// @brief Callback when the SDL_EVENT_QUIT event occurs.
/// @param e Event info.
void Engine::sdlQuitCallback(SDL_Event &e)
{
    endGame();
}

#pragma region Getters

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

float Engine::getFPS() const
{
    return fps;
}

#pragma endregion Getters
