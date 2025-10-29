#pragma once

#ifndef GAME_MANAGER_H
#define GAME_MANAGER_H

class Engine;

class GameManager
{
    public:

        GameManager(Engine *engine);
        ~GameManager();

        /// @brief Updates the game.
        /// @param deltaTime Time change between last and current frame.
        /// @return True on success, false on failure.
        bool update(double deltaTime);

        void startGame();

    private:
        Engine *engine;
};

#endif