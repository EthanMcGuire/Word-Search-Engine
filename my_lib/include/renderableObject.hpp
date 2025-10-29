#pragma once

#ifndef RENDERABLE_OBJECT_H
#define RENDERABLE_OBJECT_H

#include "object.hpp"
#include "event.hpp"

class SDL_Renderer;
class Camera;

/// @brief A object with rendering and animation capability. Can be updated and rendered to the screen.
class RenderableObject : public Object
{
    public:
        RenderableObject(std::string name, GameManager *gameManager, double x, double y);
        virtual ~RenderableObject();

        /// @brief Updates the Object.
        /// @param deltaTime Time change in seconds since last frame.
        virtual void update(double deltaTime) override {};

        #pragma region Rendering

        /// @brief Renders the object.
        /// @param renderer The renderer to draw to.
        /// @param camera The game camera.
        virtual void render(SDL_Renderer *renderer, Camera *camera) {};

        /// @brief Renders the object. Called before render and renderEnd.
        /// @param renderer The renderer to draw to.
        /// @param camera The game camera.
        virtual void renderBegin(SDL_Renderer *renderer, Camera *camera) {};

        /// @brief Renders the object. Called after renderBegin and render. By default draws this objects collision.
        /// @param renderer The renderer to draw to.
        /// @param camera The game camera.
        virtual void renderEnd(SDL_Renderer *renderer, Camera *camera);

        /// @brief Renders this game objects GUI.
        /// @param renderer The renderer to draw to.
        virtual void renderGui(SDL_Renderer *renderer) {};

        /// @brief Renders this game objects GUI. Called before renderGui and renderGuiEnd.
        /// @param renderer The renderer to draw to.
        virtual void renderGuiBegin(SDL_Renderer *renderer) {};

        /// @brief Renders this game objects GUI. Called after renderGuiBegin and renderGui. By default draws this objects rendering collision.
        /// @param renderer The renderer to draw to.
        virtual void renderGuiEnd(SDL_Renderer *renderer);

        #pragma endregion Rendering

        /// @brief Toggles this game objects collision display.
        void toggleCollisionDisplay();

        /// @brief Sets this objects draw depth.
        /// @param drawDepth The draw depth. The lower the value, the more objects it will be on top of. 
        virtual void setDrawDepth(int drawDepth); 

        /// @brief Used to get the draw depth. Used in the ObjectManager when determining draw order.
        /// @return This objects draw depth. 
        int getDrawDepth() const; 

        /// @brief Sets whether this object is visible.
        /// @param isVisible True if the object should be visible.
        virtual void setVisible(bool isVisible);

        /// @return True if this object is visible.
        bool getVisible() const;

        /// @brief Sets whether this object is should run their render event.
        /// @param enabled Whether the render event should be enabled.
        void setRenderEnabled(bool enabled);

        /// @return True if this objects render event is enabled.
        bool getRenderEnabled();

        /// @brief Sets whether this object is should run their renderGui event.
        /// @param enabled Whether the renderGui event should be enabled.
        virtual void setRenderGuiEnabled(bool enabled);     //This needs to be virtual so WidgetContainer can override it

        /// @return True if this objects renderGui event is enabled.
        bool getRenderGuiEnabled();

        /// @return This objects rendering bounding box. 
        BBox getRenderBBox() const;

    protected:
        /// @brief Sets this objects rendering bounding box.
        /// @param type The type of collision.
        /// @param left The boxes left offset.
        /// @param top The boxes top offset.
        /// @param right The boxes right offset.
        /// @param bottom The boxes bottom offset.
        void setRenderBBox(CollisionType type, int left, int top, int right, int bottom);

        /// @brief Updates the position of this Objects bounding box.
        virtual void updateBBoxPosition() override;

        int depth;              //Draw depth
        bool visible;
        bool renderEnabled;
        bool renderGuiEnabled;
        bool showCollision;     //This shows the Objects collisionInfo, not the renderCollisionInfo

        BBox renderBBox;  //Collision for rendering size. Used for GUI stuff like selecting the object with the mouse cursor  

    private:
        /// @brief Handles the EventCameraMoved event.
        /// @param event The event info.
        void cameraMovedCallback(EventCameraMoved *event);

        uint32_t cameraMovedEventListenerId;
};

#endif