#include "fileUtils.hpp"
#include <filesystem>
#include <SDL3/SDL_log.h>
#include <SDL3/SDL_iostream.h>
#include <SDL3/SDL_filesystem.h>

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

/// @brief Creates the folder path if it does not exist. Recursively creates any missing folders.
/// @param directoryPath The folder path.
/// @return True if successful or the directory already exists, false otherwise.
bool File::createFolder(std::string directoryPath)
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
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "File:: Failed to create directory %s. Error: %s", directoryPath.c_str(), e.what());

        return false;
    }

    return true;
}

bool File::writeToFile(std::string filePath, const void *data)
{
	bool success;
	SDL_IOStream *file;
	size_t dataSize, writtenBytes;

	file = SDL_IOFromFile(filePath.c_str(), "wb");

	if (file == NULL)
	{
		SDL_LogError(SDL_LOG_CATEGORY_ERROR, "File: Failed to open file for writing. Error: %s", SDL_GetError());
		
		return false;
	}
	
	dataSize = sizeof(data);
	writtenBytes = SDL_WriteIO(file, data, dataSize);

	success = true;

	if (writtenBytes < dataSize)
	{
		SDL_LogError(SDL_LOG_CATEGORY_ERROR, "File: Failed to write to file. Written bytes is less than the number of expected bytes. Error: %s", SDL_GetError());

		success = false;
	}	

	if (!SDL_CloseIO(file))
	{
		SDL_LogError(SDL_LOG_CATEGORY_ERROR, "File: Close file stream. Error: %s", SDL_GetError());
	}

	return success;
}

File::ReadFileData *File::readFile(std::string filePath)
{
	File::ReadFileData *fileData;
	void *data;
	SDL_IOStream *file;
	size_t readBytes;

	file = SDL_IOFromFile(filePath.c_str(), "rb");

	if (file == NULL)
	{
		SDL_LogError(SDL_LOG_CATEGORY_ERROR, "File: Failed to open file for reading. Error: %s", SDL_GetError());
		
		return NULL;
	}

	data = SDL_LoadFile_IO(file, &readBytes, true);

	if (data == NULL)
	{
		SDL_LogError(SDL_LOG_CATEGORY_ERROR, "File: Failed to read file. Error: %s", SDL_GetError());

		return NULL;
	}

	fileData = new File::ReadFileData({data, readBytes});

	SDL_Log("File: Successfully read file. File: %s, Bytes read: %ld", filePath.c_str(), readBytes);

	return fileData;
}

void File::freeReadFileData(File::ReadFileData *fileData)
{
	if (fileData == NULL)
	{
		SDL_LogError(SDL_LOG_CATEGORY_ERROR, "File: Attempted to free NULL file data!");

		return;
	}

	SDL_free(fileData->data);

	delete fileData;
}

std::string File::getGameFolderPath()
{
        const char *documentsPath;
        std::string folderPath;

        documentsPath = SDL_GetUserFolder(SDL_FOLDER_DOCUMENTS);

        if (documentsPath == NULL)
        {
                SDL_Log("File: Failed to get path to the documents folder. Error: %s", SDL_GetError());

                return "";
        }

        folderPath = documentsPath;
        folderPath += "/wordSearch";

	//Create the folder
	if (!File::createFolder(folderPath))
        {
                SDL_Log("File: Failed to create the game folder. Path: %s", folderPath.c_str());

                return "";
        }

        return folderPath;
}
