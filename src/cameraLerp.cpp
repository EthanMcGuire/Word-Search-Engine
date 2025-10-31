#include "cameraLerp.hpp"

/// @brief Creates a new Lerp camera.
/// @param gameManager The game manager.
/// @param pos Camera starting position.
/// @param width Camera view width.
/// @param height Camera view height.
/// @param lerpAmount Lerp amount in % per second.
/// @param maxDistance The max distance the camera can be from its follow object.
CameraLerp::CameraLerp(GameManager *gameManager, gmtl::Vec2d pos, int width, int height, double lerpAmount, int maxDistance) : Camera(gameManager, pos, width, height, maxDistance)
{
    this->lerpAmount = lerpAmount;
    this->lerpFinishedDistance = 4;
}

void CameraLerp::move(double deltaTime, gmtl::Vec2d destination)
{
    gmtl::lerp(pos, lerpAmount * deltaTime, pos, destination);

    gmtl::Vec2d distanceVector = destination - pos;
    double distance = gmtl::length(distanceVector);

    //If distance to destination is close enough, just go there
    if (distance <= lerpFinishedDistance)
    {   
        //pos = destination;
    }
}