#include "object.hpp"
#include "gameManager.hpp"
#include <SDL3/SDL_log.h>

/// @brief Creates a game object.
/// @param name Object name.
/// @param gameManager The objects game manager.
/// @param x X spawn.
/// @param y Y spawn.
Object::Object(std::string name, GameManager *gameManager, double x, double y)
{
    //Set unique identifier
    id = gameManager->getNextObjectId();

    this->name = name;
    this->gameManager = gameManager;

    SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "Created \"%s\" game object. ID = %d", this->name.c_str(), id);

    updateWhilePaused = false;
    destroyed = false;  

    origin = gmtl::Vec2i(0, 0);
    scale = gmtl::Vec2d(1, 1);
    angle = 0.0;

    setPosition(x, y);

    bBox.setCollisionType(CollisionType::NONE);
}

/// @brief Marks the game object as destroyed and informs the object manager to delete it.
void Object::destroy()
{
    if (destroyed)
    {
        return;
    }

    SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "Object destroyed: \"%s\", ID = %d", this->name.c_str(), id);

    destroyed = true;
    gameManager->destroyGameObject(this);
}

/// @brief Sets the position of this object. This also moves the objects collision.
/// @param x X position.
/// @param y Y position.
void Object::setPosition(double x, double y)
{
    pos[0] = x;
    pos[1] = y;

    updateBBoxPosition();
}

/// @brief Sets this objects origin.
/// @param x Origin X.
/// @param y Origin Y.
void Object::setOrigin(int x, int y)
{
    origin[0] = x;
    origin[1] = y;

    updateBBoxPosition();
}

/// @brief Sets this objects bounding box.
/// @param type The type of collision.
/// @param left The boxes left offset.
/// @param top The boxes top offset.
/// @param right The boxes right offset.
/// @param bottom The boxes bottom offset.
void Object::setBBox(CollisionType type, int left, int top, int right, int bottom)
{
    bBox.setCollisionType(type);
    bBox.setBBox(left, top, right, bottom);

    updateBBoxPosition();
}

/// @brief Updates the position of this Objects bounding box.
void Object::updateBBoxPosition()
{
    bBox.setLocation(pos[0] - origin[0], pos[1] - origin[1]);
}

void Object::setDestroyed(bool destroyed)
{
    this->destroyed = destroyed;
}

/// @return True if the object was destroyed. False otherwise.
bool Object::isDestroyed() const
{
    return destroyed;
}

/// @return True if this object can update while the game is paused. False otherwise.
bool Object::canUpdateWhilePaused()
{
    return updateWhilePaused;
}

/// @return This game objects name. 
std::string Object::getName() const
{
    return name;
}

/// @return This objects bounding box. 
BBox Object::getBBox() const
{
    return bBox;
}

/// @return This objects unique identifier.
unsigned int Object::getId()
{
    return id;
}