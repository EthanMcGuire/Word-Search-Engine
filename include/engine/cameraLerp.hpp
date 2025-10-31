#pragma once

#ifndef CAMERA_LERP_H
#define CAMERA_LERP_H

#include "camera.hpp"

class CameraLerp : public Camera
{
    public:
        /// @brief Creates a new Lerp camera.
        /// @param gameManager The game manager.
        /// @param pos Camera starting position.
        /// @param width Camera view width.
        /// @param height Camera view height.
        /// @param lerpAmount Lerp amount in % per second.
        /// @param maxDistance The max distance the camera can be from its follow object.
        CameraLerp(GameManager *gameManager, gmtl::Vec2d pos, int width, int height, double lerpAmount = 2.5, int maxDistance = 256);

        void setLerpAmount(double lerpAmount);

    private:
        /// @brief Moves the camera over time. Should be overridden to provide unique camera functionality.
        /// @param deltaTime The time that passed.
        /// @param destination The goal location.
        void move(double deltaTime, gmtl::Vec2d destination) override;

        double lerpAmount;
        double lerpFinishedDistance;
};

#endif