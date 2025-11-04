#pragma once

#ifndef OBJECT_DRAWER_H
#define OBJECT_DRAWER_H

#include <SDL3/SDL_render.h>
#include <vector>

class ObjectManager;
class Object;
class RenderableObject;
class Camera;

class ObjectDrawer
{
    public:
		//Don't make instances of this class
		ObjectDrawer()=delete;
	    ObjectDrawer(const ObjectDrawer&)=delete;
	    ObjectDrawer(ObjectDrawer&&)=delete;
  
        /// @brief Draws the game objects in the game objects list, in order from highest depth to lowest.
        /// @param objectManager The object manager.
        /// @param renderer The renderer to draw to.
        /// @param camera The game camera.
        static void drawGameObjects(ObjectManager *objectManager, SDL_Renderer *renderer, Camera *camera);

        /// @brief Draws the game objects GUIs objects list, in order from highest depth to lowest.
        /// @param objectManager The object manager.
        /// @param renderer The renderer to draw to.
        static void drawGameObjectGuis(ObjectManager *objectManager, SDL_Renderer *renderer);

        /// @brief Toggles the game objects collision displays.
        /// @param objectManager The object manager.
        static void toggleGameObjectCollisionDisplays(ObjectManager *objectManager);
    
    private:
        /// @brief Sorts the game objects from highest depth to lowest.
        /// @param gameObjects The objects to sort.
        static void sortGameObjectsBydepth(std::vector<RenderableObject*> &gameObjects);
};

#endif