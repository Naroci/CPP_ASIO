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

/// @brief Tries to resolve all endpoints by the given Url (std::string) and
/// Port (int)
/// @param StringUrl Base URL Address (raw without :// information like http://,
/// ftp:// etc.)
/// @param port The Port (80, 443 etc).
/// @return asio::ip::basic_resolver_results<asio::ip::tcp>
asio::ip::basic_resolver_results<asio::ip::tcp>
HttpClient::GetEndpointsFromString(std::string StringUrl, int port)
{
    try
    {
        // Create a resolver to perform DNS lookups
        asio::ip::tcp::resolver resolver(HttpClient::context);
        asio::ip::tcp::resolver::results_type endpoints =
            resolver.resolve(StringUrl, std::to_string(port));
        if (endpoints.empty())
            return asio::ip::basic_resolver_results<asio::ip::tcp>();

        return endpoints;
    }
    catch (const std::runtime_error& e)
    {
        std::cout << "Error resolving the endpoint" << std::endl;
        return asio::ip::basic_resolver_results<asio::ip::tcp>();
    }

    return asio::ip::basic_resolver_results<asio::ip::tcp>();
}

/// @brief Tries to extract the data from the given bytes and returns them as a
/// HttpContentData. Holding its Header and Body.
/// @param load The raw data as a vector<byte> transfered from the stream.
/// @return HttpContentData
HttpClient::HttpContentData
HttpClient::ExtractData(std::vector<char> &load)
{
    HttpClient::HttpContentData returnData;
    //std::vector<char> vectorCharResult =
    //    HttpClient::GetCharContentFromBytes(load);
    std::vector<char> body;
    const std::string headerEnd = "\r\n\r\n"; // End of the header
    std::string header;

    // Search for the header end in the vector of characters
    auto it = std::search(load.begin(), load.end(),
                          headerEnd.begin(), headerEnd.end());

    if (it != load.end()) // Check if the end of the header is found
    {
        // Extract the header (from the beginning to the found header end)
        header = std::string(load.data(),
                             std::distance(load.begin(), it));

        // Extract the body (data after the header)
        auto bodyStart = it + headerEnd.size();
        body.assign(bodyStart, load.end());
    }
    else
    {
        std::cerr << "Kein Header-Ende gefunden. Ganzer Input wird als Body "
                     "interpretiert.\n ";
        // If no header end is found, the whole input is treated as the body
        body.assign(load.begin(), load.end());
    }

    returnData.Header = header;
    std::cout<< "RESPONSE HEADER : " << std::endl;
    std::cout<< returnData.Header << std::endl;
    returnData.Body = body; // Assuming you want to store the body as well

    return returnData;
}

// Auslesen der bytes andhand des aktuellen sockets und des status des socket.
// insgesamt dem ganzen verhalten aus csharp sehr aehnlich.
std::vector<char> HttpClient::ReadBytesFromSocket()
{
    std::vector<char> collectedBytes;
    int bytesCollected = 0;
    std::chrono::time_point timeStart = std::chrono::system_clock::now();
    if (HttpClient::getSocket()->is_open())
    {
        std::cout << "Writing Header: \n" << mHeader;
        HttpClient::getSocket()->write_some(
            asio::buffer(HttpClient::mHeader.data(),
                         HttpClient::mHeader.size()),
            HttpClient::m_errorCode);
        HttpClient::getSocket()->wait(HttpClient::getSocket()->wait_read);

        while (!HttpClient::m_errorCode)
        {
            std::vector<char> mBuffer(mBufferSize);
            auto bytes = HttpClient::getSocket()->read_some(
                asio::buffer(mBuffer.data(), mBuffer.size()),
                HttpClient::m_errorCode);
            if (!HttpClient::m_errorCode && bytes > 0)
            {
                collectedBytes.insert(collectedBytes.end(), (mBuffer.data()),
                                      (mBuffer.data() + bytes));
                bytesCollected += bytes;
            }

            if (HttpClient::m_errorCode ||
                HttpClient::m_errorCode == asio::error::eof)
            {
                HttpClient::m_errorCode.clear();
                break;
            }
        }
    }

    auto duration = std::chrono::system_clock::now() - timeStart;
    HttpClient::getSocket()->close();

    std::cout << "bytes collected: " << bytesCollected / 1000.0 << std::endl;
    std::cout << "took: "
              << std::chrono::duration_cast<std::chrono::milliseconds>(duration)
                     .count()
              << " ms" << std::endl;

    return collectedBytes;
}

/// @brief Converts the given std::vector<std::byte> into a std::vector<char>
/// @param inputBytes the input vector of bytes.
/// @return std::vector<char>
std::vector<char>
HttpClient::GetCharContentFromBytes(std::vector<std::byte> &inputBytes)
{
    if (inputBytes.size() < 1)
        return std::vector<char>();

    std::vector<char> returnValue;

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

HttpClient::HttpContentData HttpClient::ReadFromSocket()
{
    HttpClient::HttpContentData result;
    auto byteResult = ReadBytesFromSocket();
    auto dataResult = ExtractData(byteResult);

    result = dataResult;
    return result;
}

/// @brief Downloads a string from the given url and port and returns it.
/// @param url the url (ex. http://wwww.gooogle.de)
/// @param port ex. 80, 443 etc
/// @return std::string
std::string HttpClient::DownloadString(std::string url, int port = 80)
{
    bool connected = connectClient(url, port, RequestHeaderBuilder::GET);
    if (connected == true)
    {
        HttpClient::HttpContentData result = ReadFromSocket();
        std::string stringResult = getStringResult(result.Body);
        return stringResult;
    }
    return "";
}

std::vector<char> HttpClient::DownloadByteData(std::string url, int port = 80)
{
    bool connected = connectClient(url, port, RequestHeaderBuilder::GET);
    if (connected == true)
    {
        HttpClient::HttpContentData result = ReadFromSocket();
        return result.Body;
    }
    return std::vector<char>();
}

bool HttpClient::connectClient(std::string url, int port,
                               RequestHeaderBuilder::RequestType type)
{
    std::string testRequestHeader =
        mHeaderBuilder.CreateHeaderRequest(url, type);

    url = checkURL(url);
    auto parsedurl = mHeaderBuilder.ParseUrl(url);
   
    HttpClient::mHeader = testRequestHeader;
    // Intern den aktuellen Endpunkt setzen.
    HttpClient::currentEndpointUrl = parsedurl.size() > 0 ? parsedurl[0] :  url;
    HttpClient::mPort = port;

    // Aufloesen der moeglichen Endpunkte.
    asio::ip::basic_resolver_results<asio::ip::tcp> endpoints =
        GetEndpointsFromString(HttpClient::currentEndpointUrl,
                               HttpClient::mPort);

    if (!endpoints.empty())
    {
        // Display resolved IP addresses
        for (const auto &entry : endpoints)
        {
            std::cout << "Resolved IP: " << entry.endpoint().address()
                      << std::endl;
        }

        // Create the main socket

        HttpClient::setSocket(new asio::ip::tcp::socket(HttpClient::context));
        // Start the connection
        HttpClient::getSocket()->connect(endpoints.begin()->endpoint(),
                                         HttpClient::m_errorCode);
        if (!HttpClient::m_errorCode)
        {
            std::cout << "Connected" << std::endl;
            return true;
        }
        else
        {
            std::cout << "Failed to connect to: "
                      << HttpClient::m_errorCode.message() << std::endl;
            return false;
        }
    }
    return false;
}

bool HttpClient::disconnectClient()
{
    HttpClient::context.stop();
    if (HttpClient::getSocket()->is_open())
        HttpClient::getSocket()->close();

    delete HttpClient::m_socket;

    return true;
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
    return "";
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
