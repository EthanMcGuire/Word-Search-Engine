#include "renderableObject.hpp"
#include "gameManager.hpp"
#include "eventDispatcher.hpp"
#include "camera.hpp"
#include "collisionDrawer.hpp"
#include <SDL_render.h>
#include <SDL_log.h>

RenderableObject::RenderableObject(std::string name, GameManager* gameManager, double x, double y) : Object(name, gameManager, x, y)
{
    depth = 0;
    visible = true;
    renderEnabled = true;
    renderGuiEnabled = true;
    showCollision = false;

    renderBBox.setCollisionType(CollisionType::NONE);

    cameraMovedEventListenerId = gameManager->getEventDispatcher()->addListener<EventCameraMoved>([this](EventCameraMoved *event) {
        this->cameraMovedCallback(event);
    });
}

RenderableObject::~RenderableObject()
{
    gameManager->getEventDispatcher()->removeListener(cameraMovedEventListenerId);
}

/// @brief Renders the object. Called after renderBegin and render. By default draws this objects collision.
/// @param renderer The renderer to draw to.
/// @param camera The game camera.
void RenderableObject::renderEnd(SDL_Renderer *renderer, Camera *camera)
{
    if (!visible)
    {
        return;
    }

    //Draw collision
    if (showCollision)
    {
        CollisionDrawer::drawCollision(renderer, camera, bBox);
    }
}

/// @brief Renders this game objects GUI. Called after renderGuiBegin and renderGui. By default draws this objects rendering collision.
/// @param renderer The renderer to draw to.
void RenderableObject::renderGuiEnd(SDL_Renderer *renderer)
{
    if (!visible)
    {
        return;
    }

    //Draw collision
    if (showCollision)
    {
        CollisionDrawer::drawCollision(renderer, nullptr, renderBBox, {255, 255, 0, 128});
    }
}

/// @brief Sets this objects rendering bounding box.
/// @param type The type of collision.
/// @param left The boxes left offset.
/// @param top The boxes top offset.
/// @param right The boxes right offset.
/// @param bottom The boxes bottom offset.
void RenderableObject::setRenderBBox(CollisionType type, int left, int top, int right, int bottom)
{
    renderBBox.setCollisionType(type);
    renderBBox.setBBox(left, top, right, bottom);

    updateBBoxPosition();
}

/// @brief Updates the position of this Objects bounding box.
void RenderableObject::updateBBoxPosition()
{
    Object::updateBBoxPosition();

    Camera *camera = gameManager->getCamera();
    renderBBox.setLocation(pos[0] - origin[0] - camera->getDrawX(), pos[1] - origin[1] - camera->getDrawY());
}

/// @brief Toggles this game objects collision display.
void RenderableObject::toggleCollisionDisplay()
{
    showCollision = !showCollision;
}

/// @brief Sets this objects draw depth.
/// @param drawDepth The draw depth. The lower the value, the more objects it will be on top of. 
void RenderableObject::setDrawDepth(int drawDepth)
{
    depth = drawDepth;
}

/// @brief Used to get the draw depth. Used in the ObjectManager when determining draw order.
/// @return This objects draw depth. 
int RenderableObject::getDrawDepth() const
{
    return depth;
}

/// @brief Sets whether this object is visible. Rendering will be aborted for non-visible objects.
/// @param isVisible True if the object should be visible.
void RenderableObject::setVisible(bool isVisible)
{
    visible = isVisible;
}

/// @return True if this object is visible.
bool RenderableObject::getVisible() const
{
    return visible;
}

/// @brief Sets whether this object is should run their render event.
/// @param enabled Whether the render event should be enabled.
void RenderableObject::setRenderEnabled(bool enabled)
{
    renderEnabled = enabled;
}

/// @return True if this objects render event is enabled.
bool RenderableObject::getRenderEnabled()
{
    return renderEnabled;
}

/// @brief Sets whether this object is should run their renderGui event.
/// @param enabled Whether the renderGui event should be enabled.
void RenderableObject::setRenderGuiEnabled(bool enabled)
{
    renderGuiEnabled = enabled;
}

/// @return True if this objects renderGui event is enabled.
bool RenderableObject::getRenderGuiEnabled()
{
    return renderGuiEnabled;
}

/// @return This objects rendering bounding box. 
BBox RenderableObject::getRenderBBox() const
{
    return renderBBox;
}

/// @brief Handles the EventCameraMoved event.
/// @param event The event info.
void RenderableObject::cameraMovedCallback(EventCameraMoved *event)
{
    renderBBox.setLocation(pos[0] - origin[0] - event->cameraX, pos[1] - origin[1] - event->cameraY);
}