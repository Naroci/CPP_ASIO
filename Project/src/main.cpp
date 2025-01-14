#include <iostream>
#include <vector>
#include <string>
#include <filesystem>
#include <list>
#include <array>

#include "../include/HttpClient.hpp"
#include "../include/OSHelper.h"


bool readingData = false;

int main()
{
    std::string url;
    std::cin >> url;
    if (url.size() < 1)
        return 2;

    std::cout<<OSHelper::GetCurrentOperatingSystemName()<<std::endl;
    HttpClient httpClient;
    std::string result = httpClient.DownloadString(url,80);
    std::cout<<result<<std::endl;
    return 0;
}
