#include "timerCallback.hpp"

/// @brief Creates a callback timer.
/// @param callbackTimeMs Time in Ms to execute the callback at.
/// @param loop Whether to loop after hitting callback.
TimerCallback::TimerCallback(unsigned int callbackTimeMs, bool loop)
{
    this->callbackTimeMs = callbackTimeMs;
    this->loop = loop;

    callback = nullptr;
}

/// @brief Updates the callback timer, calling the callback function if the time is hit.
void TimerCallback::updateTimer()
{
    if (!isStarted() || isPaused())
    {
        return;
    }

    if (getTicks() >= callbackTimeMs)
    {
        //Execute callback
        if (callback != nullptr)
        {
            callback();
        }

        if (!loop)
        {
            //Stop, since we are not looping
            stop();
        }
        else
        {
            //Start the timer from the beginning
            start();
        }
    }
    
}

/// @brief Sets the time delay for a callback.
/// @param callbackTimeMs Time in Ms to execute the callback at.
void TimerCallback::setCallbackTime(unsigned int callbackTimeMs)
{
    this->callbackTimeMs = callbackTimeMs;
}

/// @brief Sets whether to loop.
/// @param loop Whether to loop after hitting callback.
void TimerCallback::setLoop(bool loop)
{
    this->loop = loop;
}

/// @brief Sets the callback function to call.
/// @param callback The callback function.
void TimerCallback::setCallbackFunction(std::function<void()> callback)
{
    this->callback = std::move(callback);
}

/// @brief Clears the callback function.
void TimerCallback::clearCallbackFunction()
{
    callback = nullptr;
}