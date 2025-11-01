#include "cameraVelocity.hpp"

/// @brief Creates a new Velocity camera.
/// @param gameManager The game manager.
/// @param pos Camera starting position.
/// @param width Camera view width.
/// @param height Camera view height.
/// @param moveSpeed Move speed per second.
/// @param maxDistance The max distance the camera can be from its follow object.
CameraVelocity::CameraVelocity(GameManager *gameManager, gmtl::Vec2d pos, int width, int height, double moveSpeed, int maxDistance) : Camera(gameManager, pos, width, height, maxDistance)
{
    this->moveSpeed = moveSpeed;
}

void CameraVelocity::move(double deltaTime, gmtl::Vec2d destination)
{
    gmtl::Vec2d moveVector;
    double distance;
    double moveAmount;

    //Get the amount to move
    moveAmount = moveSpeed * deltaTime;

    //Get the distance by subtracting vectors
    moveVector = destination - pos;
    
    distance = gmtl::length(moveVector);

    //Close enough to reach the destination?
    if (distance <= moveAmount)
    {
        pos = destination;
    }
    else
    {
        gmtl::Vec2d velocity;
        
        velocity = moveVector;

        //Set vector length to move amount
        gmtl::normalize(velocity);
        velocity *= moveAmount;  

        //Move by velocity
        pos += velocity;
    }
}