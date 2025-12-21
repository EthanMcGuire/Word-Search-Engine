#pragma once

#ifndef FILE_UTILS_H
#define FILE_UTILS_H

#include <string>

namespace File
{
    struct ReadFileData
    {
	void *data;
	size_t bytes;
    };

    /// @brief Creates the folder path if it does not exist. Recursively creates any missing folders.
    /// @param directoryPath The folder path.
    /// @return True if successful or the directory already exists, false otherwise.
    bool createFolder(std::wstring directoryPath);
    bool createFolder(std::string directoryPath);

    bool writeToFile(std::string filePath, const void *data);
    ReadFileData *readFile(std::string filePath);
    void freeReadFileData(ReadFileData *fileData);

    std::string getGameFolderPath();
}

#endif
