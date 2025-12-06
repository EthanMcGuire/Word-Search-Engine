#include "objectManager.hpp"
#include "object.hpp"
#include "objectFactory.hpp"
//#include "player.hpp"
//#include "menu.hpp"
//#include "menuManager.hpp"
#include <SDL3/SDL_log.h>
#include <typeinfo>

#pragma region Game_Objects

/// @brief Creates a game object from the ObjectFactory, Adding the object to the ObjectManager.
///        For Menu's, the owningPlayer will be set to NULL. To set the owningPlayer, call createMenuObject() instead.
/// @tparam ...Args Extra arg types.
/// @tparam T Object class to return. MUST derive from Object.
/// @param objectName The name of the object to create.
/// @param x X position.
/// @param y Y position.
/// @param ...args Extra args
/// @throws bad_cast When casting fails for the given Object class.
/// @return The object reference.
template <typename T, typename... Args>
T* GameManager::createObject(std::string objectName, double x, double y, Args... args)
{
    Object *object = createBasicObject(objectName, x, y, args...);
    
    /*
    Menu *menu = dynamic_cast<Menu*>(object);

    //Set the owning player if it is a Menu
    if (menu)
    {
        setMenuPlayerIndex(menu, 0);
    } 
    */

    T *objectRef = dynamic_cast<T*>(object);

    if (!objectRef)
    {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "GameManager: Dynamic cast failed when creating object! You must have specified the incorrect Object class.");

        throw std::bad_cast();
    }

    return objectRef; 
}

/// @brief Creates a game object from the ObjectFactory, Adding the object to the ObjectManager.
///        For Menu's, the owningPlayer will be set to NULL. To set the owningPlayer, call createMenuObject() instead.
/// @tparam ...Args Extra arg types.
/// @tparam T Object class to return. MUST derive from Object.
/// @param objectName The name of the object to create.
/// @param x X position.
/// @param y Y position.
/// @param ...args Extra args
/// @return The object reference.
template <typename T, typename... Args>
T* GameManager::createObject(std::string objectName, double x, double y, std::vector<ParameterVariant> args)
{
    Object *object = createBasicObject(objectName, x, y, args);

    /*
    Menu *menu = dynamic_cast<Menu*>(object);

    //Set the owning player if it is a Menu
    if (menu)
    {
        setMenuPlayerIndex(menu, 0);
    }
    */

    T *objectRef = dynamic_cast<T*>(object);

    if (!objectRef)
    {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "GameManager: Dynamic cast failed when creating object! You must have specified the incorrect Object class.");

        throw std::bad_cast();
    }

    return objectRef; 
}

/// @brief Creates a basic object from the ObjectFactory, Adding the object to the ObjectManager.
/// @tparam ...Args Extra arg types.
/// @param objectName The name of the object to create.
/// @param x X position.
/// @param y Y position.
/// @param ...args Extra args
/// @return The object reference.
template <typename... Args>
Object* GameManager::createBasicObject(std::string objectName, double x, double y, Args... args)
{
    SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "GameManager: Creating game object...");
    
    Object *object = ObjectFactory::createObject(this, objectName, x, y, args...);

    //Add the game object
    if (object != nullptr)
    {
        addGameObject(object);
    }

    return object;
}

/// @brief Creates a basic object from the ObjectFactory, Adding the object to the ObjectManager.
/// @tparam ...Args Extra arg types.
/// @param objectName The name of the object to create.
/// @param x X position.
/// @param y Y position.
/// @param ...args Extra args
/// @return The object reference.
template <typename... Args>
Object* GameManager::createBasicObject(std::string objectName, double x, double y, std::vector<ParameterVariant> args)
{
    SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "GameManager: Creating game object...");
    
    Object *object = ObjectFactory::createObject(this, objectName, x, y, args);

    //Add the game object
    if (object != nullptr)
    {
        addGameObject(object);
    }
    
    return object;
}

/// @tparam T The type of game object to check.
/// @return True if the game object exists. False otherwise
template <typename T> bool GameManager::gameObjectExists() const
{
    return objectManager->gameObjectExists<T>();
}

/*
/// @brief Creates a Menu object using the ObjectFactory. Sets the Menus owningPlayer for input handling.
/// @param playerInputIndex The index of the player input that should be used for this menu. Use 0 for default input.
/// @param objectName The name of the object to create.
/// @param x X position.
/// @param y Y position.
/// @param ...args Extra args
/// @return The menu object reference, or nullptr if an error occured.
template <typename... Args>
Menu* GameManager::createMenuObject(int playerInputIndex, std::string objectName, double x, double y, Args... args)
{
    SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "GameManager: Creating menu...");

    Object *object = createBasicObject(objectName, x, y, args...);
    Menu *menu = dynamic_cast<Menu*>(object);

    if (menu)
    {
        setMenuPlayerIndex(menu, playerInputIndex);
    }
    else
    {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "GameManager: In createMenuObject() a non-menu object was created. This method should only be used for Menu objects.");
    }

    return menu;
}

/// @brief Creates a Menu object using the ObjectFactory. Sets the Menus owningPlayer for input handling.
/// @param playerInputIndex The index of the player input that should be used for this menu. Use 0 for default input.
/// @param objectName The name of the object to create.
/// @param x X position.
/// @param y Y position.
/// @param ...args Extra args
/// @return The menu object reference, or nullptr if an error occured.
template <typename... Args>
Menu* GameManager::createMenuObject(int playerInputIndex, std::string objectName, double x, double y, std::vector<ParameterVariant> args)
{
    SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "GameManager: Creating menu...");

    Object *object = createBasicObject(objectName, x, y, args);
    Menu *menu = dynamic_cast<Menu*>(object);

    if (menu)
    {
        setMenuPlayerIndex(menu, playerInputIndex);
    }
    else
    {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "GameManager: In createMenuObject() a non-menu object was created. This method should only be used for Menu objects.");
    }

    return menu;
}
*/

#pragma endregion Game_Objects

#pragma region Collision

/// @brief Checks if a collision is meeting with a specific game object type.
/// @tparam T The game object type to check.
/// @param objectBBox The bounding box.
/// @return True on collision, false otherwise.
template <typename T> bool GameManager::placeMeetingGameObject(BBox objectBBox) const
{
    //If no defined collision, just return false.
    if (objectBBox.getCollisionType() == CollisionType::NONE)
    {
        return false;
    }

    std::vector<T*> objects = objectManager->getGameObjectsOfType<T>();
    
    //Check if meeting with any of these objects
    for (T* obj : objects) 
    {
        if (Collision::collisionMeeting(objectBBox, obj->getBBox()))
        {
            return true;
        }
    }

    return false;
}

/// @brief Checks for a collision with a object type, and returns the first found instance if so.
/// @tparam T The game object type to check.
/// @param objectBBox The bounding box.
/// @return The Object on collision, NULL otherwise.
template <typename T> T* GameManager::getInstancePlace(BBox objectBBox) const
{
    //If no defined collision, just return NULL.
    if (objectBBox.getCollisionType() == CollisionType::NONE)
    {
        return NULL;
    }

    std::vector<T*> objects = objectManager->getGameObjectsOfType<T>();

    //Check if meeting with any of these objects
    for (T* obj : objects) 
    {
        if (Collision::collisionMeeting(objectBBox, obj->getBBox()))
        {
            return obj;
        }
    }

    return NULL;
}

/// @brief Checks for and returns all meeting instances of an object type.
/// @tparam T The game object type to check.
/// @param objectBBox The bounding box.
/// @return The list of meeting game objects.
template <typename T> std::vector<T*> GameManager::getInstancePlaceList(BBox objectBBox) const
{
    std::vector<T*> meetingObjects;

    //If no defined collision, just return
    if (objectBBox.getCollisionType() == CollisionType::NONE)
    {
        return meetingObjects;
    }

    std::vector<T*> objects = objectManager->getGameObjectsOfType<T>();

    //Check if meeting with any of these objects
    for (T* obj : objects) 
    {
        if (Collision::collisionMeeting(objectBBox, obj->getBBox()))
        {
            meetingObjects.push_back(obj);
        }
    }

    return meetingObjects;
}

#pragma endregion Collision
