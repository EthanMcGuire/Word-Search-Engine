#pragma once

#ifndef TEXTURE_H
#define TEXTURE_H

#include <SDL3/SDL_render.h>
#include <string>

class Texture
{
    public:
        Texture();
        ~Texture();

        /// @brief Loads the given image file into a surface which is then loaded into a texture.
        /// @param renderer The renderer the texture will be created for.
        /// @param path Image file path.
        /// @return True on success.
        bool loadTexture(SDL_Renderer *renderer, std::string path);

        /// @brief Frees the current texture.
        void freeTexture();

        /// @return True if a texture is loaded. False otherwise 
        bool textureLoaded() const;

        /// @brief Renders the texture to the given renderer.
        /// @param renderer The renderer to render to.
        /// @param x X location on the viewport.
        /// @param y Y location on the viewport.
        /// @param xScale Width scale of texture destination.
        /// @param yScale Height scale of texture destination.
        /// @param clip Portion of the texture to render.
        /// @param angle Image rotation angle.
        /// @param center Center point to rotate the texture at. Defaults at w/2, h/2 of the dest rect (image center).
        /// @param flip Whether to flip the texture horizontally or vertically.
        void render(SDL_Renderer *renderer, int x, int y, double xScale = 1.0, double yScale = 1.0, SDL_FRect *clip = NULL, double angle = 0.0, SDL_FPoint *center = NULL, SDL_FlipMode flip = SDL_FLIP_NONE) const;
	
        /// @brief Renders the texture to the given renderer, centering it.
        /// @param renderer The renderer to render to.
        /// @param x X location on the viewport.
        /// @param y Y location on the viewport.
        /// @param xScale Width scale of texture destination.
        /// @param yScale Height scale of texture destination.
        /// @param clip Portion of the texture to render.
        /// @param angle Image rotation angle.
        /// @param center Center point to rotate the texture at. Defaults at w/2, h/2 of the dest rect (image center).
        /// @param flip Whether to flip the texture horizontally or vertically.
void renderCentered(SDL_Renderer *renderer, int x, int y, double xScale = 1.0, double yScale = 1.0, SDL_FRect *clip = NULL, double angle = 0.0, SDL_FPoint *center = NULL, SDL_FlipMode flip = SDL_FLIP_NONE) const;

        /// @brief Renders the texture to the given renderer.
        /// @param renderer The renderer to render to.
        /// @param x X location on the viewport.
        /// @param y Y location on the viewport.
        /// @param drawWidth Width to draw the texture at.
        /// @param drawHeight Height to draw the texture at.
        /// @param clip Portion of the texture to render.
        /// @param angle Image rotation angle.
        /// @param center Center point to rotate the texture at. Defaults at w/2, h/2 of the dest rect (image center).
        /// @param flip Whether to flip the texture horizontally or vertically.
        void renderStretched(SDL_Renderer *renderer, int x, int y, int drawWidth, int drawHeight, SDL_FRect *clip = NULL, double angle = 0.0, SDL_FPoint *center = NULL, SDL_FlipMode flip = SDL_FLIP_NONE) const;

        #pragma region Setters

        /// @brief Set the color modulation (multiplier) for the texture.
        /// @param r Red component (0 - 255).
        /// @param g Green component (0 - 255).
        /// @param b Blue component (0 - 255).
        void setColorMod(Uint8 r, Uint8 g, Uint8 b);

        /// @brief Sets the blend mode for the texture.
        /// @param blending The blend mode.
        void setBlendMode(SDL_BlendMode blending);

        /// @brief Sets the alpha for the texture.
        /// @param alpha Alpha, from 0 to 255.
        void setAlpha(Uint8 alpha);

        #pragma endregion Setters

        #pragma region Getters

        /// @return The texture width. 
        int getWidth() const;

        /// @return The texture height. 
        int getHeight() const;

        #pragma endregion Getters

    protected:
        SDL_Texture *texture;
        int width, height;
};

#endif
