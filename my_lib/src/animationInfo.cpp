#include "animationInfo.hpp"

/// @brief Gets the animation info as a string.
/// @param info The AnimationInfo to convert to string.
/// @return The animation information string.
std::string getAnimationInfo(AnimationInfo *info)
{
    std::string str;

    str = "Name: " + info->name + "\n";
    str += "\tSpriteStartIndex: " + std::to_string(info->spriteStartIndex) + "\n";
    str += "\tSpriteEndIndex: " + std::to_string(info->spriteEndIndex) + "\n";
    str += "\tOriginX: " + std::to_string(info->originX) + "\n";
    str += "\tOriginY: " + std::to_string(info->originY) + "\n";
    str += "\tFPS: " + std::to_string(info->animationFPS) + "\n";
    str += "\tType: " + ANIMATION_TYPE_NAME[info->type] + "\n";

    return str;
}