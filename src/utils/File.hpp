#pragma once

#include <cstring>
#include <fstream>
#include <functional>
#include <stdexcept>

struct FileError : std::runtime_error
{
    FileError(std::string const& message)
    : std::runtime_error(message)
    {
    }
};

class File
{
public:
    File(std::string filename)
    : file(filename)
    {
        if (not file.is_open())
        {
            throw FileError("error opening file " + filename);
        }
    }

    void forEachLine(std::function<void(std::string)> callback)
    {
        std::string line;

        while (std::getline(file, line))
        {
            callback(line);
        }
    }

private:
    std::ifstream file;
};
