#include "objectDrawer.hpp"
#include "objectManager.hpp"
#include "object.hpp"
#include "renderableObject.hpp"
#include "camera.hpp"

/// @brief Draws the game objects in the game objects list, in order from highest depth to lowest.
/// @param objectManager The object manager.
/// @param renderer The renderer to draw to.
/// @param camera The game camera.
void ObjectDrawer::drawGameObjects(ObjectManager *objectManager, SDL_Renderer *renderer, Camera *camera)
{
    std::vector<RenderableObject*> gameObjects = objectManager->getGameObjectsOfType<RenderableObject>();
    std::vector<Object*>::iterator it;

    //Sort game objects
    sortGameObjectsBydepth(gameObjects);

    //Draw each game object
    for (RenderableObject* obj: gameObjects)
    {
        if (!obj->isDestroyed() && obj->getRenderEnabled())
        {
            obj->renderBegin(renderer, camera);
            obj->render(renderer, camera);
            obj->renderEnd(renderer, camera);
        }
    }
}

/// @brief Draws the game objects GUIs objects list, in order from highest depth to lowest.
/// @param objectManager The object manager.
/// @param renderer The renderer to draw to.
void ObjectDrawer::drawGameObjectGuis(ObjectManager *objectManager, SDL_Renderer *renderer)
{
    std::vector<RenderableObject*> gameObjects = objectManager->getGameObjectsOfType<RenderableObject>();
    std::vector<Object*>::iterator it;

    //Sort game objects
    sortGameObjectsBydepth(gameObjects);

    //Draw each game object GUI
    for (RenderableObject* obj: gameObjects)
    {
        if (!obj->isDestroyed() && obj->getRenderGuiEnabled())
        {
            obj->renderGuiBegin(renderer);
            obj->renderGui(renderer);
            obj->renderGuiEnd(renderer);
        }
    }
}

/// @brief Toggles the game objects collision displays.
/// @param objectManager The object manager.
void ObjectDrawer::toggleGameObjectCollisionDisplays(ObjectManager *objectManager)
{
    std::vector<RenderableObject*> gameObjects = objectManager->getGameObjectsOfType<RenderableObject>();

    for (RenderableObject* obj : gameObjects)
    {
        if (!obj->isDestroyed())
        {
            obj->toggleCollisionDisplay();
        }
    }
}

/// @brief Sorts the game objects from highest depth to lowest.
/// @param gameObjects The objects to sort.
void ObjectDrawer::sortGameObjectsBydepth(std::vector<RenderableObject*> &gameObjects)
{
    //Sort the game objects by depth
    std::sort(gameObjects.begin(), gameObjects.end(), 
                [](const RenderableObject *a, const RenderableObject *b)
                {
                    return a->getDrawDepth() > b->getDrawDepth();
                });
}