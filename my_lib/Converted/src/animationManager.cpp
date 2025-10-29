#include "animationManager.hpp"
#include "spriteAtlas.hpp"
#include "json/json.hpp"
#include "jsonUtils.hpp"
#include "stringUtils.hpp"
#include <SDL3/SDL_log.h>

using json = nlohmann::json;

AnimationManager::AnimationManager(AssetManager *assetManager)
{
    this->assetManager = assetManager;
}

/// @brief Loads the sprite animations into the animations map.
/// @param path The JSON file path, containing animation information.
/// @return True on success. False otherwise.
bool AnimationManager::loadAnimations(std::string path)
{
    bool success = true;
    json animations;
    json::iterator it;

    //Clear existing animation
    clearAnimations();

    SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "AnimationManager: Loading animations...");

    if (!std::filesystem::exists(path))
    {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "AnimationManager: File %s does not exist.", path.c_str());

        return false;
    }
    
    animations = parseJsonFileTop(path, "animations");

    //Failed to parse JSON?
    if (animations.is_null())
    {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "AnimationManager: Failed to read JSON. Field \"animations\" does not exist.");

        return false;
    }

    //Load each animation
    for (it = animations.begin(); it != animations.end(); it++)
    {
        json animation = (*it);
        
        if (!loadAnimation(animation))
        {
            success = false;

            break;
        }
    }

    if (!success)
    {
        SDL_LogInfo(SDL_LOG_CATEGORY_ERROR, "AnimationManager: Failed to load animations!");
    }

    return success;
}

/// @brief Clears the animation map.
void AnimationManager::clearAnimations()
{
    if (!animations.empty())
    {
        SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "AnimationManager: Clearing animations...");

        animations.clear();
    }
}

/// @brief Searches for and returns an animation.
/// @param name The animation name to search for.
/// @return The animation info, or NULL if it doesn't exist.
AnimationInfo* AnimationManager::getAnimation(std::string name)
{
    std::string searchName;

    searchName = stringToLower(name);

    //Search for animation with the given name
    if (animations.find(searchName) != animations.end())
    {
        return &animations[searchName];
    }
    else
    {
        return NULL;
    }
}

#pragma region PrivateMethods

/// @brief Loads an animation then calls addAnimation().
/// @param animation The JSON object containing the animation information.
/// @return True on success. False otherwise.
bool AnimationManager::loadAnimation(json animation)
{
    //Validation
    if (!animation["atlasName"].is_string())
    {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "AnimationManager: Failed to load animation. In JSON atlasName should exist and should be string.");

        return false;
    }

    if (!animation["animationInfo"].is_object())
    {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "AnimationManager: Failed to load animation. In JSON animationInfo should exist and should be string.");

        return false;
    }

    if (!animation["animationInfo"]["name"].is_string())
    {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "AnimationManager: Failed to load animation. In JSON animationInfo:name should exist and should be string.");

        return false;
    }

    if (!animation["animationInfo"]["spriteStartIndex"].is_number_unsigned())
    {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "AnimationManager: Failed to load animation. In JSON animationInfo:spriteStartIndex should exist and should be a non-negative integer.");

        return false;
    }

    if (!animation["animationInfo"]["spriteEndIndex"].is_number_unsigned())
    {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "AnimationManager: Failed to load animation. In JSON animationInfo:spriteEndIndex should exist and should be a non-negative integer.");

        return false;
    }

    if (!animation["animationInfo"]["originX"].is_number_unsigned())
    {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "AnimationManager: Failed to load animation. In JSON animationInfo:originX should exist and should be a non-negative integer.");

        return false;
    }

    if (!animation["animationInfo"]["originY"].is_number_unsigned())
    {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "AnimationManager: Failed to load animation. In JSON animationInfo:originY should exist and should be a non-negative integer.");

        return false;
    }

    if (!animation["animationInfo"]["animationFPS"].is_number_integer())
    {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "AnimationManager: Failed to load animation. In JSON animationInfo:animationFPS should exist and should be a integer.");

        return false;
    }

    if (!animation["animationInfo"]["type"].is_string())
    {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "AnimationManager: Failed to load animation. In JSON animationInfo:type should exist and should be string.");

        return false;
    }

    std::string atlasName, typeName;
    AnimationInfo info;
    SpriteAtlas *atlas;

    //Get the animation info
    animation["animationInfo"]["name"].get_to(info.name);
    animation["animationInfo"]["spriteStartIndex"].get_to(info.spriteStartIndex);
    animation["animationInfo"]["spriteEndIndex"].get_to(info.spriteEndIndex);
    animation["animationInfo"]["originX"].get_to(info.originX);
    animation["animationInfo"]["originY"].get_to(info.originY);
    animation["animationInfo"]["animationFPS"].get_to(info.animationFPS);
    animation["animationInfo"]["type"].get_to(typeName);

    //Get the sprite atlas
    animation["atlasName"].get_to(atlasName);

    atlas = assetManager->getAtlas(atlasName);

    if (atlas == NULL)
    {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "AnimationManager: Failed to load animation \"%s\". Atlas \"%s\" could not be found.", info.name.c_str(), atlasName.c_str());

        return false;
    }

    info.spriteAtlas = atlas;

    typeName = stringToLower(typeName);

    //Make sure the animation type exists
    if (ANIMATION_TYPE_NAME_MAP.find(typeName) == ANIMATION_TYPE_NAME_MAP.end())
    {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "AnimationManager: Failed to load animation \"%s\". Animation type %s does not exist!", info.name.c_str(), typeName.c_str());

        return false;
    }

    info.type = ANIMATION_TYPE_NAME_MAP.at(typeName);

    //Add the animation
    return addAnimation(info);
}

/// @brief Adds the given animation into the animation map.
/// @param info The animation to add.
/// @return True on success. False if the animation is invalid or already exists.
bool AnimationManager::addAnimation(AnimationInfo info)
{
    //Validate the animation
    if (!animationIsValid(info))
    {
        return false;
    }

    std::string name;
    
    name = stringToLower(info.name);

    //Animation already exists?
    if (animations.find(name) != animations.end())
    {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "AnimationManager: Failed to add animation. Animation \"%s\" already exists!", name.c_str());

        return false;
    }

    //Add the animation
    animations[name] = info;

    SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "AnimationManager: Added animation \"%s\".", name.c_str());

    return true;
}

/// @brief Checks if the animation is valid. It is invalid if the spriteAtlas is NULL, or the spriteRange goes out of bounds.
/// @param info The animation to check.
/// @return True if the animation is valid. False otherwise.
bool AnimationManager::animationIsValid(AnimationInfo info) const
{
    if (info.spriteAtlas == NULL)
    {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "AnimationManager: Failed to add animation. No SpriteAtlas was set for the animation!");

        return false;
    }
    else
    {
        unsigned int spriteCount;

        spriteCount = info.spriteAtlas->getSpriteCount();

        //Confirm that the sprite range is within the sprite sheets sprite count
        if (info.spriteStartIndex < 0 || info.spriteStartIndex >= spriteCount)
        {
            SDL_LogError(SDL_LOG_CATEGORY_ERROR, "AnimationManager: Failed to add animation. spriteStartIndex is outside of sprite range. spriteStartIndex: %d ValidRange: 0 -> %d", info.spriteStartIndex, spriteCount - 1);

            return false;
        }
        else if (info.spriteEndIndex < 0 || info.spriteEndIndex >= spriteCount)
        {
            SDL_LogError(SDL_LOG_CATEGORY_ERROR, "AnimationManager: Failed to add animation. spriteEndIndex is outside of sprite range. spriteEndIndex: %d ValidRange: 0 -> %d", info.spriteEndIndex, spriteCount - 1);

            return false;
        }
    }

    return true;
}

#pragma endregion PrivateMethods