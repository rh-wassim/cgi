#ifndef HTTPREQUEST_HPP
#define HTTPREQUEST_HPP

#include "Config.hpp"

//#include <iostream>
#include <deque>
#include <map>
#include <string>
//#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
#include <sstream>
#include <vector>
#include <cstdlib>
#include <sys/stat.h>
#include <filesystem>
#include <dirent.h>
#include <stdexcept>
#include <fstream>
#include <fcntl.h>

class HttpRequest {
    public:
        // Constructor to parse and initialize the request
        HttpRequest(){}
        HttpRequest(const char* request);

        // Getters for request attributes
        const std::string&                          getMethod() const;

        const std::string&                          getResource() const;

        const std::map<std::string, std::string>&   getHeaders() const;
        const std::string&                          getBody() const;
        const std::string&                          getHttpVersion() const;
        void                                        setIsGetFullBody();
        void                                        setQuery( std::string );
        const std::string&                          getQuery( void ) const ;
        int                                         isRequestWellFormed(const int &clientSocket, std::deque<server> &server);

    private:
        bool                                        isGetAllHeaders;
        bool                                        isGetFirstLine;
        bool                                        isGetFullBody;        
        std::string                                 body;
        std::string                                 method;
        std::string                                 resource;
        std::map<std::string, std::string>          headers;
        std::string                                 httpVersion;
        std::string                                 querystr;

        // Parse the HTTP request
        void                                        parse(const char* request);

};

#endif