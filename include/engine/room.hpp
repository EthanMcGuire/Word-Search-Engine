#pragma once

#ifndef ROOM_H
#define ROOM_H

#include "json/json.hpp"
#include "parameterVariant.hpp"
#include <SDL3/SDL_render.h>
#include <string>
#include <vector>

using json = nlohmann::json;

class Texture;
class Camera;
class AssetManager;

/// @brief Info about an object in the room.
struct ObjectInfo
{
    std::string objectName;
    int x, y;
    std::vector<ParameterVariant> parameters;
};

struct RoomInfo
{
    std::string name;
    std::string musicName;
    unsigned int width, height;

    int cameraStartX, cameraStartY;

    Uint8 bgColorR, bgColorG, bgColorB, bgColorA;

    std::string textureName;

    double bgAngle;
    float bgScrollSpeedX, bgScrollSpeedY;
    double bgScaleX, bgScaleY;

    bool tileBackground;

    std::vector<ObjectInfo> objects;    //Room game objects
};

/// @brief Custom from_json method to deserialize JSON into RoomInfo.
/// @param j The json containing our struct values.
/// @param roomInfo The struct to populate.
void from_json(const json& j, RoomInfo& roomInfo);

/// @brief Custom from_json method to deserialize JSON into ObjectInfo.
/// @param j The json containing our struct values.
/// @param roomInfo The struct to populate.
void from_json(const json& j, ObjectInfo& objectInfo);

/// @brief Converts the json to a possible variant value.
/// @param j The json containing the variant.
/// @param return The variant value.
/// @throws invalid_argument If the Json is an invalid argument.
ParameterVariant jsonToParameter(const json& j);

/// @brief Defines a room in the game. 
///        This includes the x and y coordinate range for the player and the camera, the game objects, and the background colors/textures.
class Room
{
    public:
        Room(AssetManager* assetManager, std::string roomName, int roomWidth, int roomHeight);

        /// @brief Loads a room using the given room name. Searches for a room JSON file of the same name.
        /// @param roomName Name of the room to load
        /// @return True on success, false otherwise.
        bool loadRoomByName(std::string roomName);

        /// @brief Loads a tilemap from a json string.
        /// @param str The json string to parse.
        /// @return True on success, false otherwise.
        bool loadRoomFromString(std::string str);

        /// @brief Clears the current room data.
        void clearRoom();

        /// @brief Clears the background texture by setting it to NULL.
        void clearBackgroundTexture();

        /// @brief Updates the background scrolling.
        void updateBackground();

        /// @brief Draws the background.
        /// @param renderer The renderer to draw to.
        /// @param camera The game camera.
        void drawBackground(SDL_Renderer *renderer, Camera *camera);

        #pragma region Setters

        /// @brief Sets the room name.
        /// @param roomName The new name.
        void setRoomName(std::string roomName);

        /// @brief Sets the room size.
        /// @param roomWidth The rooms width.
        /// @param roomHeight The rooms height.
        void setRoomSize(int roomWidth, int roomHeight);

        /// @brief Sets the background color. Only used if no background texture is set.
        /// @param color The background color.
        void setBackgroundColor(SDL_Color color);

        /// @brief Sets the background texture of the room.
        /// @param background The texture to set.
        /// @param tileBackground Whether or not to tile the background. If false, the background will stretch.
        void setBackgroundTexture(Texture *background, bool tileBackground);

        /// @brief Sets the background textures scroll speed.
        /// @param scrollSpeedX The x scroll speed.
        /// @param scrollSpeedY The y scroll speed.
        void setBackgroundScrollSpeed(float scrollSpeedX, float scrollSpeedY);

        /// @brief Sets the background textures scale. Only applies to tiled backgrounds.
        /// @param scaleX The x scale. 1 being normal sized.
        /// @param scaleY The y scale. 1 being normal sized.
        void setBackgroundScale(double scaleX, double scaleY);

        /// @brief Sets the background textures angle.
        /// @param angle Ange in degrees.
        void setBackgroundAngle(double angle);

        #pragma endregion Setters

        #pragma region Getters

        /// @return True if the room was loaded, false otherwise.
        bool getRoomLoaded();
        
        /// @return The room name.
        std::string getName();

        /// @return The room width.
        int getWidth();

        /// @return The room height.
        int getHeight();

        /// @return The cameras starting X position.
        int getCameraStartX();

        /// @return The cameras starting Y position.
        int getCameraStartY();

        /// @return The background color.
        SDL_Color getBackgroundColor();

        /// @return The background width scale.
        double getBackgroundXScale();

        /// @return The background height scale.
        double getBackgroundYScale();

        /// @return The backgrounds image angle.
        double getBackgroundAngle();

        /// @return The list of objects that should be created for this room.
        std::vector<ObjectInfo> getRoomObjects();

        /// @return Returns the name of the music for this room. If no music is defined for this room, "" is returned.
        std::string getRoomMusic();
        
        #pragma endregion Getters

    private:
        /// @brief Loads room data from the given JSON.
        /// @param roomJson The room JSON.
        /// @return True on success, false otherwise.
        bool loadRoomFromJson(json roomJson);

        AssetManager *assetManager;

        std::string name;
        std::string musicName;
        int width, height;
        int cameraStartX, cameraStartY;
        bool roomLoaded;

        //Room background
        SDL_Color backgroundColor;

        Texture *background;
        bool tileBackground;    //Whether to tile the background texture (or stretch)
        double bgAngle;

        //These only apply for a tiled background
        float bgXOffset, bgYOffset;
        float bgScrollSpeedX, bgScrollSpeedY;   //Speed per second (delta time)

        double bgScaleX, bgScaleY;

        //Objects
        std::vector<ObjectInfo> objects;
};

#endif