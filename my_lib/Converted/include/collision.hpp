#pragma once

#ifndef COLLISION_H
#define COLLISION_H

struct CollisionRect
{
    double x, y;    //Top left location
    int w;
    int h;
};

struct CollisionCirc
{
    double x, y;    //Center location
    double r;
};

enum CollisionType
{
    NONE,
    RECTANGLE,
    CIRCLE
};

/// @brief A bounding box used for collision checks.
class BBox
{
    public:
        BBox();
        BBox(CollisionType type);
        BBox(CollisionType type, int left, int top, int right, int bottom);

        /// @brief Sets the collisions bounding box.
        /// @param left Left X offset.
        /// @param top Top Y offset.
        /// @param right Right X offset.
        /// @param bottom Bottom Y offset.
        void setBBox(int left, int top, int right, int bottom);

        /// @brief Sets the collisions bounding box. Does not copy the CollisionType from the bBox.
        /// @param bBox The bounding box.
        void setBBox(BBox bBox);

        /// @brief Sets the bounding boxes collision type.
        /// @param type The collision type.
        void setCollisionType(CollisionType type);

        /// @brief Gets the bounding boxes collision type.
        /// @return The collision type.
        CollisionType getCollisionType();

        /// @brief Checks this BBoxes collision with another at the specified location.
        /// @param x The x position to check at.
        /// @param y The y position to check at.
        /// @param other The other BBox.
        /// @return True if a collision occured at the location, false otherwise.
        bool checkCollision(double x, double y, BBox other);

        /// @brief Sets the bounding boxes location. The given X and Y should be the top left of the BBox.
        /// @param x X position to set.
        /// @param y Y position to set.
        void setLocation(double x, double y);

        /// @brief Creates a collision rectangle from this bounding box.
        /// @param box The bounding box.
        /// @return A collision rectangle.
        CollisionRect getRect();

        /// @brief Creates a collision circle from this bounding box.
        /// @param box The bounding box.
        /// @return A collision circle.
        CollisionCirc getCirc();

        /// @brief Checks if the bounding box is valid.
        /// @return True if valid, false otherwise.
        bool isValid();

        double getBBoxLeft();
        double getBBoxTop();
        double getBBoxRight();
        double getBBoxBottom();
        int getBBoxWidth();
        int getBBoxHeight();

    private:
        CollisionType type = CollisionType::NONE;   //The type of collision to be used

        //Bounding box bounds
        int left;
        int top;
        int right;
        int bottom;

        double x, y;    //The bounding boxes top left location
                        //IE: To get the current left bounding position, do x + left
        
        int w, h;       //The bounding boxes width and height
};

/// @brief Stores various methods of collision checking.
namespace Collision
{
    /// @brief Checks if two bounding boxes are meeting. This is the main method of collision that should be used, as it will work for any combined collision types.
    /// @param a The first collision source.
    /// @param b The second collision source.
    /// @return True if a collision occured, false otherwise.
    bool collisionMeeting(BBox a, BBox b);

    /// @brief Checks if 2 rectangles are colliding.
    /// @param a The first rectangle.
    /// @param b The second rectangle.
    /// @return True on collision. False otherwise.
    bool checkCollisionRects(CollisionRect a, CollisionRect b);

    /// @brief Checks if 2 rectangles are colliding.
    /// @param a The first rectangles BBox.
    /// @param b The second rectangles BBox.
    /// @return True on collision. False otherwise.
    bool checkCollisionRects(BBox a, BBox b);

    /// @brief Checks if 2 circles are colliding.
    /// @param a The first circle.
    /// @param b The second circle.
    /// @return True on collision. False otherwise.
    bool checkCollisionCircs(CollisionCirc a, CollisionCirc b);

    /// @brief Checks if 2 circles are colliding.
    /// @param a The first circles BBox.
    /// @param b The second circles BBox.
    /// @return True on collision. False otherwise.
    bool checkCollisionCircs(BBox a, BBox b);

    /// @brief Checks if a circle is colliding with a rectangle.
    /// @param a The circle.
    /// @param b The rectangle.
    /// @return True on collision. False otherwise.
    bool checkCollisionCircRect(CollisionCirc a, CollisionRect b);

    /// @brief Checks if a rectangle is colliding with a circle.
    /// @param a The rectangle.
    /// @param b The circle.
    /// @return True on collision. False otherwise.
    bool checkCollisionRectCirc(CollisionRect a, CollisionCirc b);

    /// @brief Creates a bounding box from a collision rectangle.
    /// @param rect The collision rectangle.
    /// @return A bounding box describing the rectangular boundary.
    BBox getBBoxFromRect(CollisionRect rect);

    /// @brief Checks if the given collision is valid.
    /// @param box The bounding box to check.
    /// @return True if the collision is valid (Can have space). False otherwise (Is of impossible size).
    bool collisionIsValid(BBox box);

    /// @brief Checks if the given collision is valid.
    /// @param rect The collision rectangle to check.
    /// @return True if the collision is valid (Can have space). False otherwise (Is of impossible size).
    bool collisionIsValid(CollisionRect rect);

    /// @brief Checks if the given collision is valid.
    /// @param rect The collision circle to check.
    /// @return True if the collision is valid (Can have space). False otherwise (Is of impossible size).
    bool collisionIsValid(CollisionCirc circ);
    
    /// @brief Checks if the given point is in the bounding box
    /// @param x Point x position.
    /// @param y Point y position.
    /// @param bBox The bounding box to check.
    /// @return True if the point is in the collision, false otherwise.
    bool pointInBBox(double x, double y, BBox bBox);

    /// @brief Checks if the given point is in the rectangle.
    /// @param x Point x position.
    /// @param y Point y position.
    /// @param rect The rectangular region to check.
    /// @return True if the point is in the rectangle, false otherwise.
    bool pointInRectangle(double x, double y, CollisionRect rect);

    /// @brief Checks if the given point is in the rectangle.
    /// @param x Point x position.
    /// @param y Point y position.
    /// @param rX The rectangles top left x position.
    /// @param rY The rectangles top left y position.
    /// @param rW The rectangles width.
    /// @param rH The rectangles height.
    /// @return True if the point is in the rectangle, false otherwise.
    bool pointInRectangle(double x, double y, double rX, double rY, int rW, int rH);

    /// @brief Checks if the given point is in the cicle.
    /// @param x Point x position.
    /// @param y Point y position.
    /// @param circ The circular region to check.
    /// @return True if the point is in the circle, false otherwise.
    bool pointInCircle(double x, double y, CollisionCirc circ);

    /// @brief Checks if the given point is in the cicle.
    /// @param x Point x position.
    /// @param y Point y position.
    /// @param cX The circles center X position.
    /// @param cY The circles center Y position.
    /// @param r The circles radius.
    /// @return True if the point is in the circle, false otherwise.
    bool pointInCircle(double x, double y, double cX, double cY, int r);
}

#endif