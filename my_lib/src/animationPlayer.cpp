#include "animationPlayer.hpp"
#include "config.hpp"

#include <stdio.h>

AnimationPlayer::AnimationPlayer()
{
    type = AnimationType::LOOP;
    direction = AnimationDirection::NORMAL;

    finishedCallback = NULL;

    playing = false;

    spriteStartIndex = 0;
    spriteEndIndex = 0;

    frame = 0;
    frameCount = 0;
    frameDelay = 0;
    animationFPS = 10;
}

/// @brief Starts playing the animation. 
/// Sets the frameDelay based on animationFPS.
//  Loops the animation if the type is STOP_WHEN_DONE and the end of the animation was previously reached.
void AnimationPlayer::play()
{
    if (playing)
    {
        return;
    }

    playing = true;

    //Animation speed is 0?
    if (animationFPS == 0)
    {
        frameDelay = 0;

        return;
    }

    frameDelay = Config::FPS / abs(animationFPS);

    if (type == AnimationType::STOP_WHEN_DONE)
    {
        //Loop animation if at end
        if (reachedEndOfAnimation())
        {
            if (frame == 0)
            {
                frame = frameCount - 1;
            }
            else
            {
                frame = 0;
            }
        }
    }
}

/// @brief Pauses the animation.
void AnimationPlayer::pause()
{
    playing = false;
}

/// @brief Updates the current animation. Only updates if isPlaying() = true and animationFPS is not 0.
/// If the end of animation is reached:
///     PING_PONG -> Reverses the current animation direction
///     STOP_WHEN_DONE -> Stops the animation and calls finishedCallback (If it was set)
/// Changes the current frame based on animation direction. Direction is reversed if animationFPS < 0.
void AnimationPlayer::updateAnimation()
{
    if (!playing || animationFPS == 0)
    {
        return;
    }

    frameDelay--;

    //Ready to update frame?
    if (frameDelay <= 0)
    {
        //Account for end of animation
        if (reachedEndOfAnimation())
        {
            if (type == AnimationType::PING_PONG)
            {
                //Reverse animation direction
                direction = getInverseDirection(direction);
            }
            else if (type == AnimationType::STOP_WHEN_DONE)
            {
                playing = false;
                    
                //Call animation finished callback
                if (finishedCallback)
                {
                    finishedCallback();
                }

                //Return since we finished playing
                return;
            }   
        }

        //Don't update the animation if we only have 1 frame!
        if (frameCount > 1)
        {
            //Update based on animation direction
            AnimationDirection dir = getCurrentAnimationDirection();
            
            if (dir == AnimationDirection::NORMAL)
            {
                frame++;

                //Loop animation
                if (type == AnimationType::LOOP && frame > frameCount - 1)
                {
                    frame = 0;
                }
            }
            else
            {
                frame--;

                //Loop animation
                if (type == AnimationType::LOOP && frame < 0)
                {
                    frame = frameCount - 1;
                }
            }
        }

        frameDelay = Config::FPS / abs(animationFPS);
    }
}

#pragma region Setters

/// @brief Sets the animation type
/// @param newType The AnimationType to set.
void AnimationPlayer::setAnimationType(AnimationType newType)
{
    type = newType;

    //Reset animation direction if not PING_PONG.
    //PING_PONG is the only AnimationType that reverses the direction.
    if (type != AnimationType::PING_PONG)
    {
        direction = AnimationDirection::NORMAL;
    }
}

/// @brief Sets the callback function to call when an animation finishes. Only works for AnimationType::STOP_WHEN_DONE.
/// @param callback The callback function to call.
void AnimationPlayer::setAnimationFinishedCallback(const std::function<void()> &callback)
{
    finishedCallback = callback;
}

/// @brief Sets the sprite range for the animation. Determines what sprites to cycle through in the sprite atlas.
/// @param startIndex Starting sprite index. Should be from 0 -> (spriteCount - 1)
/// @param endIndex  Ending sprite index. Should be from 0 -> (spriteCount - 1)
void AnimationPlayer::setSpriteRange(unsigned int startIndex, unsigned int endIndex)
{
    spriteStartIndex = startIndex;
    spriteEndIndex = endIndex;

    frameCount = (spriteEndIndex - spriteStartIndex) + 1;

    //Clamp the frame within the valid range
    frame = SDL_clamp(frame, 0, frameCount - 1);
}

/// @brief Sets the animation speed in frames per second.
/// @param newAnimationFPS The new animation speed. Give a negative value for reversed playback. This is clamped to the game FPS, since the animation cannot play faster that that.
void AnimationPlayer::setAnimationFPS(int newAnimationFPS)
{
    //Clamp animation speed to FPS
    animationFPS = SDL_clamp(newAnimationFPS, -Config::FPS, Config::FPS);
}

/// @brief Sets the current frame of the animation.
/// @param newFrame The new frame to set. Clamps the value from 0 to (frameCount - 1).
void AnimationPlayer::setFrame(int newFrame)
{
    //Clamp the frame within the valid range
    frame = SDL_clamp(newFrame, 0, frameCount - 1);
}

#pragma endregion Setters

#pragma region Getters

/// @return The current Animation FPS. 
int AnimationPlayer::getAnimationFPS() const
{
    return animationFPS;
}

/// @return The current Animation frame. 
int AnimationPlayer::getFrame() const
{
    return frame;
}

/// @return The current sprite index to draw from the sprite atlas. 
int AnimationPlayer::getSpriteIndex() const
{
    return spriteStartIndex + frame;
}

/// @return True if the animation is playing. False otherwise. 
bool AnimationPlayer::isPlaying() const
{
    return playing;
}

#pragma endregion Getters

#pragma region AnimationState

/// @return The current animation direction. If animationFPS < 0, the inverse of direction is returned.
AnimationDirection AnimationPlayer::getCurrentAnimationDirection() const
{
    if (animationFPS < 0)
    {
        //Reverse the direction
        return getInverseDirection(direction);
    }
    else
    {
        return direction;
    }
}

/// @brief Calculates the inverse direction of the given direction.
/// @param dir The direction to calculate the inverse for.
/// @return The opposite direction from dir.
AnimationDirection AnimationPlayer::getInverseDirection(AnimationDirection dir) const
{
    if (dir == AnimationDirection::NORMAL)
    {
        return AnimationDirection::REVERSED;
    }
    else
    {
        return AnimationDirection::NORMAL;
    }
}

/// @return True if the last frame of the animation was reached. 
bool AnimationPlayer::reachedEndOfAnimation() const
{
    AnimationDirection dir = getCurrentAnimationDirection();

    if (dir == AnimationDirection::NORMAL)
    {
        return frame == frameCount - 1;
    }
    else
    {
        return frame == 0;
    }
}

#pragma endregion AnimationState