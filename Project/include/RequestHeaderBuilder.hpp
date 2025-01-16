#pragma once
#ifndef REQUESTHEADERBUILDER_HPP
#define REQUESTHEADERBUILDER_HPP
#include <string>
#include <vector>

class RequestHeaderBuilder
{
  public:
    enum RequestType{
        GET,
        POST,
        PUT,
        PATCH,
        DELETE,
        HEAD,
        OPTIONS,
        TRACE,
        CONNECT
    };
    RequestHeaderBuilder() {}
    std::string CreateHeaderRequest(std::string UrlRequest, RequestType requestType);
    std::vector<std::string> ParseUrl(std::string url);
  private:
    std::string GetStringValueBasedOnRequestType(RequestType requestType);
    std::string GetSubrequest(std::string BaseAddress, std::string RequestUrl);
};

#endif // REQUESTHEADERBUILDER_HPP
