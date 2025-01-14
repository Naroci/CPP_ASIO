#include <iostream>
#include <vector>
#include <string>
#include <filesystem>
#include <list>
#include <array>

#define ASIO_STANDALONE
#include <asio.hpp>
#include <asio/ts/buffer.hpp>
#include <asio/ts/internet.hpp>
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
