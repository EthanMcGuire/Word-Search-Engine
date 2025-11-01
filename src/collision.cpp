#include "collision.hpp"
#include <iostream>

double distanceSquared(double x1, double y1, double x2, double y2);

BBox::BBox()
{
    setCollisionType(CollisionType::NONE);
}

BBox::BBox(CollisionType type)
{
    setBBox(0, 0, 0, 0);
    setCollisionType(type);
}

BBox::BBox(CollisionType type, int left, int top, int right, int bottom)
{
    setBBox(left, top, right, bottom);
    setCollisionType(type);
}

/// @brief Sets the collisions bounding box.
/// @param left Left X offset.
/// @param top Top Y offset.
/// @param right Right X offset.
/// @param bottom Bottom Y offset.
void BBox::setBBox(int left, int top, int right, int bottom)
{
    this->left = left;
    this->top = top;
    this->right = right;
    this->bottom = bottom;

    //The right and bottom should always be >= their counterparts
    if (right < left) this->right = this->left;
    if (bottom < top) this->bottom = this->top;

    this->w = this->right - this->left + 1;    //0 to 31 is 32 pixels wide, but 31 - 0 = 31. Thus we need the +1
    this->h = this->bottom - this->top + 1;
}

/// @brief Sets the collisions bounding box. Does not copy the CollisionType from the bBox.
/// @param bBox The bounding box.
void BBox::setBBox(BBox bBox)
{
    setBBox(bBox.left, bBox.top, bBox.right, bBox.bottom);
}

/// @brief Sets the bounding boxes collision type.
/// @param type The collision type.
void BBox::setCollisionType(CollisionType type)
{
    this->type = type;
}

/// @brief Gets the bounding boxes collision type.
/// @return The collision type.
CollisionType BBox::getCollisionType()
{
    return type;
}

/// @brief Checks this BBoxes collision with another at the specified location.
/// @param x The x position to check at.
/// @param y The y position to check at.
/// @param other The other BBox.
/// @return True if a collision occured at the location, false otherwise.
bool BBox::checkCollision(double x, double y, BBox other)
{
    BBox currentBBox(type);
    double currentLeft, currentTop, currentRight, currentBottom;

    //Get the bounding box for the given location
    currentLeft = x + (double) left;
    currentTop = y + (double) top;
    currentRight = x + (double) right;
    currentBottom = y + (double) bottom;

    currentBBox.setBBox(currentLeft, currentTop, currentRight, currentBottom);

    return Collision::collisionMeeting(currentBBox, other);
}

/// @brief Sets the bounding boxes location. The given X and Y should be the top left of the BBox.
/// @param x X position to set.
/// @param y Y position to set.
void BBox::setLocation(double x, double y)
{
    this->x = x;
    this->y = y;
}

/// @brief Creates a collision rectangle from this bounding box.
/// @param box The bounding box.
/// @return A collision rectangle.
CollisionRect BBox::getRect()
{
    CollisionRect rect;

    rect.x = getBBoxLeft();
    rect.y = getBBoxTop();
    rect.w = w;
    rect.h = h;

    return rect;
}

/// @brief Creates a collision circle from this bounding box.
/// @param box The bounding box.
/// @return A collision circle.
CollisionCirc BBox::getCirc()
{
    CollisionCirc circ;

    circ.x = x + left + w / 2 - 1;
    circ.y = y + top + h / 2 - 1;
    circ.r = std::min(w / 2.0, h / 2.0);

    return circ;
}

/// @brief Checks if the bounding box is valid.
/// @return True if valid, false otherwise.
bool BBox::isValid()
{
    //left == right and top == bottom are both valid because they have a width or height of 1

    if (right < left)
    {
        return false;
    }

    if (bottom < top)
    {
        return false;
    }

    return true;
}

double BBox::getBBoxLeft()
{
    return x + (double) left;
}

double BBox::getBBoxTop()
{
    return y + (double) top;
}

double BBox::getBBoxRight()
{
    return x + (double) right;
}

double BBox::getBBoxBottom()
{
    return y + (double) bottom;
}

int BBox::getBBoxWidth()
{
    return w;
}

int BBox::getBBoxHeight()
{
    return h;
}

/// @brief Checks if two bounding boxes are meeting. This is the main method of collision that should be used, as it will work for any combined collision types.
/// @param a The first collision source.
/// @param b The second collision source.
/// @return True if a collision occured, false otherwise.
bool Collision::collisionMeeting(BBox a, BBox b)
{
    if (a.getCollisionType() == CollisionType::NONE || b.getCollisionType() == CollisionType::NONE)
    {
        return false;
    }

    if (!a.isValid() || !b.isValid())
    {
        return false;
    }

    //Check collision type

    //Rect on rect?
    if (a.getCollisionType() == CollisionType::RECTANGLE && b.getCollisionType() == CollisionType::RECTANGLE)
    {
        return checkCollisionRects(a, b);
    }

    //Rect on circ?
    else if (a.getCollisionType() == CollisionType::RECTANGLE && b.getCollisionType() == CollisionType::CIRCLE)
    {
        return checkCollisionRectCirc(a.getRect(), b.getCirc());
    }

    //Circ on rect?
    else if (a.getCollisionType() == CollisionType::CIRCLE && b.getCollisionType() == CollisionType::RECTANGLE)
    {
        return checkCollisionCircRect(a.getCirc(), b.getRect());
    }

    //Circ on circ?
    else if (a.getCollisionType() == CollisionType::CIRCLE && b.getCollisionType() == CollisionType::CIRCLE)
    {
        return checkCollisionCircs(a, b);
    }

    return false;
}

/// @brief Checks if 2 rectangles are colliding.
/// @param a The first rectangle.
/// @param b The second rectangle.
/// @return True on collision. False otherwise.
bool Collision::checkCollisionRects(CollisionRect a, CollisionRect b)
{
    return checkCollisionRects(getBBoxFromRect(a), getBBoxFromRect(b));
}

/// @brief Checks if 2 rectangles are colliding.
/// @param a The first rectangles BBox.
/// @param b The second rectangles BBox.
/// @return True on collision. False otherwise.
bool Collision::checkCollisionRects(BBox a, BBox b)
{
    if (!a.isValid() || !b.isValid())
    {
        return false;
    }

    //Using the separating axis collision check
    if (a.getBBoxBottom() < b.getBBoxTop())
    {
        return false;
    }

    if (a.getBBoxTop() > b.getBBoxBottom())
    {
        return false;
    }

    if (a.getBBoxRight() < b.getBBoxLeft())
    {
        return false;
    }

    if (a.getBBoxLeft() > b.getBBoxRight())
    {
        return false;
    }

    //X and y axes are both intersecting
    return true;
}

/// @brief Checks if 2 circles are colliding.
/// @param a The first circle.
/// @param b The second circle.
/// @return True on collision. False otherwise.
bool Collision::checkCollisionCircs(CollisionCirc a, CollisionCirc b)
{
    if (!collisionIsValid(a) || !collisionIsValid(b))
    {
        return false;
    }

    double radiusSquared = a.r + b.r;
    radiusSquared *= radiusSquared;

    if (distanceSquared(a.x, a.y, b.x, b.y) < radiusSquared)
    {
        return true;
    }

    return false;
}

/// @brief Checks if 2 circles are colliding.
/// @param a The first circles BBox.
/// @param b The second circles BBox.
/// @return True on collision. False otherwise.
bool Collision::checkCollisionCircs(BBox a, BBox b)
{
    return checkCollisionCircs(a.getCirc(), b.getCirc());
}

/// @brief Checks if a circle is colliding with a rectangle.
/// @param a The circle.
/// @param b The rectangle.
/// @return True on collision. False otherwise.
bool Collision::checkCollisionCircRect(CollisionCirc a, CollisionRect b)
{
    if (!collisionIsValid(a) || !collisionIsValid(b))
    {
        return false;
    }

    //Find the closest point on the collision box   
    double closestX, closestY;

    //Find the closest x
    if (a.x < b.x)
    {
        closestX = b.x;
    }
    else if (a.x > b.x + b.w)
    {
        closestX = b.x + b.w;
    }
    else
    {
        closestX = a.x;
    }

    //Find the closest y
    if (a.y < b.y)
    {
        closestY = b.y;
    }
    else if (a.y > b.y + b.h)
    {
        closestY = b.y + b.h;
    }
    else
    {
        closestY = a.y;
    }

    return pointInCircle(closestX, closestY, a);
}

/// @brief Checks if a rectangle is colliding with a circle.
/// @param a The rectangle.
/// @param b The circle.
/// @return True on collision. False otherwise.
bool Collision::checkCollisionRectCirc(CollisionRect a, CollisionCirc b)
{
    return checkCollisionCircRect(b, a);
}

/// @brief Creates a bounding box from a collision rectangle.
/// @param rect The collision rectangle.
/// @return A bounding box describing the rectangular boundary.
BBox Collision::getBBoxFromRect(CollisionRect rect)
{
    BBox box;

    box.setLocation(rect.x, rect.y);
    box.setBBox(0, 0, rect.w - 1, rect.h - 1);

    return box;
}

/// @brief Checks if the given collision is valid.
/// @param rect The collision rectangle to check.
/// @return True if the collision is valid (Can have space). False otherwise (Is of impossible size).
bool Collision::collisionIsValid(CollisionRect rect)
{
    if (rect.w <= 0 || rect.h <= 0)
    {
        return false;
    }

    return true;
}

/// @brief Checks if the given collision is valid.
/// @param circ The collision circle to check.
/// @return True if the collision is valid (Can have space). False otherwise (Is of impossible size).
bool Collision::collisionIsValid(CollisionCirc circ)
{
    if (circ.r <= 0)
    {
        return false;
    }

    return true;
}

/// @brief Checks if the given point is in the bounding box
/// @param x Point x position.
/// @param y Point y position.
/// @param bBox The bounding box to check.
/// @return True if the point is in the collision, false otherwise.
bool Collision::pointInBBox(double x, double y, BBox bBox)
{
    if (!bBox.isValid())
    {
        return false;
    }

    if (bBox.getCollisionType() == CollisionType::NONE)
    {
        return false;
    }
    else if (bBox.getCollisionType() == CollisionType::RECTANGLE)
    {
        return pointInRectangle(x, y, bBox.getRect());
    }
    else if (bBox.getCollisionType() == CollisionType::CIRCLE)
    {
        return pointInCircle(x, y, bBox.getCirc());
    }

    return false;
}

/// @brief Checks if the given point is in the rectangle.
/// @param x Point x position.
/// @param y Point y position.
/// @param rect The rectangular region to check.
/// @return True if the point is in the rectangle, false otherwise.
bool Collision::pointInRectangle(double x, double y, CollisionRect rect)
{
    if (!collisionIsValid(rect))
    {
        return false;
    }

    if (x >= rect.x && y >= rect.y && x < rect.x + rect.w && y < rect.y + rect.h)
    {
        return true;
    }

    return false;
}

/// @brief Checks if the given point is in the rectangle.
/// @param x Point x position.
/// @param y Point y position.
/// @param rX The rectangles top left x position.
/// @param rY The rectangles top left y position.
/// @param rW The rectangles width.
/// @param rH The rectangles height.
/// @return True if the point is in the rectangle, false otherwise.
bool Collision::pointInRectangle(double x, double y, double rX, double rY, int rW, int rH)
{
    CollisionRect rect = {rX, rY, rW, rH};

    return pointInRectangle(x, y, rect);
}

/// @brief Checks if the given point is in the cicle.
/// @param x Point x position.
/// @param y Point y position.
/// @param circ The circular region to check.
/// @return True if the point is in the circle, false otherwise.
bool Collision::pointInCircle(double x, double y, CollisionCirc circ)
{
    if (!collisionIsValid(circ))
    {
        return false;
    }

    double radiusSquared = circ.r * circ.r;

    if (distanceSquared(x, y, circ.x, circ.y) < radiusSquared)
    {
        return true;
    }

    return false;
}

/// @brief Checks if the given point is in the cicle.
/// @param x Point x position.
/// @param y Point y position.
/// @param cX The circles center X position.
/// @param cY The circles center Y position.
/// @param r The circles radius.
/// @return True if the point is in the circle, false otherwise.
bool Collision::pointInCircle(double x, double y, double cX, double cY, int r)
{
    CollisionCirc circ;

    circ.x = cX;
    circ.y = cY;
    circ.r = r;

    return pointInCircle(x, y, circ);
}

double distanceSquared(double x1, double y1, double x2, double y2)
{
    return (x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1);
}