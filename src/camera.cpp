#include "camera.hpp"
//#include "object.hpp"
#include "random.hpp"
#include "gameManager.hpp"
#include "eventDispatcher.hpp"
#include <SDL3/SDL_stdinc.h>
#include <SDL3/SDL_log.h>
#include <float.h>

/// @brief Creates a new camera.
/// @param gameManager The game manager.
/// @param pos Camera starting position.
/// @param width Camera view width.
/// @param height Camera view height.
/// @param maxDistance The max distance the camera can be from its follow object.
Camera::Camera(GameManager *gameManager, gmtl::Vec2d pos, int width, int height, int maxDistance)
{
    this->gameManager = gameManager;
    this->pos = pos;
    this->dest = pos;
    this->width = width;
    this->height = height;
    this->maxDistance = maxDistance;

    //followObject = NULL;

    screenshake = 0;
    screenshakeAmount = 1;
    shakeX = 0;
    shakeY = 0;
}

/// @brief Gets the drawing position of the camera.
/// @return The drawing position. This is the position converted to integers.
gmtl::Vec2i Camera::getDrawPos() const
{
    return gmtl::Vec2i(getDrawX(), getDrawY());
}

/// @brief Gets the drawing position on the X axis.
/// @return The X coordinate.
int Camera::getDrawX() const
{
    return pos[0] + shakeX;
}

/// @brief Gets the drawing position on the Y axis.
/// @return The Y coordinate.
int Camera::getDrawY() const
{
    return pos[1] + shakeY;
}

/// @return The cameras width. 
int Camera::getWidth() const
{
    return width;
}

/// @return The cameras height. 
int Camera::getHeight() const
{
    return height;
}

/// @return The cameras bounding rectangle for drawing.
CollisionRect Camera::getDrawRect() const
{
    CollisionRect rect;

    rect.x = round(getDrawX());
    rect.y = round(getDrawY());
    rect.w = width;
    rect.h = height;

    return rect;
}

/// @brief Updates the cameras location.
/// @param room The room the camera is moving in. Used to clamp the cameras location.
/// @param deltaTime Time change in seconds since last frame.
/// @param random The random number generator. For screenshake.
void Camera::update(Room *room, double deltaTime, Random *random)
{
    gmtl::Vec2d destination;
    
    /*
    if (followObject != NULL)
    {
        //Follow the game object
        destination = gmtl::Vec2d(followObject->pos[0] - width / 2, followObject->pos[1] - height / 2);

        //Clamp distance
        gmtl::Vec2d distance = pos - destination;

        if (gmtl::length(distance) > maxDistance)
        {
            gmtl::normalize(distance);
            distance *= maxDistance;

            pos = destination + distance;
        }
    }
    else 
    */
    
    if (!gmtl::isEqual(pos, dest, DBL_EPSILON))
    {
        destination = dest;
    }

    //Move
    move(deltaTime, destination);
    
    //Clamp in the room
    pos[0] = SDL_clamp(pos[0], 0, room->getWidth() - width);
    pos[1] = SDL_clamp(pos[1], 0, room->getHeight() - height);

    //Apply screenshake
    shakeX = 0;
    shakeY = 0;

    if (screenshake > 0)
    {
        screenshake--;

        shakeX = random->getRandomInt(-screenshakeAmount, screenshakeAmount);
        shakeY = random->getRandomInt(-screenshakeAmount, screenshakeAmount);
    }

    dispatchCameraMovedEvent();
}

/// @brief Dispatches the EventCameraMoved event, updating listeners of the Cameras position.
void Camera::dispatchCameraMovedEvent() const
{
    EventCameraMoved event(getDrawX(), getDrawY());

    gameManager->getEventDispatcher()->dispatchEvent(&event);
}

void Camera::addScreenshake(int time)
{
    screenshake += time;
}

void Camera::setScreenshakeAmount(int amount)
{
    screenshakeAmount = amount;
}

void Camera::clearScreenshake()
{
    screenshake = 0;
}

#pragma region Setters

void Camera::setDest(gmtl::Vec2d dest)
{
    this->dest = dest;
}

/*
void Camera::setFollowObject(Object *object)
{
    followObject = object;
}
*/

void Camera::setPosition(double x, double y)
{
    pos[0] = x;
    pos[1] = y;

    dispatchCameraMovedEvent();
}

#pragma endregion Setters