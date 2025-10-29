#pragma once

#ifndef ANIMATION_INFO_H
#define ANIMATION_INFO_H

#include <string>
#include <unordered_map>

class SpriteAtlas;

enum AnimationType
{
    LOOP,                   //Loop the animation
    PING_PONG,              //Change animation direction when the end is reached
    STOP_WHEN_DONE,         //Stops the animation when complete
    ANIMATION_TYPE_COUNT
};

static const std::string ANIMATION_TYPE_NAME[AnimationType::ANIMATION_TYPE_COUNT] = {"Loop", "Ping Pong", "Stop When Done"};
static const std::unordered_map<std::string, AnimationType> ANIMATION_TYPE_NAME_MAP = { {"loop", AnimationType::LOOP}, {"ping_pong", AnimationType::PING_PONG}, {"stop_when_done", AnimationType::STOP_WHEN_DONE} };

/// @brief Stores information on an animation.
struct AnimationInfo
{
    std::string name;
    SpriteAtlas *spriteAtlas;           //Sprite sheet to pull animation from
    unsigned int spriteStartIndex;      //Starting sprite in the sprite sheet
    unsigned int spriteEndIndex;        //End sprite in the sprite sheet
    unsigned int originX;               //The sprite x origin.
    unsigned int originY;               //The sprite y origin.
    int animationFPS;                   //Base frames per second for the animation.
    AnimationType type;                 //The type of animation

    AnimationInfo(std::string animationName = "Default_Name", SpriteAtlas *atlas = nullptr, unsigned int spriteStart = 0, unsigned int spriteEnd = 0, 
                    unsigned int spriteOriginX = 0, unsigned int spriteOriginY = 0, int FPS = 10, AnimationType animationType = AnimationType::LOOP) : 
                    name(animationName), spriteAtlas(atlas), spriteStartIndex(spriteStart), spriteEndIndex(spriteEnd), originX(spriteOriginX), originY(spriteOriginY), animationFPS(FPS), type(animationType) {};
};

/// @brief Gets the animation info as a string.
/// @param info The AnimationInfo to convert to string.
/// @return The animation information string.
std::string getAnimationInfo(AnimationInfo *info);

#endif