#pragma once

#include <string>

class EngineUtils
{
public:
    // Returns the string of the relative path.
    // The return values differs by operating system,
    // as there are differences between Windows & Mac
    // and how they interact with relative paths.
    static std::string GetRelativePath(std::string path);
};