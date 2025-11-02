#pragma once

#ifndef OBJECT_H
#define OBJECT_H

#include "collision.hpp"
#include <gmtl/gmtl.h>
#include <gmtl/Matrix.h>
#include <string>

class GameManager;

/// @brief A basic object with transform and animation.
class Object
{
    public:
        /// @brief Creates a game object.
        /// @param name Object name.
        /// @param gameManager The objects game manager.
        /// @param x X spawn.
        /// @param y Y spawn.
        Object(std::string name, GameManager *gameManager, double x, double y);
        virtual ~Object() = default;

        /// @brief Marks the game object as destroyed and informs the object manager to delete it.
        virtual void destroy();

        /// @brief Updates the Object.
        /// @param deltaTime Time change in seconds since last frame.
        virtual void update(double deltaTime) = 0;

        /// @brief Updates the Object. Called before update and updateEnd.
        /// @param deltaTime Time change in seconds since last frame.
        virtual void updateBegin(double deltaTime) {};

        /// @brief Updates the Object. Called after updateBegin and update.
        /// @param deltaTime Time change in seconds since last frame.
        virtual void updateEnd(double deltaTime) {};

        /// @brief Sets the position of this object. This also moves the objects BBox.
        /// @param x X position.
        /// @param y Y position.
        virtual void setPosition(double x, double y);

        /// @brief Sets whether this object has been destroyed.
        void setDestroyed(bool destroyed);

        /// @return True if the object was destroyed. False otherwise.
        bool isDestroyed() const;

        /// @return True if this object can update while the game is paused. False otherwise.
        bool canUpdateWhilePaused();

        /// @return This game objects name. 
        std::string getName() const;

        /// @return This objects bounding box. 
        BBox getBBox() const;

        /// @return This objects unique identifier.
        unsigned int getId();
        
        //Transform
        gmtl::Vec2d pos;

    protected:
        friend class ObjectManager;

        /// @brief Sets this objects origin.
        /// @param x Origin X.
        /// @param y Origin Y.
        void setOrigin(int x, int y);

        /// @brief Sets this objects bounding box.
        /// @param type The type of collision.
        /// @param left The boxes left offset.
        /// @param top The boxes top offset.
        /// @param right The boxes right offset.
        /// @param bottom The boxes bottom offset.
        void setBBox(CollisionType type, int left, int top, int right, int bottom);

        /// @brief Updates the position of this Objects bounding box.
        virtual void updateBBoxPosition();

        GameManager *gameManager;

        bool destroyed;
        bool updateWhilePaused;
        BBox bBox;

        gmtl::Vec2i origin;     //X Y Origin offset
        gmtl::Vec2d scale;
        double angle;

    private:
        std::string name;   //The object name. A unique identifier.
        unsigned int id;
};

#endif