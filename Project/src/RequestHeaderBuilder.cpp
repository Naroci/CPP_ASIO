#include <iostream>
#include <list>
#include <string>
#include <vector>

#include "../include/RequestHeaderBuilder.hpp"

const std::string HTTP_STANDARD_ATTRIBUTE = "HTTP/1.1\r\n";
const std::string REQUEST_DEFAULT_INDEXFILE = "/index.html";

const std::string REQUEST_KEYWORD_CONNECTION = "Connection:";
const std::string REQUEST_KEYWORD_HOST = "Host:";

// Standard
const std::string REQUEST_CONNECTION_ATTRIBUTE_CLOSE = "close";
const std::string REQUEST_CONNECTION_ATTRIBUTE_KEEP_ALIVE = "keep-alive";
/*
Connection: keep-alive
Keep-Alive: timeout=5, max=100
*/
const std::string REQUEST_CONNECTION_ATTRIBUTE_UPGRADE = "upgrade";
/*
Connection: upgrade
Upgrade: websocket
*/

const std::string REQUEST_KEYWORD_GET = "GET";
const std::string REQUEST_KEYWORD_POST = "POST";
const std::string REQUEST_KEYWORD_PUT = "PUT";
const std::string REQUEST_KEYWORD_PATCH = "PATCH";
const std::string REQUEST_KEYWORD_DELETE = "DELETE";
// Add other methods later too...

std::string RequestHeaderBuilder::CreateHeaderRequest(std::string UrlRequest,
                                                      RequestType requestType)
{
    std::vector<std::string>  parsedUrl = ParseUrl(UrlRequest);
    
    std::string host = parsedUrl.size() > 0 ? parsedUrl[0] : UrlRequest;

    std::string subReq = GetSubrequest(host,UrlRequest);
    std::string subvalueRequest = subReq.empty() ? REQUEST_DEFAULT_INDEXFILE :  subReq;
    std::string requTypeString = GetStringValueBasedOnRequestType(requestType);
    std::string returnValue  = requTypeString + " "+ subvalueRequest +" " +  HTTP_STANDARD_ATTRIBUTE
                                    + REQUEST_KEYWORD_HOST + " " + host + "\r\n"
                                    + REQUEST_KEYWORD_CONNECTION + " " + REQUEST_CONNECTION_ATTRIBUTE_CLOSE
                                    + "\r\n\r\n";

    return returnValue;
    /*
    std::string requestHeader = "GET /index.html HTTP/1.1\r\n"
                                    "Host: " +
                                    HttpClient::currentEndpointUrl +
                                    "\r\n"
                                    "Connection: close\r\n\r\n";
    return "";
    */
}

std::string RequestHeaderBuilder::GetSubrequest(std::string BaseAddress, std::string RequestUrl)
{
    if (BaseAddress == RequestUrl)
        return "";
    
    std::string returnValue;
    int pos = RequestUrl.find(BaseAddress);
    if (pos > -1)
    {
        returnValue = RequestUrl.substr(pos + BaseAddress.size());
    }
    return returnValue;
}

std::string
RequestHeaderBuilder::GetStringValueBasedOnRequestType(RequestType requestType)
{
    switch (requestType)
    {
    case RequestType::GET:
        return REQUEST_KEYWORD_GET;
        break;
    case RequestType::POST:
        return REQUEST_KEYWORD_GET;
        break;
    case RequestType::PUT:
        return REQUEST_KEYWORD_GET;
        break;
    case RequestType::PATCH:
        return REQUEST_KEYWORD_GET;
        break;
    case RequestType::DELETE:
        return REQUEST_KEYWORD_GET;
        break;
    default:
        return REQUEST_KEYWORD_GET;
        break;
    }

    return "";
}

std::vector<std::string> RequestHeaderBuilder::ParseUrl(std::string url)
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