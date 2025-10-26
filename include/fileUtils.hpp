#pragma once

#ifndef FILE_UTILS_H
#define FILE_UTILS_H

#include <string>

namespace File
{
    /// @brief Creates the folder path if it does not exist. Recursively creates any missing folders.
    /// @param directoryPath The folder path.
    /// @return True if successful or the directory already exists, false otherwise.
    bool createFolder(std::wstring directoryPath);
}

#endif