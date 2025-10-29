#include "texture.hpp"
#include <SDL3_image/SDL_image.h>

Texture::Texture()
{
    texture = NULL;
    width = 0;
    height = 0;
}

Texture::~Texture()
{
    freeTexture();
}

/// @brief Loads the given image file into a surface which is then loaded into a texture.
/// @param renderer The renderer the texture will be created for.
/// @param path Image file path.
/// @return True on success.
bool Texture::loadTexture(SDL_Renderer *renderer, std::string path)
{
    SDL_Surface *surface;
    
    //Free previous texture
    freeTexture();

    //Load the image into a surface
    surface = IMG_Load(path.c_str());

    if (surface == NULL)
    {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Texture: Unable to load image %s! Error: %s", path.c_str(), SDL_GetError());

        return false;
    }

    //Load the texture
    texture = SDL_CreateTextureFromSurface(renderer, surface);

    if (texture == NULL)
    {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Texture: Failed to create texture from image %s! Error: %s", path.c_str(), SDL_GetError());
    }
    else
    {
        width = surface->w;
        height = surface->h;
    }

    SDL_DestroySurface(surface);

    if (texture != NULL)
    {
        SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "Texture: Loaded texture: %s", path.c_str());
    }

    return texture != NULL;
}

/// @brief Frees the current texture.
void Texture::freeTexture()
{
    if (texture != NULL)
    {
        SDL_DestroyTexture(texture);

        texture = NULL;
        width = 0;
        height = 0;

        SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "Texture: Texture freed");
    }
}

/// @return True if a texture is loaded. False otherwise 
bool Texture::textureLoaded() const
{
    return texture != NULL;
}

/// @brief Renders the texture to the given renderer.
/// @param renderer The renderer to render to.
/// @param x X location on the viewport.
/// @param y Y location on the viewport.
/// @param xScale Width scale of texture destination.
/// @param yScale Height scale of texture destination.
/// @param clip Portion of the texture to render.
/// @param angle Image rotation angle.
/// @param center Center point to rotate the texture at. Defaults at w/2, h/2 of the dest rect (image center).
/// @param flip Whether the flip the texture horizontally or vertically.
void Texture::render(SDL_Renderer *renderer, int x, int y, double xScale, double yScale, SDL_FRect *clip, double angle, SDL_FPoint *center, SDL_FlipMode flip) const
{
    if (texture == NULL)
    {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Texture: Attempted to render NULL texture! Call loadTexture() to load a texture.");

        return;
    }

    SDL_FRect renderQuad = {x, y, width, height};

    if (clip != NULL)
    {
        renderQuad.w = clip->w;
        renderQuad.h = clip->h;
    }

    //Apply scale
    renderQuad.w *= xScale;
    renderQuad.h *= yScale;

    //Render the texture
    SDL_RenderTextureRotated(renderer, texture, clip, &renderQuad, angle, center, flip);
}

/// @brief Renders the texture to the given renderer.
/// @param renderer The renderer to render to.
/// @param x X location on the viewport.
/// @param y Y location on the viewport.
/// @param drawWidth Width to draw the texture at.
/// @param drawHeight Height to draw the texture at.
/// @param clip Portion of the texture to render.
/// @param angle Image rotation angle.
/// @param center Center point to rotate the texture at. Defaults at w/2, h/2 of the dest rect (image center).
/// @param flip Whether the flip the texture horizontally or vertically.
void Texture::renderStretched(SDL_Renderer *renderer, int x, int y, int drawWidth, int drawHeight, SDL_FRect *clip, double angle, SDL_FPoint *center, SDL_FlipMode flip) const
{
    if (texture == NULL)
    {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Texture: Attempted to render NULL texture! Call loadTexture() to load a texture.");

        return;
    }

    SDL_FRect renderQuad = {x, y, drawWidth, drawHeight};

    //Render the texture
    SDL_RenderTextureRotated(renderer, texture, clip, &renderQuad, angle, center, flip);
}

#pragma region Setters

/// @brief Set the color modulation (multiplier) for the texture.
/// @param r Red component (0 - 255).
/// @param g Green component (0 - 255).
/// @param b Blue component (0 - 255).
void Texture::setColorMod(Uint8 r, Uint8 g, Uint8 b)
{
    if (texture == NULL)
    {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Texture: Attempted to set color mod for NULL texture! Call loadTexture() to load a texture.");

        return;
    }

    SDL_SetTextureColorMod(texture, r, g, b);
}

/// @brief Sets the blend mode for the texture.
/// @param blending The blend mode.
void Texture::setBlendMode(SDL_BlendMode blending)
{
    if (texture == NULL)
    {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Texture: Attempted to set blend mode for NULL texture! Call loadTexture() to load a texture.");

        return;
    }

    SDL_SetTextureBlendMode(texture, blending);
}

/// @brief Sets the alpha for the texture.
/// @param alpha Alpha, from 0 to 255.
void Texture::setAlpha(Uint8 alpha)
{
    if (texture == NULL)
    {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Texture: Attempted to set alpha for NULL texture! Call loadTexture() to load a texture.");

        return;
    }

    SDL_SetTextureAlphaMod(texture, alpha);
}

#pragma endregion Setters

#pragma region Getters

/// @return The texture width. 
int Texture::getWidth() const
{
    return width;
}

/// @return The texture height. 
int Texture::getHeight() const
{
    return height;
}

#pragma endregion Getters