#pragma once

#ifndef ENGINE_H
#define ENGINE_H

#include <SDL3/SDL.h>

class GameController;
class AudioController;

class Engine
{
    public:
        Engine();
        ~Engine();

        /// @brief Initializes the window and renderer.
        /// @return bool True on success, false on failure.  
        bool initializeEngine();

        /// @brief Runs the games main loop.
        void mainLoop();

        AudioController* getAudioController();
    
    private:
        void endGame();

        /// @brief Handles SDL events.
        void handleEvents();

        /// @brief Handles a specific SDL event.
        void handleEvent(SDL_Event e);

        SDL_Window *window = NULL;
        SDL_Renderer *renderer = NULL;

        GameController *gameController = NULL;
        AudioController *audioController = NULL;
        
        bool gameRunning;

        float fps;
};

#endif