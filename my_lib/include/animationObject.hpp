#pragma once

#ifndef ANIMATION_OBJECT_H
#define ANIMATION_OBJECT_H

#include "renderableObject.hpp"
#include "animationPlayer.hpp"

class AnimationObject : public RenderableObject
{
    public:
        AnimationObject(std::string name, GameManager *gameManager, double x, double y);
        virtual ~AnimationObject() = default;

        /// @brief Updates the Object. By default only updates the animation player.
        /// @param deltaTime Time change in seconds since last frame.
        void update(double deltaTime) override;

        /// @brief Renders the current sprite using the Objects transform.
        /// @param renderer The renderer to draw to.
        /// @param camera The game camera.
        virtual void render(SDL_Renderer *renderer, Camera *camera) override;

        #pragma region Animation
        
        /// @brief Sets the current animation.
        /// @param info The animation to set. Should contain a valid sprite atlas (not NULL).
        void setAnimation(AnimationInfo *info);

        /// @brief Clears the current animation.
        void clearAnimation();

        /// @brief Sets the animation frame.
        /// @param frame The frame to set. Should be from 0 -> (frameCount - 1)
        void setFrame(int frame);

        /// @brief Starts animation playback.
        void playAnimation();

        /// @brief Stops animation playback.
        void pauseAnimation();

        /// @return True if animation is playing (Even if animationFPS is 0). False otherwise. 
        bool animationIsPlaying() const;

        #pragma endregion Animation

        /*Public variables*/

        //Texture settings
        double imageSpeed;  //The animation speed. 1 is normal speed. Negative results in reversed playback.

        int alpha;
        gmtl::Vec3i colorMod;

    protected:
        //Animation
        AnimationInfo *animationInfo;
        AnimationPlayer animationPlayer;
};

#endif