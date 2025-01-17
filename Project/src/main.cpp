#include <iostream>
#include <list>
#include <string>
#include <vector>

#include "../include/FileAccess.hpp"
#include "../include/HttpClient.hpp"
#include "../include/OSHelper.h"

bool readingData = false;

int main()
{
    std::cout << "TEST \n";
    std::string url;
    std::cout << "Enter a url:";
    std::cin >> url;
    if (url.size() < 1)
        return 2;

    int asd = 0;
    std::cout << OSHelper::GetCurrentOperatingSystemName() << std::endl;
    HttpClient httpClient;
    auto result = httpClient.DownloadByteData(url, 80);
    /*
        auto result = httpClient.DownloadString(url, 80);
        std::cout << "RESULT:::" << std::endl;
        std::cout << result << std::endl;
    */

    if (result.size() > 0)
    {
        std::cout << "Enter a FilePath:";
        std::string filePath;
        std::cin >> filePath;
        if (!filePath.empty())
        {
            FileAccess acces(filePath);
            acces.WriteBytesToFile(result);
        }
    }
    else
    {
        std::cout << "Nothing Downloaded..." << std::endl;
    }

    return 0;
}
