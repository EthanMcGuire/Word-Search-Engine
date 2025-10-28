#pragma once

#ifndef ASSET_MANAGER_H
#define ASSET_MANAGER_H

#include "json_fwd.hpp"
#include "spriteAtlas.hpp"
#include "bitmapFont.hpp"
#include <SDL_render.h>
#include <SDL_ttf.h>
#include <SDL_mixer.h>
#include <unordered_map>
#include <string>

using json = nlohmann::json;

class Engine;
class Random;

enum AssetType
{
    TEXTURE,
    ATLAS,
    MUSIC,
    SOUND,
    FONT,   //For TTF fonts
    BITMAP_FONT, //Bitmap fonts
    ASSET_TYPE_COUNT
};

static const std::unordered_map<std::string, AssetType> ASSET_NAME_MAP = { {"texture", AssetType::TEXTURE}, {"atlas", AssetType::ATLAS}, {"music", AssetType::MUSIC}, {"sound", AssetType::SOUND}, {"font", AssetType::FONT}, {"bitmap", AssetType::BITMAP_FONT}};
static const std::string ERROR_MESSAGE_START = "AssetManager: Failed to load asset. ";

class AssetManager
{
    public:
        /// @brief Creates the asset manager.
        /// @param renderer The window renderer. Used to load textures.
        /// @param random The RNG class. Passed to bitmap fonts for random character shake.
        AssetManager(SDL_Renderer *renderer, Random *random);

        /// @brief Gets a texture asset by name.
        /// @param name The name of the asset to pull.
        /// @return A pointer to the asset, or NULL if the asset was not found.
        Texture* getTexture(std::string name) const;

        /// @brief Gets a sprite atlas asset by name.
        /// @param name The name of the asset to pull.
        /// @return A pointer to the asset, or NULL if the asset was not found.
        SpriteAtlas* getAtlas(std::string name) const;

        /// @brief Gets a font asset by name.
        /// @param name The name of the asset to pull.
        /// @return A pointer to the asset, or NULL if the asset was not found.
        TTF_Font* getFont(std::string name) const;

        /// @brief Gets a bitmap font asset by name.
        /// @param name The name of the asset to pull.
        /// @return A pointer to the asset, or NULL if the asset was not found.
        BitmapFont* getBitmap(std::string name) const;

        /// @brief Gets a music asset by name.
        /// @param name The name of the asset to pull.
        /// @return A pointer to the asset, or NULL if the asset was not found.
        Mix_Music* getMusic(std::string name) const;

        /// @brief Gets a sound asset by name.
        /// @param name The name of the asset to pull.
        /// @return A pointer to the asset, or NULL if the asset was not found.
        Mix_Chunk* getSound(std::string name) const;

    private:
        /// @brief Loads the game assets into the unordered_map's.
        /// @param path The JSON file path, containing asset information.
        /// @return True on success. False otherwise.
        bool loadAssets(std::string path);
        
        /// @brief Frees all of the game assets.
        void clearAssets();

        /// @brief Displays the number of loaded assets for each asset type.
        void displayAssetInfo() const;

        /// @brief Loads a asset by type.
        /// @param asset The JSON object describing the asset to load. This includes the file path, and the asset type.
        /// @return True if the asset was successfully loaded. False otherwise.
        bool loadAsset(json asset);

        /// @brief Loads a texure asset.
        /// @param path The file path.
        /// @param assetName The name to give the asset. This is gotten from the file name in loadAsset().
        /// @return True on success. False otherwise.
        bool loadTexture(std::string path, std::string assetName);

        /// @brief Loads a sprite atlas asset.
        /// @param path The file path.
        /// @param assetName The name to give the asset. This is gotten from the file name in loadAsset().
        /// @param asset Contains the JSON objects describing this asset.
        /// @return True on success. False otherwise.
        bool loadAtlas(std::string path, std::string assetName, json asset);

        /// @brief Loads a font asset.
        /// @param path The file path.
        /// @param assetName The name to give the asset. This is gotten from the file name in loadAsset().
        /// @param asset Contains the JSON objects describing this asset.
        /// @return True on success. False otherwise.
        bool loadFont(std::string path, std::string assetName, json asset);

        /// @brief Loads a bitmap font asset.
        /// @param path The file path.
        /// @param assetName The name to give the asset. This is gotten from the file name in loadAsset().
        /// @return True on success. False otherwise.
        bool loadBitmap(std::string path, std::string assetName);

        /// @brief Loads a music asset.
        /// @param path The file path.
        /// @param assetName The name to give the asset. This is gotten from the file name in loadAsset().
        /// @return True on success. False otherwise.
        bool loadMusic(std::string path, std::string assetName);

        /// @brief Loads a sound asset.
        /// @param path The file path.
        /// @param assetName The name to give the asset. This is gotten from the file name in loadAsset().
        /// @return True on success. False otherwise.
        bool loadSound(std::string path, std::string assetName);

        //So the engine can access Asset loading and deleting methods
        friend class Engine;

        SDL_Renderer *renderer;
        Random *random;

        std::unordered_map<std::string, Texture*> textures;
        std::unordered_map<std::string, SpriteAtlas*> atlases;
        std::unordered_map<std::string, TTF_Font*> fonts;
        std::unordered_map<std::string, BitmapFont*> bitmaps;
        std::unordered_map<std::string, Mix_Music*> musics;
        std::unordered_map<std::string, Mix_Chunk*> sounds;
};

#endif