#include "room.hpp"
#include "camera.hpp"
#include "texture.hpp"
#include "json.hpp"
#include "jsonUtils.hpp"
#include "stringUtils.hpp"
#include "config.hpp"
#include "assetManager.hpp"
#include <SDL_log.h>

using json = nlohmann::json;

/// @brief Custom from_json method to deserialize JSON into RoomInfo.
/// @param j The json containing our struct values.
/// @param roomInfo The struct to populate.
void from_json(const json& j, RoomInfo& roomInfo) 
{
    // Deserialize and validate each field
    j.at("name").get_to(roomInfo.name);
    j.at("width").get_to(roomInfo.width);
    j.at("height").get_to(roomInfo.height);

    j.at("cameraStartX").get_to(roomInfo.cameraStartX);
    j.at("cameraStartY").get_to(roomInfo.cameraStartY);

    j.at("bgColorR").get_to(roomInfo.bgColorR);
    j.at("bgColorG").get_to(roomInfo.bgColorG);
    j.at("bgColorB").get_to(roomInfo.bgColorB);
    j.at("bgColorA").get_to(roomInfo.bgColorA);

    j.at("bgTextureName").get_to(roomInfo.textureName);

    j.at("bgAngle").get_to(roomInfo.bgAngle);
    j.at("bgScrollSpeedX").get_to(roomInfo.bgScrollSpeedX);
    j.at("bgScrollSpeedY").get_to(roomInfo.bgScrollSpeedY);
    j.at("bgScaleX").get_to(roomInfo.bgScaleX);
    j.at("bgScaleY").get_to(roomInfo.bgScaleY);

    j.at("tileBackground").get_to(roomInfo.tileBackground);

    roomInfo.objects = j.at("objects").get<std::vector<ObjectInfo>>();

    j.at("tilemap").get_to(roomInfo.tilemap);
}

/// @brief Custom from_json method to deserialize JSON into ObjectInfo.
/// @param j The json containing our struct values.
/// @param objectInfo The struct to populate.
/// @throws invalid_argument If the object parameters is not an array.
void from_json(const json& j, ObjectInfo& objectInfo) 
{
    // Deserialize and validate each field
    j.at("objectName").get_to(objectInfo.objectName);
    j.at("x").get_to(objectInfo.x);
    j.at("y").get_to(objectInfo.y);

    //Get the object parameters
    if (!j["parameters"].is_array())
    {
        throw std::invalid_argument("Room: JSON object parameters is not an array");
    }

    for (const auto& item : j["parameters"]) 
    {
        objectInfo.parameters.push_back(jsonToParameter(item));
    }
}

/// @brief Converts the json to a possible variant value.
/// @param j The json containing the variant.
/// @param return The variant value.
/// @throws invalid_argument If the Json is an invalid argument.
ParameterVariant jsonToParameter(const json& j) 
{
    if (j.is_number_integer())
    {
        return j.get<int>();
    }
    else if (j.is_number_float())
    {
        return j.get<double>();
    }
    else if (j.is_string())
    {
        return j.get<std::string>();
    }
    else
    {
        throw std::invalid_argument("Room: Unsupported JSON type for conversion to ParameterVariant.");
    }
}

Room::Room(AssetManager* assetManager, std::string roomName, int roomWidth, int roomHeight)
{
    this->assetManager = assetManager;

    roomLoaded = false;

    name = roomName;
    width = roomWidth;
    height = roomHeight;

    cameraStartX = 0;
    cameraStartY = 0;

    backgroundColor = {255, 255, 255, 255};

    background = NULL;
    tileBackground = false;
    bgAngle = 0.0;

    bgXOffset = 0.0; 
    bgYOffset = 0.0;
    bgScrollSpeedX = 0.0;
    bgScrollSpeedY = 0.0;
    
    bgScaleX = 1.0;
    bgScaleY = 1.0;

    tilemap = "";
}

/// @brief Loads a room using the given room name. Searches for a room JSON file of the same name.
/// @param roomName Name of the room to load
/// @return True on success, false otherwise.
bool Room::loadRoomByName(std::string roomName)
{
    json roomData;
    std::string path;

    SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "Room: Loading room: %s...", roomName.c_str());

    clearRoom();

    roomName = stringToLower(roomName);
    
    //Get the room file path
    path = Config::ROOM_PATH + "/" + roomName + ".json";

    //File exists?
    if (!std::filesystem::exists(path))
    {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Room: File %s does not exist.", path.c_str());

        return false;
    }

    roomData = parseJsonFile(path);

    //Load the room
    if (!loadRoomFromJson(roomData))
    {
        SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "Room: Failed to load room %s.", roomName.c_str());

        return false;
    }

    return true;
}

/// @brief Loads a tilemap from a json string.
/// @param str The json string to parse.
/// @return True on success, false otherwise.
bool Room::loadRoomFromString(std::string str)
{
    json roomData;

    SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "Room: Loading room from JSON string...");

    roomData = json::parse(str);

    //Load the room
    if (!loadRoomFromJson(roomData))
    {
        SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "Room: Failed to load room.");

        return false;
    }

    return true;
}

/// @brief Loads room data from the given JSON.
/// @param roomJson The room JSON.
/// @return True on success, false otherwise.
bool Room::loadRoomFromJson(json roomJson)
{
    RoomInfo roomInfo;

    //Failed to parse JSON?
    if (roomJson.is_null())
    {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Room: Failed to read room JSON.");

        return false;
    }

    //Deserialize
    try
    {
        roomInfo = roomJson.at("room").get<RoomInfo>();
    }
    catch (const json::exception& e)
    {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "RoomInfo: Error deserializing JSON: %s", e.what());

        return false;
    }

    //Load the background texture (If given)
    if (roomInfo.textureName != "")
    {
        Texture *texture = assetManager->getTexture(roomInfo.textureName);

        if (texture == NULL)
        {
            SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Room: Failed to room background texture. Asset does not exist: %s", roomInfo.textureName.c_str());

            return false;
        }

        setBackgroundTexture(texture, roomInfo.tileBackground);
    }

    //Populate variables from RoomInfo
    name = roomInfo.name;
    width = roomInfo.width;
    height = roomInfo.height;

    cameraStartX = roomInfo.cameraStartX;
    cameraStartY = roomInfo.cameraStartY;
    
    backgroundColor = {roomInfo.bgColorR, roomInfo.bgColorG, roomInfo.bgColorB, roomInfo.bgColorA};

    bgAngle = roomInfo.bgAngle;
    bgScrollSpeedX = roomInfo.bgScrollSpeedX;
    bgScrollSpeedY = roomInfo.bgScrollSpeedY;
    bgScaleX = roomInfo.bgScaleX;
    bgScaleY = roomInfo.bgScaleY;

    objects = roomInfo.objects;
    tilemap = roomInfo.tilemap;

    SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "Room: Loaded room %s", name.c_str());

    roomLoaded = true;

    return true;
}

/// @brief Clears the current room data.
void Room::clearRoom()
{
    roomLoaded = false;

    name = "DEFAULT";

    cameraStartX = 0;
    cameraStartY = 0;

    backgroundColor = {255, 255, 255, 255};

    background = NULL;
    tileBackground = false;
    bgAngle = 0.0;

    bgXOffset = 0.0; 
    bgYOffset = 0.0;
    bgScrollSpeedX = 0.0;
    bgScrollSpeedY = 0.0;
    
    bgScaleX = 1.0;
    bgScaleY = 1.0;

    objects.clear();

    tilemap = "";
}

/// @brief Clears the background texture by setting it to NULL.
void Room::clearBackgroundTexture()
{
    background = NULL;
}

/// @brief Updates the background scrolling.
void Room::updateBackground()
{
    //Do nothing if no background was set, or we are not tiling
    if (background == NULL || !tileBackground)
    {
        return;
    }

    //Scroll
    if (abs(bgScrollSpeedX) > SDL_FLT_EPSILON)
    {
        int bgWidth;

        bgWidth = background->getWidth() * bgScaleX;

        bgXOffset += bgScrollSpeedX;
        bgXOffset = fmod(bgXOffset, bgWidth);
    }

    if (abs(bgScrollSpeedY) > SDL_FLT_EPSILON)
    {
        int bgHeight;

        bgHeight = background->getHeight() * bgScaleY;

        bgYOffset += bgScrollSpeedY;
        bgYOffset = fmod(bgYOffset, bgHeight);
    }
}

/// @brief Draws the background.
/// @param renderer The renderer to draw to.
/// @param camera The game camera.
void Room::drawBackground(SDL_Renderer *renderer, Camera *camera)
{
    if (background == NULL)
    {
        //Draw the background color
        SDL_Rect rect = {0, 0, camera->getWidth(), camera->getHeight()};

        SDL_SetRenderDrawColor(renderer, backgroundColor.r, backgroundColor.g, backgroundColor.b, 255);
        SDL_RenderFillRect(renderer, &rect);
        SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);   //Reset to white
    }
    else
    {
        //Draw the background texture
        if (!tileBackground)
        {
            //Stretch to the room size
            background->renderStretched(renderer, floor(0.0 - camera->getDrawX()), floor(0.0 - camera->getDrawY()), width, height, NULL, bgAngle);
        } 
        else
        {
            int bgWidth, bgHeight;
            int drawX, drawY;

            bgWidth = background->getWidth() * bgScaleX;
            bgHeight = background->getHeight() * bgScaleY;

            //Apply x and y offset
            drawX = (int) bgXOffset;
            drawY = (int) bgYOffset;

            //Make sure to start at the top left of the room
            while (drawX > 0)
            {
                drawX -= bgWidth;
            }

            while (drawY > 0)
            {
                drawY -= bgHeight;
            }

            //Draw tiled
            for (int x = drawX; x < width; x += bgWidth)
            {
                for (int y = drawY; y < height; y += bgHeight)
                {
                    CollisionRect rect = {(double) x, (double) y, bgWidth, bgHeight};

                    //Only draw if the background tile is visible from the camera
                    if (Collision::checkCollisionRects(rect, camera->getDrawRect()))
                    {
                        background->render(renderer, x - floor(camera->getDrawX()), y - floor(camera->getDrawY()), bgScaleX, bgScaleY, NULL, bgAngle);
                    }
                }
            }
        }
    }
}

#pragma region Setters

/// @brief Sets the room name.
/// @param roomName The new name.
void Room::setRoomName(std::string roomName)
{
    name = roomName;
}

/// @brief Sets the room size.
/// @param roomWidth The rooms width.
/// @param roomHeight The rooms height.
void Room::setRoomSize(int roomWidth, int roomHeight)
{
    width = roomWidth;
    height = roomHeight;
}

/// @brief Sets the background color. Only used if no background texture is set.
/// @param color The background color.
void Room::setBackgroundColor(SDL_Color color)
{
    backgroundColor = color;
}

/// @brief Sets the background texture of the room.
/// @param background The texture to set.
/// @param tileBackground Whether or not to tile the background. If false, the background will stretch.
void Room::setBackgroundTexture(Texture *background, bool tileBackground)
{
    this->background = background;
    this->tileBackground = tileBackground;

    //Reset position
    bgXOffset = 0; 
    bgYOffset = 0;
}

/// @brief Sets the background textures scroll speed.
/// @param scrollSpeedX The x scroll speed.
/// @param scrollSpeedY The y scroll speed.
void Room::setBackgroundScrollSpeed(float scrollSpeedX, float scrollSpeedY)
{
    bgScrollSpeedX = scrollSpeedX;
    bgScrollSpeedY = scrollSpeedY;
}

/// @brief Sets the background textures scale. Only applies to tiled backgrounds.
/// @param scaleX The x scale. 1 being normal sized.
/// @param scaleY The y scale. 1 being normal sized.
void Room::setBackgroundScale(double scaleX, double scaleY)
{
    bgScaleX = scaleX;
    bgScaleY = scaleY;
}

/// @brief Sets the background textures angle.
/// @param angle Ange in degrees.
void Room::setBackgroundAngle(double angle)
{
    bgAngle = angle;
}

#pragma endregion Setters

#pragma region Getters

/// @return True if the room was loaded, false otherwise.
bool Room::getRoomLoaded()
{
    return roomLoaded;
}

/// @return The room name.
std::string Room::getName()
{
    return name;
}

/// @return The room width.
int Room::getWidth()
{
    return width;
}

/// @return The room height.
int Room::getHeight()
{
    return height;
}

/// @return The cameras starting X position.
int Room::getCameraStartX()
{
    return cameraStartX;
}

/// @return The cameras starting Y position.
int Room::getCameraStartY()
{
    return cameraStartY;
}

/// @return The background color.
SDL_Color Room::getBackgroundColor()
{
    return backgroundColor;
}

/// @return The background width scale.
double Room::getBackgroundXScale()
{
    return bgScaleX;
}

/// @return The background height scale.
double Room::getBackgroundYScale()
{
    return bgScaleY;
}

/// @return The backgrounds image angle.
double Room::getBackgroundAngle()
{
    return bgAngle;
}

/// @return The list of objects that should be created for this room.
std::vector<ObjectInfo> Room::getRoomObjects()
{
    return objects;
}

/// @return The name of the TileMap for this room, or "" if this room has no map.
std::string Room::getTileMapName()
{
    return tilemap;
}

#pragma endregion Getters