#include "../include/OSHelper.h"
#include <string>


OSHelper::OperatingSystem OSHelper::GetCurrentOperatingSystem(){
    #ifdef __APPLE__
        return OSHelper::MacOS;
    #elif _WIN32
        return OSHelper::Windows;
    #elif __linux__
        return OSHelper::Linux;
    #else
        return OSHelper::Unspecified;
    #endif
}

std::string OSHelper::GetCurrentOperatingSystemName(){
    #ifdef __APPLE__
        return "MacOS";
    #elif _WIN32
        return "Windows";
    #elif __linux__
        return "Linux";
    #else
        return "Unspecified";
    #endif
}