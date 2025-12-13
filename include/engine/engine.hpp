#pragma once

#ifndef ENGINE_H
#define ENGINE_H

#include <SDL3/SDL.h>
#include <functional>
#include <queue>

class GameManager;
class AudioController;
class EventDispatcher;
class Random;
class AssetManager;

class Engine
{
    public:
        Engine() {}; 
        ~Engine() {};

        /// @brief Initializes the engine. Initializing SDL, the window and render, subsystems, and other required classes.
        /// @return bool True on success, false on failure.
        bool initializeEngine();

        /// @brief Closes the engine, cleaning up any initialized resources.
        void closeEngine();

        /// @brief Starts the game by initializing the GameManager and loading the starting room.
        void startGame();

        /// @brief Ends the game.
        void endGame();

        /// @brief Runs the games main loop.
        void mainLoop();

        #pragma region WrapperMethods

        /// @brief Adds a SDL listener callback.
        /// @param type The type of SDL_Event to listen for. 
        /// @param callback The method to call once the event is dispatched.
        void addSDLEventListener(Uint32 type, std::function<void(SDL_Event&)> callback);

        /// @brief Adds a listener for the given event type.
        /// @tparam T The event type we are listening for (Should derive from Event).
        /// @param callback The method to call once the event is dispatched.
        template<typename T>
        void addEventListener(std::function<void(T*)> callback);

        #pragma endregion WrapperMethods

        #pragma region Getters

        AudioController* getAudioController() const;
        EventDispatcher* getEventDispatcher() const;
	SDL_Window *getWindow() const;
        AssetManager* getAssetManager() const;
        Random* getRandomNumberGenerator() const;
        float getFPS() const;

        #pragma endregion Getters
    
    private:
        /// @brief Handles SDL events.
        void handleEvents();

        /// @brief Callback when the SDL_EVENT_QUIT event occurs.
        /// @param e Event info.
        void sdlQuitCallback(SDL_Event &e);

        bool gameRunning;
        float fps;

        SDL_Window *window = NULL;
        SDL_Renderer *renderer = NULL;

        GameManager *gameManager = NULL;
        AudioController *audioController = NULL;
        EventDispatcher *eventDispatcher = NULL;
        AssetManager *assetManager = NULL;
        Random *rng = NULL;
};

#endif
