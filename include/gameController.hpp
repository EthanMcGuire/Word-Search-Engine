#pragma once

#ifndef GAME_CONTROLLER_H
#define GAME_CONTROLLER_H

class Engine;

class GameController
{
    public:

        GameController(Engine *engine);
        ~GameController();

        /// @brief Updates the game.
        /// @param deltaTime Time change between last and current frame.
        /// @return True on success, false on failure.
        bool update(double deltaTime);

        void startGame();

    private:
        Engine *engine;
};

#endif