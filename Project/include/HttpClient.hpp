#ifndef HTTPCLIENT_HPP
#define HTTPCLIENT_HPP

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
    std::string ReadFromSocket(asio::ip::tcp::socket &sock,
                               asio::error_code &ec);

    std::string GetEndpointAddress() const { return mEndpointAdr; }

    asio::ip::basic_resolver_results<asio::ip::tcp>
    GetEndpointsFromString(std::string StringUrl, int port);

    int GetPort() const { return mPort; }
    bool GetIfReading() const { return mIsReading; }

  private:
  std::vector<char> GetCharContentFromBytes(std::vector<std::byte> &inputBytes);
    std::string currentEndpointUrl;
    bool mIsReading;
    std::string checkURL(std::string url);
    int mBufferSize = 4096;
    std::string getStringResult(std::vector<char> &resultBuffer);
    int mPort = 80;
    std::string mEndpointAdr;
    std::string GetStringFromBytes(std::vector<std::byte> &inputBytes);
    asio::io_context context;
    std::vector<std::string> GetURLSubValues(std::string url);
};

#endif // HTTPCLIENT_HPP
