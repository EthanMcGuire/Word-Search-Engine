#pragma once

#ifndef OBJECT_MANAGER_H
#define OBJECT_MANAGER_H

#include <vector>
#include <queue>
#include <unordered_map>

class Object;

class ObjectManager
{
    public:
        ObjectManager();
        ~ObjectManager();

        /// @brief Adds a game object to the game objects list.
        /// @param object The game object to add.
        void addGameObject(Object *object);

        /// @brief Removes a game object from the game objects list. This does NOT call delete on the object.
        /// @param object The game object to remove.
        void removeGameObject(Object *object);

        /// @brief Deletes all of the game objects from the game objects list, then clears the list. All pointers to said objects should be set to NULL.
        void freeGameObjects();

        /// @brief Adds a game object to be destroyed.
        /// @param object The game object to later destroy.
        void addGameObjectToDestroy(Object *object);

         /// @brief Destroys the game objects in the destroy list.
        void destroyGameObjects();

        /// @brief Updates the game objects in the game objects list. This is done in an arbitrary order
        /// @param deltaTime Time change in seconds since last frame.
        /// @param gamePaused Whether the game state is paused. Only objects that can run while paused will update.
        void updateGameObjects(double deltaTime, bool gamePaused);

        /// @tparam T The type of game object to check.
        /// @return True if the game object exists. False otherwise
        template <typename T> bool gameObjectExists() const;

        /// @brief Gets the next unique ID from the object manager. Used to uniquely identify each object.
        /// @return The next ID.
        unsigned int getNextObjectId();

        /// @brief Gets a game object by their unique identifier.
        /// @param id The object ID.
        /// @return The object reference, or nullptr if it wasn't found.
        Object* getObjectByID(int id) const;

        /// @return The list of game objects.
        std::vector<Object*>& getGameObjects();

        /// @return The game objects that are not destroyed.
        std::vector<Object*> getGameObjectsNotDestroyed();

        /// @brief Gets the game objects that derive from the given object type.
        /// @tparam T The object class type.
        /// @return The list of game objects that derive from type T.
        template <typename T> std::vector<T*> getGameObjectsOfType() const;

    private:
        std::vector<Object*> gameObjects;
        std::unordered_map<int, Object*> gameObjectMap;
        std::queue<Object*> gameObjectsToDestroy;  //Objects to destroy at the end of the current frame

        unsigned int nextObjectId;
};

#include "objectManager.tpp"

#endif