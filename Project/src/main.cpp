#include <iostream>
#include <list>
#include <string>
#include <vector>

#include "../include/HttpClient.hpp"
#include "../include/OSHelper.h"
#include "../include/FileAccess.hpp"

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
    std::string result = httpClient.DownloadString(url, 80);
    std::cout << "RESULT:::" << std::endl;
    std::cout << result << std::endl;
    
    std::cout << "Enter a FilePath:";
    std::string filePath;
    std::cin >> filePath;
    if (!filePath.empty())
    {
        FileAccess acces(filePath);
        acces.WriteStringToFile(result);
    }
    return 0;
}
