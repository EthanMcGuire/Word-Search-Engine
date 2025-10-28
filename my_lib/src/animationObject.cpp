#include "animationObject.hpp"
#include "spriteAtlas.hpp"
#include "draw.hpp"
#include "camera.hpp"
#include "collisionDrawer.hpp"
#include <SDL_render.h>
#include <SDL_log.h>

AnimationObject::AnimationObject(std::string name, GameManager *gameManager, double x, double y) : RenderableObject(name, gameManager, x, y)
{
    animationInfo = NULL;

    imageSpeed = 1;

    alpha = 255;
    colorMod = gmtl::Vec3i(255, 255, 255);
}

/// @brief Updates the Object. By default only updates the collision location and the animation player.
/// @param deltaTime Time change in seconds since last frame.
void AnimationObject::update(double deltaTime)
{
    //Set the animations FPS based on imageSpeed
    if (animationInfo != NULL)
    {
        animationPlayer.setAnimationFPS(animationInfo->animationFPS * imageSpeed);
    }

    //This does nothing if the animation isn't playing
    animationPlayer.updateAnimation();
}

/// @brief Renders the current sprite using the Objects transform.
/// @param renderer The renderer to draw to.
/// @param camera The game camera.
void AnimationObject::render(SDL_Renderer *renderer, Camera *camera)
{
    if (!visible)
    {
        return;
    }
    
    if (animationInfo != NULL)
    {
        Draw::drawSpriteExt(renderer, camera, animationInfo->spriteAtlas, animationPlayer.getSpriteIndex(), pos, origin, scale, angle, alpha, colorMod);
    }
}

/// @brief Sets the current animation.
/// @param info The animation to set. Should contain a valid sprite atlas (not NULL).
void AnimationObject::setAnimation(AnimationInfo *info)
{
    //Clear animation if given NULL animation
    if (info == NULL)
    {
        clearAnimation();

        return;
    }

    //Sprite atlas doesn't exist?
    if (info->spriteAtlas == NULL)
    {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Object: Failed to set animation. The animation has a NULL sprite atlas.");

        return;
    }

    animationInfo = info;

    //Set up the animation player
    animationPlayer.setAnimationFPS(animationInfo->animationFPS * imageSpeed);
    animationPlayer.setSpriteRange(animationInfo->spriteStartIndex, animationInfo->spriteEndIndex);
    animationPlayer.setAnimationType(animationInfo->type);

    //Set the draw origin
    origin[0] = info->originX;
    origin[1] = info->originY;

    SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "Object: Loaded animation: \n%s", getAnimationInfo(animationInfo).c_str());
}

/// @brief Clears the current animation.
void AnimationObject::clearAnimation()
{
    animationInfo = NULL;
    animationPlayer.setAnimationFPS(0);
}

/// @brief Sets the animation frame.
/// @param frame The frame to set. Should be from 0 -> (frameCount - 1)
void AnimationObject::setFrame(int frame)
{
    animationPlayer.setFrame(frame);
}

/// @brief Starts animation playback.
void AnimationObject::playAnimation()
{
    animationPlayer.play();
}

/// @brief Stops animation playback.
void AnimationObject::pauseAnimation()
{
    animationPlayer.pause();
}

/// @return True if animation is playing (Even if animationFPS is 0). False otherwise. 
bool AnimationObject::animationIsPlaying() const
{
    return animationPlayer.isPlaying();
}