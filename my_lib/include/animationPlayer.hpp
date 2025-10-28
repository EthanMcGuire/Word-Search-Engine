#pragma once

#ifndef ANIMATION_PLAYER_H
#define ANIMATION_PLAYER_H

#include "animationInfo.hpp"
#include <functional>

enum AnimationDirection
{
    NORMAL,
    REVERSED
};

/// @brief Used to cycle through the frames of an animation. Its only purpose is to cycle through a range of sprite indices.
/// Call getFrame() to get the current frame. This should be used with SpriteAtlas->render() to draw the correct sprite.
/// Call updateAnimation() every frame to update the animation. This will only work if the animation is playing, so call play() first.
class AnimationPlayer
{
    public:
        AnimationPlayer();

        /// @brief Starts playing the animation.
        void play();

        /// @brief Pauses the animation.
        void pause();

        /// @brief Updates the current animation. Only updates if isPlaying() = true.
        void updateAnimation();

        #pragma region Setters

        /// @brief Sets the animation type
        /// @param newType The AnimationType to set.
        void setAnimationType(AnimationType newType);

        //Use lambda to pass a function as a callback
        /*animationPlayer.setAnimationFinishedCallback([this]() {
            this->callbackTest();
        });
        */

        /// @brief Sets the callback function to call when an animation finishes. Only works for AnimationType::STOP_WHEN_DONE.
        /// @param callback The callback function to call.
        void setAnimationFinishedCallback(const std::function<void()> &callback);

        /// @brief Sets the sprite range for the animation. Determines what sprites to cycle through in the sprite atlas.
        /// @param startIndex Starting sprite index. Should be from 0 -> (spriteCount - 1)
        /// @param endIndex  Ending sprite index. Should be from 0 -> (spriteCount - 1)
        void setSpriteRange(unsigned int startIndex, unsigned int endIndex);

        /// @brief Sets the animation speed in frames per second.
        /// @param newAnimationFPS The new animation speed. Give a negative value for reversed playback.
        void setAnimationFPS(int newAnimationFPS);

        /// @brief Sets the current frame of the animation.
        /// @param newFrame The new frame to set. Should be from 0 -> (frameCount - 1). frameCount is determined by setSpriteRange().
        void setFrame(int newFrame);

        #pragma endregion Setters
        
        #pragma region Getters
        
        /// @return The current Animation FPS. 
        int getAnimationFPS() const;

        /// @return The current Animation frame. 
        int getFrame() const;

        /// @return The current sprite index to draw from the sprite atlas. 
        int getSpriteIndex() const;

        /// @return True if the animation is playing. False otherwise. 
        bool isPlaying() const;

        #pragma endregion Getters

    private:
        #pragma region AnimationState

        /// @return The current animation direction. 
        AnimationDirection getCurrentAnimationDirection() const;

        /// @brief Calculates the inverse direction of the given direction.
        /// @param dir The direction to calculate the inverse for.
        /// @return The opposite direction from dir.
        AnimationDirection getInverseDirection(AnimationDirection dir) const;

        /// @return True if the last frame of the animation was reached. 
        bool reachedEndOfAnimation() const;

        #pragma endregion AnimationState

        AnimationType type;
        AnimationDirection direction;

        //Callback for when animation finished. Only for STOP_WHEN_DONE animation type.
        std::function<void()> finishedCallback;

        bool playing;

        //Sprite range for a sprite sheet
        unsigned int spriteStartIndex;
        unsigned int spriteEndIndex;

        //Current frame info
        //Frame goes from 0 to frameCount - 1. Where frameCount = (spriteEndIndex - spriteStartIndex + 1)
        int frame;
        int frameCount;
        int frameDelay;                 //Delay to next frame
        int animationFPS;               //Animation frames played per second. Negative reverses the animation
};

#endif