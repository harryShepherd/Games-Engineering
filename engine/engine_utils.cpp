#include "engine_utils.hpp"
#include <iostream>

#ifdef __APPLE__

#include <mach-o/dyld.h>
#include <limits.h>

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

#else

std::string EngineUtils::GetRelativePath(std::string path)
{
    return path;
}

#endif