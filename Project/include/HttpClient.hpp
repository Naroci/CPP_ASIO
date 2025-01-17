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

    HttpContentData ExtractData(std::vector<char> &load);

    // Main Test Function.
    std::string DownloadString(std::string url, int port);
    std::vector<char> DownloadByteData(std::string url, int port);

    // Getters & Setters.
    void SetBufferSize(int size) { mBufferSize = size; };
    int GetBufferSize() const { return mBufferSize; }

    std::vector<char> ReadBytesFromSocket();
    HttpContentData ReadFromSocket();

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
    asio::error_code m_errorCode;
    RequestHeaderBuilder mHeaderBuilder;
    asio::ip::basic_resolver_results<asio::ip::tcp> m_target_endpoints;

    std::string checkURL(std::string url);
    std::string getStringResult(std::vector<char> &resultBuffer);

    std::string GetStringFromBytes(std::vector<std::byte> &inputBytes);
    std::vector<char> GetCharContentFromBytes(std::vector<std::byte> &inputBytes);

    bool connectClient(std::string url, int port,
                               RequestHeaderBuilder::RequestType type);

    bool disconnectClient();


    asio::ip::tcp::socket *getSocket() {return m_socket;};

    asio::ip::tcp::socket *m_socket;
    void setSocket(asio::ip::tcp::socket *Socket) { m_socket = Socket; };

};

#endif // HTTPCLIENT_HPP
