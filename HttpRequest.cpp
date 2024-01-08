#include "HttpRequest.hpp"

// Constructor to parse and initialize the request
HttpRequest::HttpRequest(const char* request):isGetAllHeaders(0), isGetFirstLine(0), isGetFullBody(0), body("")
{
    parse(request);
    // int index = 
    setQuery(this->resource.substr(this->resource.find('?') + 1));
    this->resource = this->resource.substr(0, this->resource.find('?'));
}

// Getters for request attributes
const std::string& HttpRequest::getMethod() const 
{
    return method;
}

const std::string& HttpRequest::getQuery() const
{
    return querystr;
}

const std::string& HttpRequest::getResource() const 
{
    return resource;
}

const std::map<std::string, std::string>& HttpRequest::getHeaders() const 
{
    return headers;
}

const std::string&  HttpRequest::getBody() const
{
    return body;
}

const std::string&  HttpRequest::getHttpVersion() const
{
    return httpVersion;
}

void    HttpRequest::setQuery( std::string qstr)
{
    querystr = qstr;
}

// Parse the HTTP request
static  std::string strtrim(const std::string& str) {
    size_t start = 0;
    size_t end = str.length();

    // Find the first non-whitespace character from the beginning
    while (start < end && std::isspace(str[start])) {
        ++start;
    }

    // Find the first non-whitespace character from the end
    while (end > start && std::isspace(str[end - 1])) {
        --end;
    }

    // Return the trimmed substring
    return str.substr(start, end - start);
}
void HttpRequest::parse(const char* request) 
{
    std::istringstream requestStream(request);
    std::string         line;

    // Parse the first line to get the method and resource
    if (!isGetFirstLine && std::getline(requestStream, line))
    {
        std::istringstream lineStream(line);
        lineStream >> method >> resource >> httpVersion;
        isGetFirstLine = 1;
    }


    // Parse headers
    int isR = 0;
    while (!isGetAllHeaders && !requestStream.eof() && std::getline(requestStream, line)) 
    {
        if (line == "\r" && line.size() == 1)
        {
            isGetAllHeaders = 1;
            break;
        }
        size_t separatorPos = line.find(':');
        if (isR != 2 && separatorPos != std::string::npos)
        {
            std::string key = line.substr(0, separatorPos);
            std::string value = line.substr(separatorPos + 1);
            //std::cout << key << value << std::endl;
            headers[strtrim(key)] = strtrim(value);
        }

        // else if (isR != 2 && line == "\r")
        //         isR++;
        // else if (!isfline)
        // {
        //     body += line;
        //     isfline = 1;
        // }
    }

    bool isfline = 0;
    while (!requestStream.eof() && std::getline(requestStream, line))
    {
        if (!isfline || requestStream.eof())
        {
            body += line;
            isfline = 1;
        }
        else body += "\n" + line;
    }
    
}

bool isInvalidURI(const std::string& uri) {
    // Check for control characters
    for (std::size_t i = 0; i < uri.length(); ++i) {
        if (std::iscntrl(uri[i])) {
            return true;
        }
    }

    // Check for unsafe characters
    const char* unsafeChars = "<>#%{}|\\^~[]`";
    if (strpbrk(uri.c_str(), unsafeChars) != NULL) {
        return true;
    }

    // Check for invalid percent encodings
    // for (std::size_t i = 0; i < uri.length() - 2; ++i) {
    //     if (uri[i] == '%' && !isxdigit(uri[i + 1]) && !isxdigit(uri[i + 2])) {
    //         return true;
    //     }
    // }

    // Check for invalid path or file name characters
    const char* invalidPathChars = ":*?\"<>|";
    if (strpbrk(uri.c_str(), invalidPathChars) != NULL) {
        return true;
    }

    // Add more checks as needed...

    
    return false;
}

int HttpRequest::isRequestWellFormed(const int &clientSocket, std::deque<server> &server)
{
        const char* response = NULL;

         // check if the request uri has more then 2048 chars
        if (getResource().size() > 2048)
        {
            response = "HTTP/1.1 414 Request-URI Too Long\r\nContent-Type: text/html\r\n\r\n<html><body><h1>414 Request-URI Too Long</h1></body></html>";
        }
        // Check the Transfer-Encoding header
        std::map<std::string, std::string>::const_iterator transferEncodingIt = getHeaders().find("Transfer-Encoding");
        if (!response && transferEncodingIt != getHeaders().end() && transferEncodingIt->second != "chunked")
        {
            response = "HTTP/1.1 501 Not Implemented\r\nContent-Type: text/html\r\n\r\n<html><body><h1>501 Not Implemented</h1><p></body></html>";
        }
        if (!response && getMethod() == "POST")
        {
            std::map<std::string, std::string>::const_iterator ContentLength = getHeaders().find("Content-Length");
            if (transferEncodingIt == getHeaders().end() && ContentLength == getHeaders().end())
                response = "HTTP/1.1 404 Not Implemented\r\nContent-Type: text/html\r\n\r\n<html><body><h1>404 Bad Request</h1></body></html>";
        }
        //check for non allowed characters in uri?
        if (!response && isInvalidURI(getResource()))
        {
            response = "HTTP/1.1 404 Bad Request\r\nContent-Type: text/html\r\n\r\n<html><body><h1>404 Bad Request</h1></body></html>";
        }
       
        // check if the body is larger then then max_client_body_size
        for (size_t i = 0; i < server.size(); i++)
        {
            if (getBody().size() > server[i].mx_cl_bd_size)
            {
                response = "HTTP/1.1 413 Request Entity Too Large\r\nContent-Type: text/html\r\n\r\n<html><body><h1>413 Request Entity Too Large</h1></body></html>";
            }
        }
        // check if the method is allowed
        if (response)
        {
            ssize_t bytesSent = send(clientSocket, response, strlen(response), 0);
            if (bytesSent == -1)
                std::cerr << "Error sending response\n";
            return 0;
        }
        return 1;
}

void    HttpRequest::setIsGetFullBody()
{
    isGetFullBody = 1;
}