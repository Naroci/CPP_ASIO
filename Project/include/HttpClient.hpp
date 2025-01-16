#ifndef HTTPCLIENT_HPP
#define HTTPCLIENT_HPP

#include "RequestHeaderBuilder.hpp"
#include <asio/ip/tcp.hpp>
#include <string>
#include <vector>

class HttpClient
{
  public:
    HttpClient() {}
    struct HttpContentData
    {
        std::string Header;
        std::vector<char> Body;
    };

    HttpContentData ExtractData(std::vector<std::byte> &load);

    // Main Test Function.
    std::string DownloadString(std::string url, int port);

    // Getters & Setters.
    void SetBufferSize(int size) { mBufferSize = size; };
    int GetBufferSize() const { return mBufferSize; }

    std::vector<std::byte> ReadBytesFromSocket(asio::ip::tcp::socket &sock,
                                               asio::error_code &ec);


    HttpContentData ReadFromSocket(asio::ip::tcp::socket &sock,
                               asio::error_code &ec);

    std::string GetEndpointAddress() const { return mEndpointAdr; }

    asio::ip::basic_resolver_results<asio::ip::tcp>
    GetEndpointsFromString(std::string StringUrl, int port);

    int GetPort() const { return mPort; }
    bool GetIfReading() const { return mIsReading; }

  private:
    bool mIsReading;
    int mPort = 80;
    int mBufferSize = 4096;

    std::string currentEndpointUrl;
    std::string mEndpointAdr;
    std::string mHeader;

    asio::io_context context;
    RequestHeaderBuilder mHeaderBuilder;

    std::string checkURL(std::string url);
    std::string getStringResult(std::vector<char> &resultBuffer);

    std::string GetStringFromBytes(std::vector<std::byte> &inputBytes);
    std::vector<char> GetCharContentFromBytes(std::vector<std::byte> &inputBytes);
};

#endif // HTTPCLIENT_HPP
