#include "engine_utils.hpp"
#include <iostream>

// We are on Mac
#ifdef __APPLE__

#include <mach-o/dyld.h>
#include <limits.h>

/// <summary>
/// Returns the relative path of the input as a string.
/// Return values differ by operating system as there are differences between Windows and Mac.
/// </summary>
/// <param name="path">The string path</param>
/// <returns>The relative path</returns>
std::string EngineUtils::GetRelativePath(std::string path)
{
        char buf[PATH_MAX];
        uint32_t buf_size = PATH_MAX;
        
        if (!_NSGetExecutablePath(buf, &buf_size)) {
            
            std::string out = std::string(buf); 
            return out.erase(out.size() - 8) + path;

        }

        return "";
}

// We are on windows or linux
#else

/// <summary>
/// Returns the relative path of the input as a string.
/// Return values differ by operating system as there are differences between Windows and Mac.
/// </summary>
/// <param name="path">The string path</param>
/// <returns>The relative path</returns>
std::string EngineUtils::GetRelativePath(std::string path)
{
    return path;
}

#endif