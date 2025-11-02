#include "objectManager.hpp"
#include "object.hpp"

/// @tparam T The type of game object to check.
/// @return True if the game object exists. False otherwise
template <typename T> bool ObjectManager::gameObjectExists() const
{
    static_assert(std::is_base_of<Object, T>::value, "ObjectManager:getGameObjectsOfType(): Typename T should be of type Object!");

    //Check each game object for object of type T
    for (Object* obj : gameObjects) 
    {
        T* castedObj = dynamic_cast<T*>(obj);

        if (castedObj) 
        {
            return true;
        }
    }

    return false;
}

/// @brief Gets the game objects that derive from the given object type.
/// @tparam T The object class type.
/// @return The list of game objects that derive from type T.
template <typename T> std::vector<T*> ObjectManager::getGameObjectsOfType() const
{
    static_assert(std::is_base_of<Object, T>::value, "ObjectManager:getGameObjectsOfType(): Typename T should be of type Object!");

    std::vector<T*> objects;

    //Add each game object that dervices from T
    for (Object* obj : gameObjects) 
    {
        T* castedObj = dynamic_cast<T*>(obj);

        if (castedObj) 
        {
            objects.push_back(castedObj);
        }
    }

    return objects;
}