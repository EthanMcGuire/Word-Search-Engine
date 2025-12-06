#ifndef OBJ_GAME_CLOCK_H
#define OBJ_GAME_CLOCK_H

#include "renderableObject.hpp"
#include "countdownTimer.hpp"

class GameManager;

class ObjGameClock : public RenderableObject
{
    public:
        ObjGameClock(GameManager *gameManager, double x, double y);

        /// @brief Updates the Object.
        /// @param deltaTime Time change in seconds since last frame.
        void update(double deltaTime) override;

        /// @brief Renders this game objects GUI.
        /// @param renderer The renderer to draw to.
        void renderGui(SDL_Renderer *renderer) override;

        /// @brief Called when the countdown timer finishes. Initiates the game over sequence.
        void timerCompleted();

        #pragma region Timer

        /// @brief Starts the clock, resetting it to the total time.
        void startTimer();

        /// @brief Pauses the clock.
        void pauseTimer();

        /// @brief Unpauses the clock.
        void unpauseTimer();

        /// @brief Sets the clock time.
        /// @param time The time in milliseconds.
        void setTime(int time);

        /// @brief Adds time to the clock.
        /// @param addTime Time to add in milliseconds.
        void addTime(int addTime);

        /// @brief Removes time from the clock.
        /// @param addTime Time to remove in milliseconds.
        void removeTime(int removeTime);

        #pragma endregion Timer

    private:
        CountdownTimer timer;
};

#endif