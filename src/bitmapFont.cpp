#include "bitmapFont.hpp"
#include "random.hpp"
#include "rapidxml/rapidxml_utils.hpp"
#include <SDL3/SDL_log.h>
#include <fstream>

using namespace rapidxml;

/// @brief Creates a bitmap font class.
/// @param random The engines random number generator. Used for character shake.
BitmapFont::BitmapFont(Random *random)
{
    rng = random;

    fontLoaded = false;

    drawColor = {255, 255, 255};

    shakeIntensity = 1;
    shake = false;  
    packed = false;

    currentSep = 0;
    lineHeight = 0;
}

BitmapFont::~BitmapFont()
{
    freeFont();

    SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "BitmapFont: Bitmap Font freed");
}

/// @brief Loads the font. This is a wrapper for the loadFontFromXml() method.
/// @param renderer The render to load the texture for.
/// @param path The path to the font configuration file.
/// @return True on success, false otherwise.
bool BitmapFont::loadFont(SDL_Renderer *renderer, std::string path)
{
    bool success = false;

    freeFont();

    //Load the XML configuration
    try
    {
        success = loadFontFromXml(renderer, path);
    }
    catch(const std::exception& e)
    {
        success = false;

        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "BitmapFont: Failed to load bitmap font %s! Exception occured: %s", path.c_str(), e.what());
    }
    
    //Free font data on failure
    if (!success)
    {
        freeFont();

        return false;
    }

    SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "BitmapFont: Loaded font %s with %d characters!", fontName.c_str(), charCount);

    fontLoaded = true;

    return true;
}

/// @brief Loads the font from the XML file.
/// @param renderer The render to load the texture for.
/// @param path The path to the font configuration file.
/// @return True on success, false otherwise.
bool BitmapFont::loadFontFromXml(SDL_Renderer *renderer, std::string path)
{
    file<> xmlFile(path.c_str());
    xml_document<> doc;
    doc.parse<0>(xmlFile.data());

    xml_node<> *rootNode = doc.first_node("font");

    xml_node<> *info = rootNode->first_node();
    xml_node<> *common = info->next_sibling();
    xml_node<> *pages = common->next_sibling();
    xml_node<> *charsNode = pages->next_sibling();

    //Get the font creation info
    fontName = getAttrValue(info, "face");

    //Get the font common info
    lineHeight = std::stoi(getAttrValue(common, "lineHeight"));
    base = std::stoi(getAttrValue(common, "base"));
    scaleW = std::stoi(getAttrValue(common, "scaleW"));
    scaleH = std::stoi(getAttrValue(common, "scaleH"));
    pageCount = std::stoi(getAttrValue(common, "pages"));
    packed = std::stoi(getAttrValue(common, "packed"));
    alphaChnl = std::stoi(getAttrValue(common, "alphaChnl"));
    redChnl = std::stoi(getAttrValue(common, "redChnl"));
    greenChnl = std::stoi(getAttrValue(common, "greenChnl"));
    blueChnl = std::stoi(getAttrValue(common, "blueChnl"));

    //Get the font folder path
    std::string basePath;
    int fileStartIndex;

    fileStartIndex = path.find_last_of('/');
    basePath = path.substr(0, fileStartIndex + 1);

    //Load the texture pages
    xml_node<> *page = pages->first_node();

    for (unsigned int i = 0; i < pageCount; i++)
    {
        std::string texturePath;
        Texture *texture;

        texturePath = basePath + page->first_attribute("file")->value();

        texture = new Texture();

        //Load the font texture page
        if (!texture->loadTexture(renderer, texturePath))    
        {
            SDL_LogError(SDL_LOG_CATEGORY_ERROR, "TextDrawer: Failed to load texture page %d.", i);

            delete texture;

            return false;
        }

        //Add the texture page
        texturePages.push_back(texture);

        page = page->next_sibling();
    }

    //Load the characters
    charCount = std::stoi(charsNode->first_attribute("count")->value());

    xml_node<> *nextChar = charsNode->first_node();

    for (unsigned int i = 0; i < charCount; i++)
    {
        CharInfo charInfo;

        //Get the character info
        charInfo.id = std::stoi(getAttrValue(nextChar, "id"));
        charInfo.x = std::stoi(getAttrValue(nextChar, "x"));
        charInfo.y = std::stoi(getAttrValue(nextChar, "y"));
        charInfo.width = std::stoi(getAttrValue(nextChar, "width"));
        charInfo.height = std::stoi(getAttrValue(nextChar, "height"));
        charInfo.xoffset = std::stoi(getAttrValue(nextChar, "xoffset"));
        charInfo.yoffset = std::stoi(getAttrValue(nextChar, "yoffset"));
        charInfo.xadvance = std::stoi(getAttrValue(nextChar, "xadvance"));
        charInfo.page = std::stoi(getAttrValue(nextChar, "page"));
        charInfo.channel = std::stoi(getAttrValue(nextChar, "chnl"));

        chars[charInfo.id] = charInfo;

        //Add the character
        nextChar = nextChar->next_sibling();
    }

    return true;
}

/// @brief Reads the attributes string value if it exists. 
/// @param node The node to get an attribute for.
/// @param attrName The attribute name.
/// @return The attribute value, or an empty string if it doesn't exist.
std::string BitmapFont::getAttrValue(xml_node<> *node, std::string attrName)
{
    xml_attribute<> *attr = node->first_attribute(attrName.c_str());

    if (attr)
    {
        return attr->value();
    }

    return "";
}

/// @brief Frees the current font texture and resets the configuration.
void BitmapFont::freeFont()
{
    if (!fontLoaded)
    {
        return;
    }

    SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "BitmapFont: Deleting font %s...", fontName.c_str());

    fontLoaded = false;

    for (auto it = texturePages.begin(); it != texturePages.end(); it++)
    {
        (*it)->freeTexture();
        delete (*it);
    }

    texturePages.clear();
    chars.clear();
}

/// @brief Draws text to the screen.
/// @param renderer The renderer to draw to.
/// @param x The x position to draw at. Relative to the viewport.
/// @param y The y position to draw at. Relative to the viewport.
/// @param text The text to draw.
/// @param color The text color. Default is white.
void BitmapFont::drawText(SDL_Renderer *renderer, int x, int y, std::string text, SDL_Color color)
{
    drawTextAligned(renderer, x, y, text, color);
}

/// @brief Draws the given text with line width and separation.
/// @param renderer The renderer to draw to.
/// @param x The x position to draw at. Relative to the viewport.
/// @param y The y position to draw at. Relative to the viewport.
/// @param text The text to draw.
/// @param lineWidth Line width. If -1, is infinite.
/// @param sep Line separation. If -1, uses the default line height.
/// @param color The text color. Default is white.
void BitmapFont::drawTextExt(SDL_Renderer *renderer, int x, int y, std::string text, int lineWidth, int sep, SDL_Color color)
{
    drawTextAlignedExt(renderer, x, y, text, lineWidth, sep, color);
}

void BitmapFont::drawTextAligned(SDL_Renderer *renderer, int x, int y, std::string text, SDL_Color color, TextAlign hAlign, TextAlign vAlign)
{
    drawTextAlignedExt(renderer, x, y, text, -1, -1, color, hAlign, vAlign);
}

void BitmapFont::drawTextAlignedExt(SDL_Renderer *renderer, int x, int y, std::string text, int lineWidth, int sep, SDL_Color color, TextAlign hAlign, TextAlign vAlign)
{
    if (!fontLoaded)
    {
        SDL_LogInfo(SDL_LOG_CATEGORY_ERROR, "TextDrawer: Failed to draw text. Font has not been loaded.");

        return;
    }

    //Get the X and Y offset
    int xOffset, yOffset;

    getTextAlignOffset(xOffset, yOffset, text, lineWidth, sep, hAlign, vAlign);

    x += xOffset;
    y += yOffset;

    //Draw the text
    int currentWidth = 0;
    char preChar = ' ';

    //Set line separation
    currentSep = (sep == -1) ? lineHeight : sep;

    cursorX = x;
    baseCursorX = x;
    cursorY = y;

    shake = false;

    setDrawColor(color);

    //Loop through and draw each character in the text
    for (unsigned int i = 0; i < text.length(); i++)
    {
        char c = text[i];
        CharInfo *charInfo;
        
        //Handle escape characters
        if (c == '/')
        {
            if (i + 1 < text.length())
            {
                //Skip the escape character
                i++;

                char specialChar = tolower(text[i]);

                //Handle the special character
                if (handleSpecialChar(specialChar))
                {
                    continue;
                }
            }
            else
            {
                continue;
            }
        }

        //Are we at a new word?
        if (lineWidth != -1 && preChar == ' ')
        {
            //Check if the word will fit
            if (currentWidth != 0) //If we are already at the beginning of a line theres no point in checking
            {
                if (currentWidth + getNextWordWidth(text, i) > lineWidth)
                {
                    //Go to the next line
                    cursorX = baseCursorX;
                    cursorY += currentSep;
                    currentWidth = 0;
                }
            }
        }

        //Get the characters font info
        charInfo = getCharInfo(c);

        if (charInfo != NULL)
        {
            //Go to a new line if the width exceeds the line width
            if (lineWidth != -1 && currentWidth + charInfo->xadvance > lineWidth)
            {
                cursorX = baseCursorX;
                cursorY += currentSep;
                currentWidth = 0;
            }

            drawChar(renderer, charInfo);

            //Advance the cursor position
            cursorX += charInfo->xadvance;

            currentWidth += charInfo->xadvance;
        }

        preChar = c;
    }

    //Reset the texture colors
    setDrawColor({255, 255, 255, 255});
}

/// @param text The text to check the width of.
/// @param lineWidth Line width. If -1, is infinite.
/// @param sep Line separation. If -1, uses the default line height.
/// @return The text width
unsigned int BitmapFont::getTextWidth(std::string text, int lineWidth, int sep)
{
    unsigned int width, height;
    int minYOffset;

    getTextSize(width, height, minYOffset, text, lineWidth, sep);

    return width;
}

/// @param text The text to check the height of.
/// @param lineWidth Line width. If -1, is infinite.
/// @param sep Line separation. If -1, uses the default line height.
/// @return The text height
unsigned int BitmapFont::getTextHeight(std::string text, int lineWidth, int sep)
{
    unsigned int width, height;
    int minYOffset;

    getTextSize(width, height, minYOffset, text, lineWidth, sep);

    return height;
}

void BitmapFont::getTextAlignOffset(int &xOffset, int &yOffset, std::string text, int lineWidth, int sep, TextAlign hAlign, TextAlign vAlign)
{
    unsigned  width, height;
    int minYOffset;

    getTextSize(width, height, minYOffset, text, lineWidth, sep);

    xOffset = 0;
    yOffset = 0;

    //Get alignment based on text size
    if (hAlign == RIGHT)
    {
        xOffset -= width;
    }
    else if (hAlign == CENTER)
    {
        xOffset -= width / 2;
    }

    //Align the characters snug against the top by subtracting the smallest yOffset
    yOffset -= minYOffset;

    if (vAlign == BOTTOM)
    {
        yOffset -= height;
    }
    else if (vAlign == CENTER)
    {
        yOffset -= height / 2;
    }
}

/// @brief Gets the width and height of the given text for this font.
/// @param width The returned width.
/// @param height The returned height.
/// @param minYOffset The returned smallest Y Offset for the first line in relation to the drawing position.
/// @param text The text to get the size of.
/// @param lineWidth Line width. If -1, is infinite.
/// @param sep Line separation. If -1, uses the default line height.
void BitmapFont::getTextSize(unsigned int &width, unsigned int &height, int &minYOffset, std::string text, int lineWidth, int sep)
{
    char preChar = ' ';
    int currentWidth = 0;
    bool firstLine;

    width = 0;
    height = 0;

    //Used to determine the tallest character in the first line
    minYOffset = 999;
    firstLine = true;

    //Set line separation
    currentSep = (sep == -1) ? lineHeight : sep;

    //Loop through and draw each character in the text
    for (unsigned int i = 0; i < text.length(); i++)
    {
        char c = text[i];
        CharInfo *charInfo;
        
        //Handle escape characters
        if (c == '/')
        {
            if (i + 1 < text.length())
            {
                //Skip the escape character
                i++;

                char specialChar = tolower(text[i]);

                //Handle the special character
                if (specialChar == 'n')
                {
                    currentWidth = 0;
                    height += currentSep;
                    firstLine = false;

                    continue;
                }
            }
            else
            {
                continue;
            }
        }

        //Are we at a new word?
        if (lineWidth != -1 && preChar == ' ')
        {
            //Check if the word will fit
            if (currentWidth != 0) //If we are already at the beginning of a line theres no point in checking
            {
                if (currentWidth + getNextWordWidth(text, i) > lineWidth)
                {
                    currentWidth = 0;
                    height += currentSep;
                    firstLine = false;
                }
            }
        }

        //Get the characters font info
        charInfo = getCharInfo(c);

        if (charInfo != NULL)
        {
            //Go to a new line if the width exceeds the line width
            if (lineWidth != -1 && currentWidth + charInfo->xadvance > lineWidth)
            {
                currentWidth = 0;
                height += currentSep;
                firstLine = false;
            }

            //Get the minimum Y Offset
            if (firstLine)
            {
                minYOffset = std::min(minYOffset, charInfo->yoffset);
            }

            //Get the max width
            currentWidth += charInfo->xadvance;
            width = fmax(currentWidth, width);
        }

        preChar = c;
    }

    //Default to 0 if not characters in the first line
    if (minYOffset == 999)
    {
        minYOffset = 0;
    }

    //Add the first lines height to the text height
    height += base - minYOffset;
}

int BitmapFont::getNextWordWidth(std::string text, int charIndex)
{
    int wordWidth = 0;

    for (unsigned int j = charIndex; j < text.length(); j++)
    {
        char wordChar = text[j];

        //Done?
        if (wordChar == ' ')
        {
            break;
        }
        //Handle special characters
        else if (wordChar == '/')
        {
            if (j + 1 < text.length())
            {
                j++;

                //We are done if its a new line
                if (text[j] == 'n')
                {
                    break;
                }
                else
                {
                    //Skip the special character
                    continue;
                }
            }
        }
        else
        {
            wordWidth += getCharInfo(wordChar)->xadvance;
        }
    }

    return wordWidth;
}

/// @brief Draws a character to the screen using the current cursor position.
/// @param renderer The renderer to draw to.
/// @param charInfo The font info on the character to draw.
void BitmapFont::drawChar(SDL_Renderer *renderer, CharInfo *charInfo)
{
    SDL_FRect clip;
    Texture *texturePage;
    int drawX, drawY;

    //Display the character
    clip = {(float) charInfo->x, (float) charInfo->y, (float) charInfo->width, (float) charInfo->height};
    texturePage = texturePages.at(charInfo->page);

    drawX = cursorX + charInfo->xoffset;
    drawY = cursorY + charInfo->yoffset;

    //Apply shake
    if (shake)
    {
        drawX += rng->getRandomInt(-1, 1);
        drawY += rng->getRandomInt(-1, 1);
    }

    texturePage->render(renderer, drawX, drawY, 1.0, 1.0, &clip);
}

/// @brief Handles a special character.
/// @param specialChar The character.
/// @return True if the current characters rendering should be skipped. False otherwise.
bool BitmapFont::handleSpecialChar(char specialChar)
{
    switch (specialChar)
    {
        case '/':
        {
            //Continue to draw normally, we want to draw the forward slash
        }
        break;

        case 'n':
        {
            //Go to a new line
            cursorX = baseCursorX;
            cursorY += currentSep;

            return true;
        }
        break;

        case 's':
        {
            //Toggle shaking
            shake = !shake;

            return true;
        }
        break;

        case 'w':
        {
            //Set the text color to white
            setDrawColor({255, 255, 255, 255});

            return true;
        }
        break;

        case 'd':
        {
            //Set the text color to black (dark)
            setDrawColor({0, 0, 0, 255});

            return true;
        }
        break;

        case 'r':
        {
            //Set the text color to red
            setDrawColor({255, 0, 0, 255});

            return true;
        }
        break;

        case 'g':
        {
            //Set the text color to green
            setDrawColor({0, 255, 0, 255});

            return true;
        }
        break;

        case 'b':
        {
            //Set the text color to blue
            setDrawColor({0, 0, 255, 255});

            return true;
        }
        break;
    }

    return false;
}

/// @param c The character to get the info for.
/// @return The fonts CharInfo for the given character, or NULL if the character isn't in the font.
CharInfo* BitmapFont::getCharInfo(char c)
{
    CharInfo *charInfo;
    int charId = -1;

    //Get the characters ASCII/unicode ID
    charId = static_cast<int>(c);

    //Couldn't get the character ID from the font
    //THIS SHOULDN'T HAPPEN
    if (charId == -1)
    {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "BitmapFont: Failed to render character %c. No proper conversion to font character format.", c);

        return NULL;
    }

    //Make sure the character is in our character list
    if (chars.find(charId) == chars.end())
    {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "BitmapFont: Failed to render character %c. Character is not in the font.", c);

        return NULL;
    }

    charInfo = &chars.at(charId);

    return charInfo;
}

/// @brief Sets a new draw color/alpha if the color or alpha was changed.
/// @param color The draw color to set.
void BitmapFont::setDrawColor(SDL_Color color)
{
    //Set color mod if color was changed
    if (drawColor.r != color.r || drawColor.g != color.g || drawColor.b != color.b)
    {
        for (unsigned int i = 0; i < texturePages.size(); i++)
        {
            texturePages[i]->setColorMod(color.r, color.g, color.b);
        }

        drawColor.r = color.r;
        drawColor.g = color.g;
        drawColor.b = color.b;
    }

    setAlpha(color.a);
}

/// @brief Sets the alpha for the font.
/// @param alpha Alpha, from 0 to 255.
void BitmapFont::setAlpha(Uint8 alpha)
{
    //Set alpha if it was changed
    if (drawColor.a != alpha)
    {
        for (unsigned int i = 0; i < texturePages.size(); i++)
        {
            texturePages[i]->setAlpha(alpha);
        }

        drawColor.a = alpha;
    }
}