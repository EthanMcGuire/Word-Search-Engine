#include "collisionDrawer.hpp"
#include "camera.hpp"

/// @brief Draws a collision debug.
/// @param renderer The renderer to draw to.
/// @param camera The game camera. Set to null if no camera is being used (GUI).
/// @param bBox Collision to draw.
/// @param color The color of the collision.
void CollisionDrawer::drawCollision(SDL_Renderer* renderer, Camera *camera, BBox bBox, SDL_Color color)
{
    if (bBox.getCollisionType() == CollisionType::NONE)
    {
        return;
    }

    double cameraX = 0;
    double cameraY = 0;

    //Camera is being used?
    if (camera != nullptr)
    {
        cameraX = camera->getDrawX();
        cameraY = camera->getDrawY();
    }

    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);

    if (bBox.getCollisionType() == CollisionType::RECTANGLE)
    {
        SDL_FRect rect = {(float) floor(bBox.getBBoxLeft() - cameraX), (float) floor(bBox.getBBoxTop() - cameraY), (float) bBox.getBBoxWidth(), (float) bBox.getBBoxHeight()};

        SDL_RenderFillRect(renderer, &rect);
    }
    else if (bBox.getCollisionType() == CollisionType::CIRCLE)
    {
        //TODO
        //
    }
    
    SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);   //Reset to white
}