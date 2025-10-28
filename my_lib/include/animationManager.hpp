#pragma once

#ifndef ANIMATION_MANAGER_H
#define ANIMATION_MANAGER_H

#include "animationInfo.hpp"
#include "json_fwd.hpp"
#include "assetManager.hpp"
#include <unordered_map>
#include <string>

using json = nlohmann::json;

class AnimationManager
{
    public:
        AnimationManager(AssetManager *assetManager);

        /// @brief Loads the sprite animations into the animations map.
        /// @param path The JSON file path, containing animation information.
        /// @return True on success. False otherwise.
        bool loadAnimations(std::string path);

        /// @brief Clears the animation map.
        void clearAnimations();

        /// @brief Searches for and returns an animation.
        /// @param name The animation name to search for.
        /// @return The animation info, or NULL if it doesn't exist.
        AnimationInfo* getAnimation(std::string name);

    private:
        /// @brief Loads an animation then calls addAnimation().
        /// @param animation The JSON object containing the animation information.
        /// @return True on success. False otherwise.
        bool loadAnimation(json animation);

        /// @brief Adds the given animation into the animation map.
        /// @param info The animation to add.
        /// @return True on success. False otherwise.
        bool addAnimation(AnimationInfo info);

        /// @brief Checks if the animation is valid.
        /// @param info The animation to check.
        /// @return True if the animation is valid. False otherwise.
        bool animationIsValid(AnimationInfo info) const;

        AssetManager *assetManager;

        std::unordered_map<std::string, AnimationInfo> animations;
};

#endif