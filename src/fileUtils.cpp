#include "fileUtils.hpp"
#include <filesystem>
#include <SDL3/SDL_log.h>

/// @brief Creates the folder path if it does not exist. Recursively creates any missing folders.
/// @param directoryPath The folder path.
/// @return True if successful or the directory already exists, false otherwise.
bool File::createFolder(std::wstring directoryPath)
{
    try
    {
        //Do nothing if already exists
        if (std::filesystem::exists(directoryPath))
        {
            return true;
        }

        std::filesystem::create_directories(directoryPath);
    }
    catch(const std::exception& e)
    {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "File:: Failed to create directory %ls. Error: %s", directoryPath.c_str(), e.what());

        return false;
    }

    return true;
}