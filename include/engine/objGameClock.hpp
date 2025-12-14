#ifndef OBJ_GAME_CLOCK_H
#define OBJ_GAME_CLOCK_H

#include "renderableObject.hpp"
#include "countdownTimer.hpp"
#include <vector>

class GameManager;
class BitmapFont;
class Texture;

struct TimeChange
{
	double offsetY;
	int time;
	int delay;
};

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

	/// @brief Sets the callback function to call once the clock reaches 0.
	/// @param callback The callback function.
	void setCallbackFunction(std::function<void()> callback);

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

	/// @brief Adds a time change to the timeChanges vector to display on the GUI.
	/// @param time The change in time for this time change.
	void addTimeChange(int time);

        #pragma endregion Timer

    private:
	const int TIME_OFFSET_X = 56;
	const int TIME_CHANGE_OFFSET_Y = 20;
	const float TIME_CHANGE_OFFSET_LERP = 0.2;
	const int TIME_CHANGE_MOVE_DELAY_MS = 1000;
	const double CLOCK_HAND_ANGLE_CHANGE = 360 / 60;	//Angle change per second (60 seconds = full rotation)
	const float CLOCK_HAND_LENGTH = 8;
	const int DELTA_CURRENT_TIME_CHANGE = 20 * 1000;
	const int CLOCK_LOW_TIME = 10;				//10 seconds before the clock should become red

        CountdownTimer timer;
	BitmapFont *font;
	Texture *clockTexture;
	double timeUntilSecond = 1.0;
	double currentTimeChange = 0.0;
	double clockHandAngle = -90;

	std::vector<TimeChange> timeChanges;
};

#endif
