#include "objectManager.hpp"
#include "object.hpp"
#include <SDL3/SDL_log.h>
#include <algorithm>

ObjectManager::ObjectManager()
{
    nextObjectId = 0;
}

ObjectManager::~ObjectManager()
{
    freeGameObjects();
}

/// @brief Adds a game object to the game objects list.
/// @param object The game object to add.
void ObjectManager::addGameObject(Object *object)
{
    if (object == NULL)
    {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "ObjectManager: Attempted to add NULL game object!");

        return;
    }

    gameObjects.push_back(object);
    gameObjectMap[object->getId()] = object;

    SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "ObjectManager: Added game object \"%s\" ID = %d.", object->getName().c_str(), object->getId());
}

/// @brief Removes a game object from the game objects list. This does NOT call delete on the object.
/// @param object The game object to remove.
void ObjectManager::removeGameObject(Object *object)
{
    std::vector<Object*>::iterator it;

    it = std::find(gameObjects.begin(), gameObjects.end(), object);

    //Erase the game object if it exists
    if (it != gameObjects.end())
    {
        gameObjects.erase(it);
    }

    //Remove the object from the ID map
    gameObjectMap.erase(object->id);

    SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "ObjectManager: Removed game object \"%s\" ID = %d.", object->getName().c_str(), object->getId());
}

/// @brief Deletes all of the game objects from the game objects list, then clears the list. All pointers to said objects should be set to NULL.
void ObjectManager::freeGameObjects()
{
    std::vector<Object*>::iterator it;

    SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "ObjectManager: Deleting game objects...");

    //Delete each game object
    for (it = gameObjects.begin(); it != gameObjects.end(); it++)
    {
        delete (*it);
        (*it) = NULL;

	SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "ObjectManager: Deleted game object.");
    }

    gameObjects.clear();
    gameObjectMap.clear();
    std::queue<Object*>().swap(gameObjectsToDestroy);

    SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "ObjectManager: Finished deleting game objects.");
}

/// @brief Adds a game object to be destroyed.
/// @param object The game object to later destroy.
void ObjectManager::addGameObjectToDestroy(Object *object)
{
    if (object == NULL)
    {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "ObjectManager: Attempted to destroy NULL game object!");

        return;
    }

    gameObjectsToDestroy.push(object);
}

/// @brief Destroys the game objects in the destroy list.
void ObjectManager::destroyGameObjects()
{
    //Destroy the game objects.
    while (!gameObjectsToDestroy.empty())
    {
        Object *object = gameObjectsToDestroy.front();
        gameObjectsToDestroy.pop();

        removeGameObject(object);
        delete object;

        SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "ObjectManager: Destroyed game object!");
    }
}

/// @brief Updates the game objects in the game objects list. This is done in an arbitrary order
/// @param deltaTime Time change in seconds since last frame.
/// @param gamePaused Whether the game state is paused. Only objects that can run while paused will update.
void ObjectManager::updateGameObjects(double deltaTime, bool gamePaused)
{
    //Update each game object
    for (unsigned int i = 0; i < gameObjects.size(); i++)
    {
        //Skip this object if we are paused
        if (gamePaused && !gameObjects[i]->canUpdateWhilePaused())
        {
            continue;
        }

        if (!gameObjects[i]->isDestroyed())
        {
            gameObjects[i]->updateBegin(deltaTime);
            gameObjects[i]->update(deltaTime);
            gameObjects[i]->updateEnd(deltaTime);
        }
    }
}

/// @brief Gets the next unique ID from the object manager. Used to uniquely identify each object.
/// @return The next ID.
unsigned int ObjectManager::getNextObjectId()
{
    return nextObjectId++;
}

/// @brief Gets a game object by their unique identifier.
/// @param id The object ID.
/// @return The object reference, or nullptr if it wasn't found.
Object* ObjectManager::getObjectByID(int id) const
{
    if (gameObjectMap.find(id) != gameObjectMap.end())
    {
        return gameObjectMap.at(id);
    }

    return nullptr;
}

/// @return The list of game objects.
std::vector<Object*>& ObjectManager::getGameObjects()
{
    return gameObjects;
}

/// @return The game objects that are not destroyed.
std::vector<Object*> ObjectManager::getGameObjectsNotDestroyed()
{
    std::vector<Object*> notDestroyed;
    std::vector<Object*>::iterator it;

    for (it = gameObjects.begin(); it != gameObjects.end(); it++)
    {
        if (!(*it)->isDestroyed())
        {
            notDestroyed.push_back(*it);
        }
    }

    return notDestroyed;
}
