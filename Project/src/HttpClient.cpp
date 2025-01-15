//
// Created by naroci on 13.10.24.
//
#include "../include/HttpClient.hpp"
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

// Versucht anhand der  uebergebenen url und dessen port entsprechende
// endpoints aufzuloesen.
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

HttpClient::HttpContentData
HttpClient::ExtractData(std::vector<std::byte> &load)
{
    HttpClient::HttpContentData returnData;
    std::vector<char> vectorCharResult =
        HttpClient::GetCharContentFromBytes(load);
    std::vector<char> body;
    const std::string headerEnd = "\r\n\r\n"; // End of the header
    std::string header;

    // Search for the header end in the vector of characters
    auto it = std::search(vectorCharResult.begin(), vectorCharResult.end(),
                          headerEnd.begin(), headerEnd.end());

    if (it != vectorCharResult.end()) // Check if the end of the header is found
    {
        // Extract the header (from the beginning to the found header end)
        header = std::string(vectorCharResult.data(),
                             std::distance(vectorCharResult.begin(), it));

        // Extract the body (data after the header)
        auto bodyStart = it + headerEnd.size();
        body.assign(bodyStart, vectorCharResult.end());
    }
    else
    {
        std::cerr << "Kein Header-Ende gefunden. Ganzer Input wird als Body "
                     "interpretiert.\n ";
        // If no header end is found, the whole input is treated as the body
        body.assign(vectorCharResult.begin(), vectorCharResult.end());
    }

    returnData.Header = header;
    returnData.Body = body; // Assuming you want to store the body as well

    return returnData;
}

// Auslesen der bytes andhand des aktuellen sockets und des status des socket.
// insgesamt dem ganzen verhalten aus csharp sehr aehnlich.
std::vector<std::byte>
HttpClient::ReadBytesFromSocket(asio::ip::tcp::socket &sock,
                                asio::error_code &ec)
{

    std::vector<std::byte> collectedBytes;
    int bytesCollected = 0;
    std::chrono::time_point timeStart = std::chrono::system_clock::now();
    if (sock.is_open())
    {
        // Im idealfall spaeter auslagern.
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
            std::vector<std::byte> mBuffer(mBufferSize);
            auto bytes = sock.read_some(
                asio::buffer(mBuffer.data(), mBuffer.size()), ec);
            if (!ec && bytes > 0)
            {
                collectedBytes.insert(collectedBytes.end(), (mBuffer.data()),
                                      (mBuffer.data() + bytes));
                bytesCollected += bytes;
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
    std::cout << "took: "
              << std::chrono::duration_cast<std::chrono::milliseconds>(duration)
                     .count()
              << " ms" << std::endl;

    return collectedBytes;
}

std::vector<char>
HttpClient::GetCharContentFromBytes(std::vector<std::byte> &inputBytes)
{
    if (inputBytes.size() < 1)
        return std::vector<char>();

    std::vector<char> returnValue;
    // returnValue.reserve(inputBytes.size());

    std::vector<char> collectedData;
    for (const auto byteVal : inputBytes)
    {
        char value = static_cast<char>(byteVal);
        returnValue.push_back(value);
    }

    return returnValue;
}

std::string HttpClient::GetStringFromBytes(std::vector<std::byte> &inputBytes)
{
    if (inputBytes.size() < 1)
        return "";

    std::string returnValue;
    returnValue.reserve(inputBytes.size());

    std::vector<char> collectedData;
    for (const auto byteVal : inputBytes)
    {
        char value = static_cast<char>(byteVal);
        returnValue.push_back(value);
    }

    return returnValue;
}

std::string HttpClient::ReadFromSocket(asio::ip::tcp::socket &sock,
                                       asio::error_code &ec)
{
    std::string result;
    auto byteResult = ReadBytesFromSocket(sock, ec);
    auto dataResult = ExtractData(byteResult);
    result = GetStringFromBytes(byteResult);

    // Ende des Response Header.
    // -> Anhand des Index Header von Nutzlast trennen und entsprechend
    // auswerten (bei string).
    auto index = result.find("\r\n\r\n");

    if (index > 0)
    {
        std::string header = result.substr(0, index);
        if (header.size() > 0)
        {
            std::cout << "Header?>>>>>>>>>>:\n " << header
                      << "\nHEADER END?\n\n";
        }

        std::string content = result.substr(index, result.size() - index);
        if (content.size() > 0)
        {
            std::cout << "Content?>>>>>>>>>>:\n " << content
                      << "\nCONTENT END?\n\n";
        }
    }
    return result;
}

// Fetches the content of the given url as a string and returns it.
std::string HttpClient::DownloadString(std::string url, int port = 80)
{
    // URL Check. http und https muessen entfernt werden, da sie sonst zu
    // fehlern bei der adress -aufloesung fuehren.

    url = checkURL(url);
    auto results = HttpClient::GetURLSubValues(url);
    if (!results.empty())
    {
        for (const std::string entry : results)
        {
            std::cout << entry << "\n";
        }

        std::cout << "\n";
    }

    // Intern den aktuellen Endpunkt setzen.
    HttpClient::currentEndpointUrl = url;

    // Aufloesen der moeglichen Endpunkte.
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
        // auto rbyteResult = ReadBytesFromSocket(socket, ec);
        std::string result = ReadFromSocket(socket, ec);
        // std::string result = ReadFromSocket(socket, ec);
        HttpClient::context.stop();

        return result;
    }
    return nullptr;
}

std::vector<std::string> HttpClient::GetURLSubValues(std::string url)
{
    std::vector<std::string> returnValues;
    std::string currentStringValue;
    std::string searchurl = url;
    std::string searchmask = "://";
    int urlStart = searchurl.find(searchmask);

    if (urlStart != std::string::npos)
    {
        // Skip the initial "://" (e.g., in "https://", "ftp://")
        searchurl = searchurl.substr(urlStart + searchmask.size());
    }

    int res = searchurl.find('/');
    if (res > -1)
    {
        // Loop through each character in the modified searchurl
        for (const char chara : searchurl)
        {
            if (chara == '/')
            {
                if (!currentStringValue.empty())
                {
                    returnValues.push_back(
                        currentStringValue); // Add non-empty segments
                }
                currentStringValue.clear(); // Reset the current segment
            }
            else
            {
                currentStringValue += chara; // Add character to current segment
            }
        }

        // Add the last segment if it exists
        if (!currentStringValue.empty())
        {
            returnValues.push_back(currentStringValue);
        }
    }

    return returnValues;
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
