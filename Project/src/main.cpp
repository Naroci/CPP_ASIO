#include <iostream>
#include <list>
#include <string>
#include <vector>

#include "../include/HttpClient.hpp"
#include "../include/OSHelper.h"

bool readingData = false;

int main()
{
    std::cout << "test \n";
    std::string url;
    std::cin >> url;
    if (url.size() < 1)
        return 2;

    int asd = 0;
    std::cout << OSHelper::GetCurrentOperatingSystemName() << std::endl;
    HttpClient httpClient;
    std::string result = httpClient.DownloadString(url, 80);
    std::cout << result << std::endl;
    return 0;
}
