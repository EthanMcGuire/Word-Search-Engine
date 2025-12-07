#include "gameManager.hpp"
#include "audioController.hpp"
#include "assetManager.hpp"
#include "engine.hpp"
#include "room.hpp"
#include "cameraLerp.hpp"
#include "config.hpp"
#include "eventDispatcher.hpp"
#include "objectFactory.hpp"
#include "objectManager.hpp"
#include "objectDrawer.hpp"
#include "object.hpp"

GameManager::GameManager(Engine *engine)
{
    this->engine = engine;

    gamePaused = false;
    showFPS = true;
}

GameManager::~GameManager()
{
    if (room != NULL)
    {
        delete room;
        room = NULL;
    }

    if (camera != NULL)
    {
        delete camera;
        camera = NULL;
    }

    if (objectManager != NULL)
    {
        delete objectManager;
        objectManager = NULL;
    }
}

/// @brief Initializes the game manager by creating the room and camera.
void GameManager::initializeGameManager()
{
    room = new Room(engine->getAssetManager(), "DEFAULT", Config::SCREEN_WIDTH, Config::SCREEN_HEIGHT);
    camera = new CameraLerp(this, gmtl::Vec2d(0, 0), Config::SCREEN_WIDTH, Config::SCREEN_HEIGHT);
    objectManager = new ObjectManager();

    //Top level input events
    getEventDispatcher()->addSDLListener(SDL_EVENT_KEY_DOWN, [this](SDL_Event &e) {
        this->keyboardCallback(e);
    });

    getEventDispatcher()->addSDLListener(SDL_EVENT_KEY_UP, [this](SDL_Event &e) {
        this->keyboardCallback(e);
    });
}

#pragma region GameControl

/// @brief Ends the game.
void GameManager::endGame() const
{
    engine->endGame();
}

/// @brief Pauses the game.
void GameManager::pauseGame()
{
    gamePaused = true;
}

/// @brief Unpauses the game.
void GameManager::unpauseGame()
{
    gamePaused = false;
}

#pragma endregion GameControl

#pragma region Game_Loop

/// @brief Updates the game.
/// @param deltaTime Time change between last and current frame.
/// @return True on success, false on failure.
bool GameManager::update(double deltaTime)
{
    //Update background
    room->updateBackground();

    //Update objects
    objectManager->updateGameObjects(deltaTime, gamePaused);

    //Update camera
    if (!getGamePaused())
    {
        camera->update(room, deltaTime, getRandom());
    }

    //Load a new room
    if (readyToLoadRoom)
    {
        readyToLoadRoom = false;

        if (!loadRoom())
        {
            SDL_LogError(SDL_LOG_CATEGORY_ERROR, "GameManager: Failed to load room %s.", nextRoom.c_str());

            return false;
        }
    }

    return true;
}

/// @brief Draws the room background and the game objects.
/// @param renderer The game windows renderer.
void GameManager::draw(SDL_Renderer *renderer)
{
    //Draw background
    room->drawBackground(renderer, camera);

    //Draw objects
    ObjectDrawer::drawGameObjects(objectManager, renderer, camera);
}

/// @brief Draws the game GUI. This includes the HUD and any debug information.
/// @param renderer The game windows renderer.
void GameManager::drawGui(SDL_Renderer *renderer)
{
    //Default font
    BitmapFont *font = getAssetManager()->getBitmap("fntOpenSans");

    if (font == NULL)
    {
        SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "GameManager: Font fntOpenSans does not exist!");

        return;
    }

    //Draw game objects GUIs
    ObjectDrawer::drawGameObjectGuis(objectManager, renderer);

    //Draw debug info
    if (showFPS)
    {
        std::string text = "FPS: ";
        text += std::to_string((int) floor(engine->getFPS()));

        font->drawText(renderer, 8, 8, text);
    }
}

#pragma endregion Game_Loop

#pragma region Game_State

/// @brief Sets a room to load at the end of the frame.
/// @param nextRoomToLoad The name of the room to load.
void GameManager::setRoomToLoad(std::string nextRoomToLoad)
{
    SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "GameManager: Set next room to load: %s", nextRoomToLoad.c_str());

    readyToLoadRoom = true;
    nextRoom = nextRoomToLoad;
}

/// @brief Loads a room (a level). Clears all game data like game objects. Also creates base objects for the room.
/// @return True on success, false otherwise.
bool GameManager::loadRoom()
{
    SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "GameManager: Loading room %s", nextRoom.c_str());

    resetGameData();

    if (!room->loadRoomByName(nextRoom))
    {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "GameManager: Failed to load room.");

        return false;
    }

    //Set the camera position
    camera->setPosition(room->getCameraStartX(), room->getCameraStartY());

    //Create game objects
    std::vector<ObjectInfo> objects = room->getRoomObjects();

    try
    {
	    for (const auto& object : objects)
	    {
		createObject(object.objectName, object.x, object.y, object.parameters);
	    }
    }
    catch (std::exception& ex)
    {
	    SDL_LogError(SDL_LOG_CATEGORY_ERROR, "GameManager: Failed to create objects when loading room. %s", ex.what());

	    return false;
    }

    //Play music
    AudioController *audioController;
    std::string musicName;

    audioController = engine->getAudioController();

    musicName = room->getRoomMusic();

    if (musicName != "")
    {
        if (!audioController->playMusic(musicName))
        {
            SDL_LogError(SDL_LOG_CATEGORY_ERROR, "GameManager: Failed to play music for the room: %s", nextRoom.c_str());

            return false;
        }
    }
    else
    {
        audioController->stopMusic();
    }

    return true;
}

/// @brief Resets the game data including game objects and the camera.
void GameManager::resetGameData()
{
    clearGameObjects();

    //Reset the room
    room->clearRoom();

    //Reset the camera
    camera->setPosition(0, 0);
}

#pragma endregion Game_State

#pragma region Game_Objects

/// @brief Destroys a game object.
/// @param object The object to destroy.
void GameManager::destroyGameObject(Object *object)
{
    if (object == nullptr)
    {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "GameManager: Attemped to destroy a NULL game object.");

        return;
    }

    object->setDestroyed(true);
    pendObjectForDestruction(object);
}

/// @brief Destroys a game object.
/// @param id ID of the object to destroy.
void GameManager::destroyGameObject(int id)
{
    Object *object;

    object = objectManager->getObjectByID(id);

    if (object == nullptr)
    {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "GameManager: Attemped to destroy object of ID: %d. This object is NULL.", id);

        return;
    }

    destroyGameObject(object);
}

/// @brief Destroys the game objects that are pending to be destroyed. Should be called at the end of each frame.
void GameManager::destroyGameObjects()
{
    objectManager->destroyGameObjects();
}

/// @brief Gets the next unique ID from the object manager. Used to uniquely identify each object.
/// @return The next ID.
unsigned int GameManager::getNextObjectId()
{
    return objectManager->getNextObjectId();
}

/// @brief Destroys all of the game objects.
void GameManager::clearGameObjects()
{
    SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "GameManager: Clearing all game objects...");
    
    //menuManager->clearMenus();          //This needs to be done before freeGameObjects, as calling freeGameObjects first will result in null destroy calls
    objectManager->freeGameObjects();
}

/// @brief Adds a game object to the object manager. This does NOT create the game object.
///        If the object is a Player, adds it to the Players list.
///        If the object is a Menu, adds it to the MenuManager.
/// @param object The object to add. This should not be NULL.
void GameManager::addGameObject(Object *object)
{
    objectManager->addGameObject(object);
}

/// @brief Pends a game object for deletion by adding it to the destroy list. The game object will be marked as destroyed.
/// @param object The game object to kill.
void GameManager::pendObjectForDestruction(Object *object)
{
    objectManager->addGameObjectToDestroy(object);
}

#pragma endregion Game_Objects

#pragma region Input

/// @brief Keyboard callback for debugging purposes.
/// @param e Event info.
void GameManager::keyboardCallback(SDL_Event &e)
{
    if (e.key.type == SDL_EVENT_KEY_DOWN)
    {
        if (e.key.repeat > 0)
        {
            //Button held
        }
        else
        {
            //Button pressed
            switch (e.key.key)
            {
                case SDLK_F1:
                {
                    showFPS = !showFPS;
                }
                break;

                case SDLK_F2:
                {
                    ObjectDrawer::toggleGameObjectCollisionDisplays(objectManager);
                }
                break;
            }
        }
    } 
    else if (e.key.type == SDL_EVENT_KEY_UP)
    {
        //Button released
    }
}

#pragma endregion Input

#pragma region Getters

/// @return True if the game is paused, false otherwise.
bool GameManager::getGamePaused()
{
    return gamePaused;
}

/// @return The game camera.
Camera* GameManager::getCamera() const
{
    return camera;
}

/// @return The current room.
Room* GameManager::getRoom() const
{
    return room;
}

/// @return The Random number generator class.
Random* GameManager::getRandom() const
{
    return engine->getRandomNumberGenerator();
}

/// @return The asset manager. Used to pull various assets.
AssetManager* GameManager::getAssetManager() const
{
    return engine->getAssetManager();
}

/// @return The audio controller. Used to play music and sounds.
AudioController* GameManager::getAudioController() const
{
    return engine->getAudioController();
}

/// @return The EventDispatcher. Used to add event listeners and dispatch events.
EventDispatcher* GameManager::getEventDispatcher() const
{
    return engine->getEventDispatcher();
}

/// @brief Pulls a bitmap font from the asset manager.
/// @param name The font name.
/// @return The font, or nullptr.
BitmapFont* GameManager::getBitmapFont(std::string name) const
{
    return getAssetManager()->getBitmap(name);
}

#pragma endregion Getters
