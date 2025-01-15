//
// Created by naroci on 13.10.24.
//

#ifndef HTTPCLIENT_HPP
#define HTTPCLIENT_HPP
#include <string>
#include <vector>
#include <asio/ip/tcp.hpp>

#endif //HTTPCLIENT_HPP

class HttpClient
{
    public :
        HttpClient()
        {
          
        }

        // Main Test Function.
        std::string DownloadString(std::string url, int port);

        // Getters &  Setters.
        void SetBufferSize(int size) { mBufferSize = size;};
        int GetBufferSize() const { return mBufferSize;}

        std::string ReadFromSocket(asio::ip::tcp::socket &sock, asio::error_code &ec);

        std::string GetEndpointAddress() const { return mEndpointAdr;}

        asio::ip::basic_resolver_results<asio::ip::tcp> GetEndpointsFromString(std::string StringUrl, int port);

        int GetPort() const { return mPort;}
        bool GetIfReading() const { return mIsReading;}

    private:
        std::string currentEndpointUrl;
        bool mIsReading;
        std::string checkURL (std::string url);
        int mBufferSize = 4096;
        std::string getStringResult(std::vector<char> &resultBuffer);
        int mPort = 80;
        std::string mEndpointAdr;
        asio::io_context context;
};
