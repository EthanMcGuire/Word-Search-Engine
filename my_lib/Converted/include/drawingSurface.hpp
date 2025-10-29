#pragma once

#ifndef DRAWING_SURFACE_H
#define DRAWING_SURFACE_H

#include <SDL3/SDL_render.h>

class DrawingSurface
{
    public:
        /// @brief Creates a drawing surface to draw on.
        DrawingSurface();
        ~DrawingSurface();

        /// @brief Creates the drawing surface. Can be called again to resize the surface.
        /// @param renderer The renderer to create the texture from.
        /// @param w Surface width.
        /// @param h Surface height.
        /// @return True on success, false otherwise.
        bool createSurface(SDL_Renderer *renderer, unsigned int w, unsigned int h);

        /// @brief Checks if the surface texture exists.
        /// @return True if the surface texture was created.
        bool surfaceExists() const;

        /// @brief Sets the rendering target to this drawing surface.
        /// @param renderer The renderer to set the target for.
        void targetSurface(SDL_Renderer *renderer) const;

        /// @brief Draws the surface.
        /// @param renderer The renderer to draw to.
        /// @param x X draw position.
        /// @param y Y draw position.
        /// @param xScale The X scale to draw the surface at.
        /// @param yScale The Y scale to draw the surface at.
        /// @param clip Portion of the texture to render. NULL to render the full texture.
        /// @param angle Image rotation angle.
        /// @param center Center point to rotate the texture at. Defaults at w/2, h/2 of the dest rect (image center).
        /// @param flip Whether to flip the texture horizontally or vertically.
        void drawSurface(SDL_Renderer *renderer, int x, int y, double xScale = 1.0, double yScale = 1.0, SDL_FRect *clip = NULL, double angle = 0.0, SDL_FPoint *center = NULL, SDL_FlipMode flip = SDL_FLIP_NONE) const;
        
        /// @brief Draws the surface.
        /// @param renderer The renderer to draw to.
        /// @param x X draw position.
        /// @param y Y draw position.
        /// @param drawWidth Width to draw the texture at.
        /// @param drawHeight Height to draw the texture at.
        /// @param clip Portion of the texture to render. NULL to render the full texture.
        /// @param angle Image rotation angle.
        /// @param center Center point to rotate the texture at. Defaults at w/2, h/2 of the dest rect (image center).
        /// @param flip Whether to flip the texture horizontally or vertically.
        void drawSurfaceStretched(SDL_Renderer *renderer, int x, int y, int drawWidth, int drawHeight, SDL_FRect *clip = NULL, double angle = 0.0, SDL_FPoint *center = NULL, SDL_FlipMode flip = SDL_FLIP_NONE) const;

        /// @brief Set the color modulation (multiplier) for the surface.
        /// @param r Red component (0 - 255).
        /// @param g Green component (0 - 255).
        /// @param b Blue component (0 - 255).
        void setColorMod(Uint8 r, Uint8 g, Uint8 b);
        
        /// @brief Sets the blend mode for the surface.
        /// @param blending The blend mode.
        void setBlendMode(SDL_BlendMode blending);
        
        /// @brief Sets the alpha for the surface.
        /// @param alpha Alpha, from 0 to 255.
        void setAlpha(Uint8 alpha);

        /// @return The surface width in pixels.
        unsigned int getWidth() const;

        /// @return The surface height in pixels.
        unsigned int getHeight() const;

    private:
        /// @brief Frees the drawing surface from memory.
        void freeSurface();

        SDL_Texture *texture;
        unsigned int width;
        unsigned int height;
};

#endif