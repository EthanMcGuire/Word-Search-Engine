#include "assetManager.hpp"
#include "random.hpp"
#include "audioController.hpp"
#include "texture.hpp"
#include "spriteAtlas.hpp"
#include "json/json.hpp"
#include "jsonUtils.hpp"
#include "stringUtils.hpp"
#include <fstream>
#include <SDL3/SDL_log.h>

using json = nlohmann::json;

/// @brief Creates the asset manager.
/// @param renderer The window renderer. Used to load textures.
/// @param audioController The audio controller. Used to load sound assets.
/// @param random The RNG class. Passed to bitmap fonts for random character shake.
AssetManager::AssetManager(SDL_Renderer *renderer, AudioController *audioController, Random *random)
{
    this->renderer = renderer;
    this->audioController = audioController;
    this->random = random;
}

#pragma region Getters

/// @brief Gets a texture asset by name.
/// @param name The name of the asset to pull.
/// @return A pointer to the asset, or NULL if the asset was not found.
Texture* AssetManager::getTexture(std::string name) const
{
    name = stringToLower(name);

    if (textures.find(name) != textures.end())
    {
        return textures.at(name);
    }
    else
    {
        return NULL;
    }
}

/// @brief Gets a sprite atlas asset by name.
/// @param name The name of the asset to pull.
/// @return A pointer to the asset, or NULL if the asset was not found.
SpriteAtlas* AssetManager::getAtlas(std::string name) const
{
    name = stringToLower(name);

    if (atlases.find(name) != atlases.end())
    {
        return atlases.at(name);
    }
    else
    {
        return NULL;
    }
}

/// @brief Gets a font asset by name.
/// @param name The name of the asset to pull.
/// @return A pointer to the asset, or NULL if the asset was not found.
TTF_Font* AssetManager::getFont(std::string name) const
{
    name = stringToLower(name);

    if (fonts.find(name) != fonts.end())
    {
        return fonts.at(name);
    }
    else
    {
        return NULL;
    }
}

/// @brief Gets a bitmap font asset by name.
/// @param name The name of the asset to pull.
/// @return A pointer to the asset, or NULL if the asset was not found.
BitmapFont* AssetManager::getBitmap(std::string name) const
{
    name = stringToLower(name);

    if (bitmaps.find(name) != bitmaps.end())
    {
        return bitmaps.at(name);
    }
    else
    {
        return NULL;
    }
}

#pragma endregion Getters

#pragma region AssetLoading

/// @brief Loads the game assets into the unordered_map's.
/// @param path The JSON file path, containing asset information.
/// @return True on success. False otherwise.
bool AssetManager::loadAssets(std::string path)
{
    if (renderer == NULL || random == NULL)
    {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "AssetManager: Failed to load assets! The renderer or random number generator are NULL!");

        return false;
    }

    bool success = true;
    json assets;
    json::iterator it;

    //Clear existing assets
    clearAssets();

    SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "AssetManager: Loading assets...");

    if (!std::filesystem::exists(path))
    {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "AssetManager: File %s does not exist.", path.c_str());

        return false;
    }
    
    assets = parseJsonFileTop(path, "assets");

    //Failed to parse JSON?
    if (assets.is_null())
    {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "AssetManager: Failed to read JSON. Field \"assets\" does not exist.");

        return false;
    }

    //Load each asset
    for (it = assets.begin(); it != assets.end(); it++)
    {
        json asset = (*it);
        
        if (!loadAsset(asset))
        {
            success = false;

            break;
        }
    }

    if (!success)
    {
        SDL_LogInfo(SDL_LOG_CATEGORY_ERROR, "AssetManager: Failed to load assets!");
    }

    return success;
}

/// @brief Frees all of the game assets.
void AssetManager::clearAssets()
{
    SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "AssetManager: Freeing all assets...");

    for (auto it = textures.begin(); it != textures.end(); it++)
    {
        delete it->second;
        it->second = NULL;
    }

    for (auto it = atlases.begin(); it != atlases.end(); it++)
    {
        delete it->second;
        it->second = NULL;
    }

    for (auto it = bitmaps.begin(); it != bitmaps.end(); it++)
    {
        delete it->second;
        it->second = NULL;
    }

    for (auto it = fonts.begin(); it != fonts.end(); it++)
    {
        TTF_CloseFont(it->second);
        it->second = NULL;
    }

    textures.clear();
    atlases.clear();
    fonts.clear();
    bitmaps.clear();
}

/// @brief Displays the number of loaded assets for each asset type.
void AssetManager::displayAssetInfo() const
{
    SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "AssetManager: Loaded asset information: \
                                                \n\tTexture Count: %lu\n\tAtlas Count: %lu\n\tFont Count: %lu \
                                                \n\tBitmap Count: %lu\n\tMusic Count: %lu\n\tSound Count: %lu\n", textures.size(), atlases.size(), fonts.size(), bitmaps.size(), audioController->getMusicCount(), audioController->getSoundCount());
}

/// @brief Loads a asset by type.
/// @param asset The JSON object describing the asset to load. This includes the file path, and the asset type.
/// @return True if the asset was successfully loaded. False otherwise.
bool AssetManager::loadAsset(json asset)
{
    json filePath, assetType;
    
    filePath = asset["filePath"];
    assetType = asset["assetType"];
    
    //Confirm that string values were given
    if (!filePath.is_string())
    {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "%sIn JSON filePath should exist and should be string.", ERROR_MESSAGE_START.c_str());
        
        return false;
    }

    if (!assetType.is_string())
    {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "%sIn JSON assetType should exist and should be string.", ERROR_MESSAGE_START.c_str());

        return false;
    }

    std::string path;
    std::string typeName;
    std::string assetName;
    AssetType type;

    filePath.get_to(path);

    //Get asset name from path
    int startIndex;

    assetName = stringToLower(path);
    startIndex = assetName.find_last_of("/") + 1;
    assetName = assetName.substr(startIndex, assetName.find_last_of(".") - startIndex);

    //Get asset type
    assetType.get_to(typeName);
    typeName = stringToLower(typeName);

    //Make sure the asset type exists
    if (ASSET_NAME_MAP.find(typeName) == ASSET_NAME_MAP.end())
    {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "%sassetType %s does not exist!", ERROR_MESSAGE_START.c_str(), typeName.c_str());

        return false;
    }

    type = ASSET_NAME_MAP.at(typeName);

    SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "Loading %s asset: %s...", typeName.c_str(), path.c_str());

    //Load asset based on its type
    switch (type)
    {
        case AssetType::TEXTURE:
        {
            return loadTexture(path, assetName);
        }
        break;

        case AssetType::ATLAS:
        {
            return loadAtlas(path, assetName, asset);
        }
        break;

        case AssetType::FONT:
        {
            return loadFont(path, assetName, asset);
        }
        break;

        case AssetType::BITMAP_FONT:
        {
            return loadBitmap(path, assetName);
        }
        break;

        case AssetType::MUSIC:
        {
            return loadMusic(path, assetName);
        }
        break;

        case AssetType::SOUND:
        {
            return loadSound(path, assetName);
        }
        break;

        default:
        {
            //Do nothing (Asset not implemented)
        }
        break;
    }

    return true;
}

/// @brief Loads a texure asset.
/// @param path The file path.
/// @param assetName The name to give the asset. This is gotten from the file name in loadAsset().
/// @return True on success. False otherwise.
bool AssetManager::loadTexture(std::string path, std::string assetName)
{
    Texture *texture;

    //Make sure asset doesn't already exist
    if (textures.find(assetName) != textures.end())
    {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "%sDuplicate texture asset. Texture %s was added twice!", ERROR_MESSAGE_START.c_str(), assetName.c_str());

        return false;
    }

    texture = new Texture();

    //Load the asset
    if (!texture->loadTexture(renderer, path))
    {
        delete texture;

        return false;
    }

    textures.emplace(assetName, texture);

    return true;
}

/// @brief Loads a sprite atlas asset.
/// @param path The file path.
/// @param assetName The name to give the asset. This is gotten from the file name in loadAsset().
/// @return True on success. False otherwise.
bool AssetManager::loadAtlas(std::string path, std::string assetName, json asset)
{
    SpriteAtlas *atlas;
    json spriteWidth, spriteHeight;
    int width, height;

    //Make sure asset doesn't already exist
    if (atlases.find(assetName) != atlases.end())
    {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "%sDuplicate atlas asset. Atlas %s was added twice!", ERROR_MESSAGE_START.c_str(), assetName.c_str());

        return false;
    }

    //Get asset info
    spriteWidth = asset["spriteWidth"];
    spriteHeight = asset["spriteHeight"];

    //Confirm sprite width and height are valid
    if (!spriteWidth.is_number_integer() || !spriteHeight.is_number_integer())
    {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "%sIn JSON spriteWidth and spriteHeight should both exist and be integers!", ERROR_MESSAGE_START.c_str());

        return false;
    }

    spriteWidth.get_to(width);
    spriteHeight.get_to(height);

    //Load the asset
    atlas = new SpriteAtlas();

    if (!atlas->loadTexture(renderer, path, width, height))
    {
        delete atlas;

        return false;
    }

    atlases.emplace(assetName, atlas);

    return true;
}

/// @brief Loads a font asset.
/// @param path The file path.
/// @param assetName The name to give the asset. This is gotten from the file name in loadAsset().
/// @return True on success. False otherwise.
bool AssetManager::loadFont(std::string path, std::string assetName, json asset)
{
    TTF_Font *font;
    unsigned int ptsize;

    //Make sure asset doesn't already exist
    if (fonts.find(assetName) != fonts.end())
    {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "%sDuplicate font asset. Font %s was added twice!", ERROR_MESSAGE_START.c_str(), assetName.c_str());

        return false;
    }

    //Get the font point size
    if (!asset["ptsize"].is_number_unsigned())
    {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "%sIn asset JSON ptsize should exist and should be a non-negative integer.", ERROR_MESSAGE_START.c_str());
    }

    asset["ptsize"].get_to(ptsize);

    //Load the asset
    font = TTF_OpenFont(path.c_str(), ptsize);

    if (font == NULL)
    {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Failed to load font %s! Error: %s", assetName.c_str(), SDL_GetError());

        return false;
    }

    fonts[assetName] = font;

    return true;
}

/// @brief Loads a bitmap font asset.
/// @param path The file path.
/// @param assetName The name to give the asset. This is gotten from the file name in loadAsset().
/// @return True on success. False otherwise.
bool AssetManager::loadBitmap(std::string path, std::string assetName)
{
    BitmapFont *bitmap;

    //Make sure asset doesn't already exist
    if (bitmaps.find(assetName) != bitmaps.end())
    {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "%sDuplicate bitmap asset. Bitmap %s was added twice!", ERROR_MESSAGE_START.c_str(), assetName.c_str());

        return false;
    }
    
    //Load the bitmap
    bitmap = new BitmapFont(random);

    if (!bitmap->loadFont(renderer, path))
    {
        delete bitmap;

        return false;
    }

    bitmaps.emplace(assetName, bitmap);

    return true;
}

/// @brief Loads a music asset.
/// @param path The file path.
/// @param assetName The name to give the asset. This is gotten from the file name in loadAsset().
/// @return True on success. False otherwise.
bool AssetManager::loadMusic(std::string path, std::string assetName)
{
    return audioController->loadMusic(path.c_str(), assetName);
}

/// @brief Loads a sound asset.
/// @param path The file path.
/// @param assetName The name to give the asset. This is gotten from the file name in loadAsset().
/// @return True on success. False otherwise.
bool AssetManager::loadSound(std::string path, std::string assetName)
{
    return audioController->loadSound(path.c_str(), assetName);
}

#pragma endregion AssetLoading
