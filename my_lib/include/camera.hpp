#pragma once

#ifndef CAMERA_H
#define CAMERA_H

#include "room.hpp"
#include "collision.hpp"
#include <gmtl/gmtl.h>
#include <gmtl/Matrix.h>

class GameManager;
class Random;
class Object;

class Camera
{
    public:
        /// @brief Creates a new camera.
        /// @param gameManager The game manager.
        /// @param pos Camera starting position.
        /// @param width Camera view width.
        /// @param height Camera view height.
        /// @param maxDistance The max distance the camera can be from its follow object.
        Camera(GameManager *gameManager, gmtl::Vec2d pos, int width, int height, int maxDistance = 256);
        virtual ~Camera() = default;

        /// @brief Gets the drawing position of the camera.
        /// @return The drawing position. This is the position converted to integers.
        gmtl::Vec2i getDrawPos() const;

        /// @brief Gets the drawing position on the X axis.
        /// @return The X coordinate.
        int getDrawX() const;

        /// @brief Gets the drawing position on the Y axis.
        /// @return The Y coordinate.
        int getDrawY() const;

        /// @return The cameras width. 
        int getWidth() const;

        /// @return The cameras height. 
        int getHeight() const;

        /// @return The cameras bounding rectangle for drawing.
        CollisionRect getDrawRect() const;

        /// @brief Updates the cameras location.
        /// @param room The room the camera is moving in. Used to clamp the cameras location.
        /// @param deltaTime Time change in seconds since last frame.
        /// @param random The random number generator. For screenshake.
        void update(Room *room, double deltaTime, Random *random);

        void addScreenshake(int time);
        void setScreenshakeAmount(int amount);
        void clearScreenshake();

        void setDest(gmtl::Vec2d dest);
        void setFollowObject(Object *object);
        void setPosition(double x, double y);

    protected:
        gmtl::Vec2d pos;
        gmtl::Vec2d dest;

        Object *followObject;   //Game object to follow

    private:
        /// @brief Dispatches the EventCameraMoved event, updating listeners of the Cameras position.
        void dispatchCameraMovedEvent() const;

        /// @brief Moves the camera over time. Should be overridden to provide unique camera functionality.
        /// @param deltaTime The time that passed.
        /// @param destination The goal location.
        virtual void move(double deltaTime, gmtl::Vec2d destination) = 0;

        GameManager *gameManager;

        int width, height;

        int maxDistance;    //Max distance from target follow object

        int screenshake;        //Frames to screenshake for
        int screenshakeAmount;  //+- pixels to shake
        int shakeX, shakeY;
};

#endif