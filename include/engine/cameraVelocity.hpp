#pragma once

#ifndef CAMERA_VELOCITY_H
#define CAMERA_VELOCITY_H

#include "camera.hpp"

class CameraVelocity : public Camera
{
    public:
        /// @brief Creates a new Velocity camera.
        /// @param gameManager The game manager.
        /// @param pos Camera starting position.
        /// @param width Camera view width.
        /// @param height Camera view height.
        /// @param moveSpeed Move speed per second.
        /// @param maxDistance The max distance the camera can be from its follow object.
        CameraVelocity(GameManager *gameManager, gmtl::Vec2d pos, int width, int height, double moveSpeed, int maxDistance = 256);

        void setMoveSpeed(double moveSpeed);

    private:
        /// @brief Moves the camera over time. Should be overridden to provide unique camera functionality.
        /// @param deltaTime The time that passed.
        /// @param destination The goal location.
        void move(double deltaTime, gmtl::Vec2d destination) override;
        
        double moveSpeed;       //Velocity per second
};

#endif