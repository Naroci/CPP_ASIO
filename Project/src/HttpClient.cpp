//
// Created by naroci on 13.10.24.
//
#include "../include/HttpClient.hpp"

#define ASIO_STANDALONE
#include "../include/OSHelper.h"
#include <asio.hpp>
#include <asio/ts/buffer.hpp>
#include <asio/ts/internet.hpp>
#include <cctype> // for std::isprint

#include <array>
#include <chrono>
#include <filesystem>
#include <iostream>
#include <list>
#include <stdexcept>
#include <string>
#include <vector>

asio::ip::basic_resolver_results<asio::ip::tcp>
HttpClient::GetEndpointsFromString(std::string StringUrl, int port)
{
    // Create a resolver to perform DNS lookups
    asio::ip::tcp::resolver resolver(HttpClient::context);
    asio::ip::tcp::resolver::results_type endpoints =
        resolver.resolve(StringUrl, std::to_string(port));
    if (endpoints.empty())
        return asio::ip::basic_resolver_results<asio::ip::tcp>();

    return endpoints;
}

std::string HttpClient::ReadFromSocket(asio::ip::tcp::socket &sock,
                                       asio::error_code &ec)
{
    std::vector<char> collectedData;
    std::string result;
    int bytesCollected = 0;
    std::chrono::time_point timeStart = std::chrono::system_clock::now();
    if (sock.is_open())
    {
        std::string requestHeader = "GET /index.html HTTP/1.1\r\n"
                                    "Host: " +
                                    HttpClient::currentEndpointUrl +
                                    "\r\n"
                                    "Connection: close\r\n\r\n";

        sock.write_some(
            asio::buffer(requestHeader.data(), requestHeader.size()), ec);
        sock.wait(sock.wait_read);

        while (!ec)
        {
            std::vector<char> mBuffer(mBufferSize);
            auto bytes = sock.read_some(
                asio::buffer(mBuffer.data(), mBuffer.size()), ec);
            if (!ec && bytes > 0)
            {
                bytesCollected += bytes;
                collectedData.insert(collectedData.end(), mBuffer.begin(),
                                     mBuffer.end());
            }

            if (ec || ec == asio::error::eof)
            {
                ec.clear();
                break;
            }
        }
    }

    auto duration = std::chrono::system_clock::now() - timeStart;
    sock.close();

    std::cout << "bytes collected: " << bytesCollected / 1000.0 << std::endl;
    std::cout << "took: " << duration.count() << std::endl;
    result = getStringResult(collectedData);

    return result;
}

// Fetches the content of the given url as a string and returns it.
std::string HttpClient::DownloadString(std::string url, int port = 80)
{
    url = checkURL(url);
    HttpClient::currentEndpointUrl = url;
    asio::ip::basic_resolver_results<asio::ip::tcp> endpoints =
        GetEndpointsFromString(url, port);
    if (!endpoints.empty())
    {
        // Display resolved IP addresses
        for (const auto &entry : endpoints)
        {
            std::cout << "Resolved IP: " << entry.endpoint().address()
                      << std::endl;
        }

        asio::error_code ec;
        // Create the main socket
        asio::ip::tcp::socket socket(HttpClient::context);
        // Start the connection
        socket.connect(endpoints.begin()->endpoint(), ec);

        if (!ec)
        {
            std::cout << "Connected" << std::endl;
        }
        else
        {
            std::cout << "Failed to connect to: " << ec.message() << std::endl;
            return NULL;
        }
        std::string result = ReadFromSocket(socket, ec);
        HttpClient::context.stop();

        // Convert the collected data to a string

        return result;
    }
    return nullptr;
}

// prepares the given address and removes "http://" or "https://" from the
// beginning of the string.
std::string HttpClient::checkURL(std::string url)
{
    auto res = url.find("https://", 0);
    if (url.find("https://", 0) != std::string::npos)
    {
        std::string https = "https://";
        int size = https.size();
        auto returnstring = url.substr(size, url.size() - size);
        return returnstring;
    }
    else if (url.find("http://", 0) != std::string::npos)
    {
        std::string https = "http://";
        int size = https.size();
        auto returnstring = url.substr(size, url.size() - size);
        return returnstring;
    }
    return NULL;
}

// Returns a string based on the given std::vector<char>
std::string HttpClient::getStringResult(std::vector<char> &resultBuffer)
{
    if (resultBuffer.empty())
        return ""; // Return an empty string if no data

    std::string result;
    bool lastCharWasNewline =
        false; // Flag to track if the last character was a newline

    for (char c : resultBuffer)
    {
        // Check if the character is printable or a special control character
        // like \n, \r, or \t
        if (std::isprint(static_cast<unsigned char>(c)) || c == '\n' ||
            c == '\r' || c == '\t')
        {
            // Avoid appending multiple consecutive newline characters
            if ((c == '\n' || c == '\r') && lastCharWasNewline)
            {
                continue; // Skip this character to avoid empty lines
            }

            result += c; // Append the character to the result
            lastCharWasNewline =
                (c == '\n' || c == '\r'); // Update flag for newline check
        }
    }

    return result;
}
