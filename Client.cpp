#include "Client.hpp"
#include <cerrno>    // for errno

void    printRequest(const char *buffer)
{
    int i = 0;
    //std::cout << "----------------------------------------------------\n";
    while (buffer && buffer[i])
    {
        if (buffer[i] == '\r')
            std::cout << "\\r";
        else if (buffer[i] == '\n')
            std::cout << "\\n";
        else 
            std::cout << buffer[i];
        i++;
    }
   // std::cout << "----------------------------------------------------\n";
}
int Client::getcontentLength()
{
    return contentLength;
}
void printParsedRequest(HttpRequest &request)
{
        std::cout << "======================================================\n";

// Print parsed attributes
        std::cout << "Method:|" << request.getMethod() << "|" << std::endl;
        std::cout << "Resource:|" << request.getResource() << "|" << std::endl;
        std::cout << "HttpVersion:|" << request.getHttpVersion() << "|" << std::endl;

        const std::map<std::string, std::string>& headers = request.getHeaders();
        for (std::map<std::string, std::string>::const_iterator it = headers.begin(); it != headers.end(); ++it) 
        {
            std::cout << it->first<< it->second << std::endl;
        }
        std::cout << "Body:|" << request.getBody() << std::endl;
        std::cout << "======================================================\n";
}
std::string Client::getExtensionByContentType(const std::string& contentType) 
{
    typedef std::map<std::string, std::string> ContentTypeToExtensionMap;
    ContentTypeToExtensionMap contentTypeToExtension;

   contentTypeToExtension["text/html"] = ".html";
        contentTypeToExtension["text/css"] = ".css";
        contentTypeToExtension["application/javascript"] = ".js";
        contentTypeToExtension["image/jpeg"] = ".jpeg";
        contentTypeToExtension["image/png"] = ".png";
        contentTypeToExtension["image/gif"] = ".gif";
        contentTypeToExtension["video/mp4"] = ".mp4";
        contentTypeToExtension["application/pdf"] = ".pdf";
        contentTypeToExtension["text/plain"] = ".txt";
        contentTypeToExtension["application/xml"] = ".xml";
        contentTypeToExtension["application/json"] = ".json";
        contentTypeToExtension["text/csv"] = ".csv";
        contentTypeToExtension["application/msword"] = ".doc";
        contentTypeToExtension["application/vnd.openxmlformats-officedocument.wordprocessingml.document"] = ".docx";
        contentTypeToExtension["application/vnd.ms-excel"] = ".xls";
        contentTypeToExtension["application/vnd.openxmlformats-officedocument.spreadsheetml.sheet"] = ".xlsx";
        contentTypeToExtension["application/vnd.ms-powerpoint"] = ".ppt";
        contentTypeToExtension["application/vnd.openxmlformats-officedocument.presentationml.presentation"] = ".pptx";
        contentTypeToExtension["application/zip"] = ".zip";
        contentTypeToExtension["application/x-tar"] = ".tar";
        contentTypeToExtension["application/gzip"] = ".gz";
        contentTypeToExtension["audio/mpeg"] = ".mp3";
        contentTypeToExtension["audio/ogg"] = ".ogg";
        contentTypeToExtension["font/woff"] = ".woff";
        contentTypeToExtension["font/woff2"] = ".woff2";
        contentTypeToExtension["text/x-c"] = ".cpp";
        contentTypeToExtension["text/x-java-source"] = ".java";
        contentTypeToExtension["application/x-yaml"] = ".yml";
        contentTypeToExtension["application/sql"] = ".sql";
        contentTypeToExtension["text/calendar"] = ".ics";
        contentTypeToExtension["text/php"] = ".php";
        contentTypeToExtension["image/x-icon"] = ".ico";
        contentTypeToExtension["image/svg+xml"] = ".svg";
        contentTypeToExtension["video/x-msvideo"] = ".avi";
        contentTypeToExtension["video/quicktime"] = ".mov";
        contentTypeToExtension["image/bmp"] = ".bmp";
        contentTypeToExtension["image/webp"] = ".webp";
        contentTypeToExtension["image/tiff"] = ".tiff";
        contentTypeToExtension["application/vnd.ms-excel.sheet.macroEnabled.12"] = ".ejs";
        contentTypeToExtension["video/mpeg"] = ".mpg";
        contentTypeToExtension["video/webm"] = ".webm";
        contentTypeToExtension["text/x-java-source"] = ".java";
        contentTypeToExtension["text/x-c++src"] = ".cpp";
        contentTypeToExtension["text/php"] = ".php";
        contentTypeToExtension["application/x-yaml"] = ".yml";
        contentTypeToExtension["application/sql"] = ".sql";
        contentTypeToExtension["text/calendar"] = ".ics";
        contentTypeToExtension["application/bat"] = ".bat";
        contentTypeToExtension["application/octet-stream"] = ".py";
        contentTypeToExtension["application/x-msdownload"] = ".dll";
        contentTypeToExtension["application/java-vm"] = ".class";
        contentTypeToExtension["image/vnd.adobe.photoshop"] = ".psd";
        contentTypeToExtension["audio/wav"] = ".wav";
        contentTypeToExtension["audio/flac"] = ".flac";
        contentTypeToExtension["application/x-httpd-php"] = ".php";
        
    std::string lowercaseContentType = toLowerCase(contentType);

    ContentTypeToExtensionMap::const_iterator it = contentTypeToExtension.find(lowercaseContentType);
    if (it != contentTypeToExtension.end()) {
        return it->second;
    } else {
        return "";
    }
}

Client::Client(int Socket, confi &cf):clientSocket(Socket), isGetFullReq(0), isSentFullRes(0), cfg(cf),isSetRequest(0),isFileCreated(0),isGetFullBody(0),bytes(0),isChunked(0)
{
    //bytes = 0;
    tmpFileName = "";
}
int    isReachedBody(char *buffer)
{
    int i = 0;

    while (buffer && buffer[i])
    {
        if (buffer[i] == '\r' && buffer[i + 1] == '\n' && buffer[i + 2] == '\r' && buffer[i + 3] == '\n')
            return (i + 4);
        ++i;
    }
    return 0;
}

void    Client::getRequest()
{ 
    if (!isGetFullReq)
    {
        int bodyBegin;
        std::cout << "get request from client on socket " << getClientSocket() << std::endl;
        
        memset(buffer,0, 1024);
        bytes = recv(clientSocket, buffer, 1023, 0);
        //std::cout << "bytes readed "<<  bytes << std::endl;
        if (bytes == -1) 
        {
            std::cerr << "Error receiving data\n";
            isSentFullRes = 1;
            isGetFullReq = 1;
            return;
        }
        else if (bytes > 0)
        {
            buffer[bytes] = '\0';
            requestBuffer += buffer;
        }
        bodyBegin = isReachedBody(buffer);
        if (bytes != 1023 || bodyBegin)
        {
            memset(bodyBuffer,0, sizeof(bodyBuffer));
            isGetFullReq = 1;
            int i = 0;
            while (bodyBegin < bytes)
            {
                bodyBuffer[i] = buffer[bodyBegin];
                ++bodyBegin;
                ++i;
            }
            bytes = i;
            bodyBuffer[i] = 0;
            
        }
        std::cout << "buffer " << buffer << std::endl;
    }
    else if (!isGetFullBody)
    {
        memset(bodyBuffer,0, sizeof(bodyBuffer));
        bytes = recv(clientSocket, bodyBuffer, 1023, 0);
       // std::cout << "bytes readed "<<  bytes << std::endl;
        //std::cout << "bytes readedfrom socket " << bytes << std::endl;
        if (bytes == -1) 
        {
            std::cerr << "Error receiving data\n";
            isSentFullRes = 1;
            isGetFullReq = 1;
            if (isFileCreated && inputfile.is_open())
                inputfile.close();

        }
        else if (bytes > 0)
        {
            bodyBuffer[bytes] = '\0';
        }
       // printRequest(bodyBuffer); 
    }
}

bool    Client::isRequestFinished()
{
    return isGetFullReq;
}
bool    Client::isResponseFinished()
{
    return isSentFullRes;
}
int    Client::getClientSocket()const{
    return clientSocket;
}
void    Client::closeClientSocket()
{
    close(clientSocket);
}
void    Client::setResponse()
{
    isSentFullRes = 1;
}

void    Client::setParsedRequest()
{
    HttpRequest req(requestBuffer.c_str());
    request = req;
    isSetRequest = 1;
    contentLength = 0;
    std::map<std::string, std::string>::const_iterator it = request.getHeaders().find("Content-Length");
    if (it != request.getHeaders().end() && (it->second).size())
    {
        contentLength = atoi((it->second).c_str());
    }

}

HttpRequest&    Client::getParsedRequest()
{
    return request;
}
Client::~Client(){}

void Client::handelread()
{
    getRequest();
    //std::cout << "bytes read" <<  bytes << std::endl;
    if (!isRequestFinished() || isResponseFinished()) 
            return;
    if (!isSetRequest)
    {
        setParsedRequest();
        isSetRequest = 1;
        setClientResponce();

    }
}
void    Client::handleClient()
{
        if (isResponseFinished())
        return;
        if (!isRequestFinished() && request.getMethod() != "POST")
            return;
        if(request.getMethod() == "POST")
            methodpost();
        else if(request.getMethod() == "DELETE" &&  isRequestFinished() == true)
        {
                methoddelete();
        }
        else if(request.getMethod() == "GET" && isRequestFinished() == true)
            methodGet();
       
}

void Client::setresponse(const std::string& status,const std::string& contentType,const std::string& content) 
 {
   
    response = status + "\r\n"
                   "Content-Type: " + contentType + "\r\n"
                   "Content-Length: " +std::to_string(content.size())+ "\r\n"
                   "\r\n" + content;
}

const char *Client::getresponse()
{
    return response.c_str();
}

void    Client::setClientResponce()
{
    isSentHreders = 0;
    methodAllowed = false;
    bestmatch = "none";
    bestMatchLength = 0;
    bestMatchLocation = NULL;
    matchinglocation();
}
const location*  Client::getbestMatchLocation()
{
    return  bestMatchLocation;
}
std::string Client::getFileExtension(const std::string& filePath) 
{
    size_t dotPosition = filePath.find_last_of('.');

    if (dotPosition != std::string::npos && dotPosition < filePath.length() - 1) {
        return "." + filePath.substr(dotPosition + 1);
    }
    return ".cpp";
}

bool Client::deleteResource(std::string fileName) 
{
     struct stat buffer;
     
    if (stat(fileName.c_str(), &buffer) == 0) {
                std::remove(fileName.c_str());
        return true;
    } 
    else 
    {
        std::cerr << fileName << "' not found." << std::endl;
        return false;
    }
}
 std::string Client::concatenatePaths(const std::string& path1, const std::string& path2) 
 {
            if (path1.empty() || path1[path1.size() - 1] == '/')
                return path1 + path2;
            else
                return path1 + '/' + path2;
}

std::string Client::toLowerCase(const std::string& str) 
{
    std::string result = str;
    for (size_t i = 0; i < result.size(); ++i) {
        result[i] = tolower(result[i]);
    }
    return result;
}
std::string Client::getContentTypeByExtension(const std::string& extension) 
{
    std::map<std::string, std::string> extensionToContentType;
    extensionToContentType[".html"] = "text/html";
        extensionToContentType[".css"] = "text/css";
        extensionToContentType[".js"] = "application/javascript";
        extensionToContentType[".jpeg"] = "image/jpeg";
        extensionToContentType[".png"] = "image/png";
        extensionToContentType[".gif"] = "image/gif";
        extensionToContentType[".mp4"] = "video/mp4";
        extensionToContentType[".pdf"] = "application/pdf";
        extensionToContentType[".txt"] = "text/plain";
        extensionToContentType[".xml"] = "application/xml";
        extensionToContentType[".json"] = "application/json";
        extensionToContentType[".csv"] = "text/csv";
        extensionToContentType[".doc"] = "application/msword";
        extensionToContentType[".docx"] = "application/vnd.openxmlformats-officedocument.wordprocessingml.document";
        extensionToContentType[".xls"] = "application/vnd.ms-excel";
        extensionToContentType[".xlsx"] = "application/vnd.openxmlformats-officedocument.spreadsheetml.sheet";
        extensionToContentType[".ppt"] = "application/vnd.ms-powerpoint";
        extensionToContentType[".pptx"] = "application/vnd.openxmlformats-officedocument.presentationml.presentation";
        extensionToContentType[".zip"] = "application/zip";
        extensionToContentType[".tar"] = "application/x-tar";
        extensionToContentType[".gz"] = "application/gzip";
        extensionToContentType[".mp3"] = "audio/mpeg";
        extensionToContentType[".ogg"] = "audio/ogg";
        extensionToContentType[".woff"] = "font/woff";
        extensionToContentType[".woff2"] = "font/woff2";
        extensionToContentType[".ico"] = "image/x-icon";
        extensionToContentType[".svg"] = "image/svg+xml";
        extensionToContentType[".avi"] = "video/x-msvideo";
        extensionToContentType[".mov"] = "video/quicktime";
        extensionToContentType[".bmp"] = "image/bmp";
        extensionToContentType[".webp"] = "image/webp";
        extensionToContentType[".tiff"] = "image/tiff";
        extensionToContentType[".ejs"] = "application/vnd.ms-excel.sheet.macroEnabled.12";
        extensionToContentType[".mpg"] = "video/mpeg";
        extensionToContentType[".webm"] = "video/webm";
        extensionToContentType[".ics"] = "text/calendar";
        extensionToContentType[".java"] = "text/x-java-source";
        extensionToContentType[".cpp"] = "text/x-c++src";
        extensionToContentType[".php"] = "text/php";
        extensionToContentType[".yml"] = "application/x-yaml";
        extensionToContentType[".sql"] = "application/sql";
        extensionToContentType[".log"] = "text/plain";
        extensionToContentType[".bat"] = "application/bat";
        extensionToContentType[".exe"] = "application/octet-stream";
        extensionToContentType[".dll"] = "application/x-msdownload";
        extensionToContentType[".class"] = "application/java-vm";
        extensionToContentType[".psd"] = "image/vnd.adobe.photoshop";
        extensionToContentType[".wav"] = "audio/wav";
        extensionToContentType[".flac"] = "audio/flac";

    std::string lowercaseExtension = toLowerCase(extension);

    std::map<std::string, std::string>::const_iterator it = extensionToContentType.find(lowercaseExtension);
    if (it != extensionToContentType.end()) {
        return it->second;
    } else {
        return "text/plain";
    }
}
void Client::split(const std::string& input, char delimiter, std::string& part1, std::string& part2) 
{
    std::size_t pos = input.find(delimiter);
    if (pos != std::string::npos) {
        part1 = input.substr(0, pos);
        part2 = input.substr(pos + 1);
    } 
    else 
    {
        part1 = input;
        part2 = "";
    }
}
std::string &Client::getbestmatch()
{
    return bestmatch;
}
std::string Client::getfilePath()
{
    size_t pos = request.getResource().find(getbestmatch());
    if(pos !=std::string::npos)
    {
        return filePath = getbestMatchLocation()->root + request.getResource().substr(pos + getbestmatch().length());
    }
    return "error";
}               


bool Client::sendHredes()
{
    if (isSentHreders)
        return 1;
    fileStream.open(getfilePath(), std::ios::binary);
   
    fileStream.seekg(0, std::ios::end);
    size_t             file_size = fileStream.tellg();
    fileStream.seekg(0, std::ios::beg);
    std::string     fileContent;
    if(fileStream.is_open())
    {
        std::string fileExtension = getFileExtension(filePath);
        std::string contentType =getContentTypeByExtension(fileExtension);
        fileContent = "HTTP/1.1 200 OK";
        fileContent += "\r\n"
        "Content-Type: " + contentType + "\r\n"
        "Content-Length: " + std::to_string(file_size) + "\r\n\r\n";
        send(getClientSocket(), fileContent.c_str(), fileContent.size(), 0);
        fileStream.close();
        fileDescriptor = open(getfilePath().c_str(), O_RDONLY | O_NONBLOCK);
    }
    else 
    {
        setResponse();
    }
    isSentHreders = 1;
    return 0;

}

void Client::methodGetFile()
{
   int bufferSize = 1024;
    char buffer[bufferSize];
    memset(buffer, 0, bufferSize);
    if(sendHredes())
    {  
        int bytes = read(fileDescriptor,buffer, bufferSize - 1);
        if (bytes == -1  || bytes == 0)
        {
            close(fileDescriptor);
            setResponse();
            return;
        }
        buffer[bytes] = '\0';
        ssize_t sentBytes= send(getClientSocket(), buffer, bytes, 0);
        if (sentBytes < 0)
        {
            perror("send failed");
            close(fileDescriptor);
            setResponse();
            return;
        }
    }                   
}
bool Client::sendHredes2(std::string  fullPath)
{
    
    if (isSentHreders)
        return 1;
    fileStream.open(fullPath, std::ios::binary);
    fileStream.seekg(0, std::ios::end);
    size_t             file_size = fileStream.tellg();
    fileStream.seekg(0, std::ios::beg);
    std::string     fileContent;
    if(fileStream.is_open())
    {
        //printf("im in header\n");
        std::string fileExtension = getFileExtension(fullPath);
        std::string contentType =getContentTypeByExtension(fileExtension);
        fileContent = "HTTP/1.1 200 OK";
        fileContent += "\r\n"
        "Content-Type: " + contentType + "\r\n"
        "Content-Length: " + std::to_string(file_size) + "\r\n\r\n";
        send(getClientSocket(), fileContent.c_str(), fileContent.size(), 0);
       // printf("%s\n",fileContent.c_str());
        fileStream.close();
        fileDescriptor = open(fullPath.c_str(), O_RDONLY | O_NONBLOCK);
    }
    else 
    {
        setResponse();
    }
    isSentHreders = 1;
    return 0;

}
void Client::handleDirectoryAutoIndex()
{
     if(request.getResource().back() != '/')
    {
        std::string other =request.getResource() + "/";
        setresponse("HTTP/1.1 301 Moved Permanently", "text/html", "<!DOCTYPE HTML PUBLIC \"-//IETF//DTD HTML 2.0//EN\">\n<html><head><title>301 Moved Permanently</title></head><body><h1>Moved Permanently</h1><p>The document has moved <a href='http://localhost:8080" + other + "'>here</a>.</p></body></html>");
        send(getClientSocket(),getresponse(),strlen(getresponse()),0);      
        setResponse();
        return;         
    }
    else 
    {
        std::vector<std::string> filelist;
         DIR* directory = opendir(filePath.c_str());
        if (directory != NULL)
        {
            std::string fullPath = filePath  + getbestMatchLocation()->index;
            if (!getbestMatchLocation()->index.empty() && access(fullPath.c_str(), F_OK) != -1)
            {  
                closedir(directory);
                if(getbestMatchLocation()->cgi =="off")
                {
                    int bufferSize = 1024;
                    char buffer[bufferSize];
                    memset(buffer, 0, bufferSize);
                    if(sendHredes2(fullPath))
                    {  
                    int bytes = read(fileDescriptor,buffer, bufferSize - 1);
                    if (bytes == -1  || bytes == 0)
                    {
                            close(fileDescriptor);
                            setResponse();
                            return;
                    }
                    buffer[bytes] = '\0';
                    send(getClientSocket(), buffer, bytes, 0);
                    }
                    else if(getbestMatchLocation()->cgi =="on")
                    {
                                
                    }
                }
            }
            else
            {
            if(getbestMatchLocation()->auto_index == "on")
            {
                struct dirent* entry;
                while ((entry = readdir(directory)) != NULL)
                {
                    filelist.push_back(entry->d_name);
                }
                closedir(directory);
                std::string htmlContent = "<!DOCTYPE html>\n<html>\n<head><title>Index of " + request.getResource() + "</title></head>\n<body>\n<h1>Index of " + request.getResource() + "</h1>\n<ul>\n";
                std::vector <std::string>::const_iterator file;
                std::string fileContent;
                for (file = filelist.begin(); file != filelist.end(); ++file) 
                {
                    const std::string fileLink =concatenatePaths(request.getResource(),*file);
                    std::ifstream fileStream(fileLink.c_str());
                    std::stringstream buffer;
                    buffer << fileStream.rdbuf();
                    fileContent = buffer.str();
                    htmlContent += "<li><a href=\"" + fileLink + "\">" + *file +"</a></li>\n";
                }
                setresponse("HTTP/1.1 200 OK", "text/html",htmlContent);
                send(getClientSocket(), getresponse(), strlen(getresponse()), 0);
                setResponse(); 
            }
            else if(getbestMatchLocation()->auto_index == "off")
            {
                setresponse("HTTP/1.1 403 Forbidden","text/html","<!DOCTYPE HTML PUBLIC -//IETF//DTD HTML 2.0//EN><html><head><title>403 Forbidden</title></head><body><h1>Forbidden</h1><p>You don t have permission to access /forbidden/index.html on this server.</p></body></html>");
                send(getClientSocket(),getresponse(),strlen(getresponse()),0);
                setResponse();
            }
            }
        }
         else 
            notfound();
    }
}
void Client::methodGet()
{
    struct stat fileinfo;
    std::cout << "check URL : " << this->request.getResource() << std::endl;
    if (stat(getfilePath().c_str(), &fileinfo) == 0) 
    {
        if (S_ISREG(fileinfo.st_mode)) {
            printf("im heree\n");
            if (getbestMatchLocation()->cgi == "off")
            {
                printf("there is file\n");
                methodGetFile();
            }
             else if (getbestMatchLocation()->cgi == "on") 
            {
                handle_cgi(getfilePath(), getFileExtension(getfilePath()), *this);
                this->setResponse();
            }
        } 
        else if (S_ISDIR(fileinfo.st_mode)) 
            handleDirectoryAutoIndex();
    } 
    else
        notfound();
}
void Client::notfound()
{
    if (!handleErrorResponse("404")) 
    {
        setresponse("HTTP/1.1 404 Not Found", "text/html", "<!DOCTYPE HTML PUBLIC \"-//IETF//DTD HTML 2.0//EN\">\r\n<html><head>\r\n<title>404 Not Found</title>\r\n</head><body>\r\n<h1>Not Found</h1>\r\n<p>The requested URL was not found on this server.</p>\r\n</body></html>\r\n");
        send(getClientSocket(), getresponse(), strlen(getresponse()), 0);
        setResponse();
        return;
    }
}
bool Client::handleErrorResponse(const std::string& errorCode)
{
        std::map<std::string, std::string>& err_pages = err_page;

        if (err_page.find(errorCode) != err_pages.end()) 
        {
            std::string concatenatedBuffer;
            std::ifstream file(err_page.find(errorCode)->second.c_str(), std::ios::binary);

            if (file.is_open()) {
                const int bufferSize = 1024;
                char buffer[bufferSize];

                while (file.read(buffer, bufferSize)) {
                    concatenatedBuffer.append(buffer, file.gcount());
                }
                if (file.eof()) {
                    send(getClientSocket(), concatenatedBuffer.c_str(), concatenatedBuffer.length(), 0);
                    setResponse();
                    file.close();
                    return true; 
                }
            }
        }
        return false;
}
void Client::methodnotallow()
{
     if(!handleErrorResponse("405"))
    {
        setresponse("HTTP/1.1 405 Method Not Allowed", "text/html", "<html><head><title>405 Method Not Allowed</title></head><body><h1>Method Not Allowed</h1><p>The requested method is not allowed for the requested URL.</p></body></html>");
        send(getClientSocket(), getresponse(), strlen(getresponse()), 0);
        setResponse();
        return;  
    } 
}
 void Client::matchinglocation()
{
         if (request.getHeaders().find("Host") != request.getHeaders().end())
         {
            std::string key, value;
            split(request.getHeaders().find("Host")->second, ':', key, value);   
             for (std::deque<server>::iterator iit = cfg.servers.begin(); iit != cfg.servers.end() ; iit++)
             {  
                if(key =="localhost")
                {
                     key = "127.0.0.1";
                }
                if((*iit).port == std::stoi(value) && ((*iit).host.compare(key)) == 0)
                {                    
                    err_page = (*iit).err_pages;
                   // if(getcontentLength() >(*iit).mx_cl_bd_size )

                    
                    for (std::deque<location>::const_iterator it = (*iit).locations.begin(); it != (*iit).locations.end(); ++it) 
                    {
                            if(request.getResource().find((*it).path_location) == 0)
                            {
                                if ((*it).path_location.length() >= bestMatchLength) 
                                {
                                    printf("im here\n");
                                    bestmatch = (*it).path_location;
                                    bestMatchLocation = &(*it);
                                    bestMatchLength = (*it).path_location.length();
                                }
                            }
                    }
                   // std::cout <<"bestmatch is " <<bestmatch << std::endl;
                    if(bestmatch == "none")
                    {
                        printf("none\n");
                        notfound();
                    }
                }
            }
         }
        if(bestMatchLocation)
        {
            if( !bestMatchLocation->redirection.empty())
            {
                if(!handleErrorResponse("301"))  
                {
                    setresponse("HTTP/1.1 301 Moved Permanently","text/html","<!DOCTYPE HTML PUBLIC -//IETF//DTD HTML 2.0//EN><html><head><title>301 Moved Permanently</title></head><body><h1>Moved Permanently</h1><p>The document has moved <a href=http://"+ request.getHeaders().find("Host")->second +">here</a>.</p></body></html>");  
                    send(getClientSocket(),getresponse(),strlen(getresponse()),0);
                    setResponse();
                    return;
                }  
            }
            const std::string& requestMethod = request.getMethod();
            std::cout << request.getMethod() << std::endl;
            for (std::deque<std::string>::const_iterator methodIt = (*bestMatchLocation).allow_methods.begin(); methodIt != bestMatchLocation->allow_methods.end(); ++methodIt)
            {
                if (*methodIt == requestMethod)
                {
                    methodAllowed = true;
                    break;
                }
            }  
            if (!methodAllowed)
                methodnotallow();
        }
}

bool    Client::createFile()
{
    if (isFileCreated)
        return true;
    if (!bytes)
    {
        setResponse();
        return false;
    }
   // printParsedRequest(request);
    std::map<std::string, std::string>::const_iterator it = request.getHeaders().find("Transfer-Encoding");
    if (it != request.getHeaders().end() && it->second == "chunked")
    {
        isChunked = true;
        std::stringstream ss;
        ss << "tmp/tmp" << getClientSocket();
        tmpFileName = ss.str();
       // std::cout << "tmp file name: "<< tmpFileName <<"\n";
        tmpInputfile.open(tmpFileName, std::ios::binary);
        if(!tmpInputfile.is_open())
        {
            std::cerr <<"Error opening the temperary file writing " <<  strerror(errno) << std::endl;
            setResponse();
            setresponse("HTTP/1.1 201 Created", "text/html", "<!DOCTYPE HTML PUBLIC \"-//IETF//DTD HTML 2.0//EN\">\r\n<html><head>\r\n<title>201 Created</title>\r\n</head><body>\r\n<h1>Created</h1>\r\n<p>Error opening the file.</p>\r\n</body></html>\r\n");
            send(getClientSocket(),getresponse(),strlen(getresponse()),0);
            isGetFullBody = 1;
            return false;
        }
        tmpInputfile.write(bodyBuffer, bytes);
        totalReaded = bytes;
        std::string fileExtension = getExtensionByContentType(request.getHeaders().find("Content-Type")->second);
        fileNamePost = getbestMatchLocation()->root + "/example" + fileExtension;

        inputfile.open(getbestMatchLocation()->root + "/example" + fileExtension, std::ios::binary);
        if(!inputfile.is_open())
        {
            std::cerr <<"Error opening the file" << std::endl;

            setResponse();
            setresponse("HTTP/1.1 201 Created", "text/html", "<!DOCTYPE HTML PUBLIC \"-//IETF//DTD HTML 2.0//EN\">\r\n<html><head>\r\n<title>201 Created</title>\r\n</head><body>\r\n<h1>Created</h1>\r\n<p>Error opening the file.</p>\r\n</body></html>\r\n");
            send(getClientSocket(),getresponse(),strlen(getresponse()),0);
            return false;
        }
        isFileCreated = true;
        return false;
    }
    std::string fileExtension = getExtensionByContentType(request.getHeaders().find("Content-Type")->second);
    std::cout << "check type : " << request.getHeaders().find("Content-Type")->second << std::endl;
    fileNamePost = getbestMatchLocation()->root + "/example" + fileExtension;
    std::cout <<"in creat file"<< fileNamePost << std::endl;
    inputfile.open(getbestMatchLocation()->root + "/example" + fileExtension, std::ios::binary);
    if(!inputfile.is_open())
    {
        std::cerr <<"Error opening the file" << std::endl;
        setResponse();
        setresponse("HTTP/1.1 201 Created", "text/html", "<!DOCTYPE HTML PUBLIC \"-//IETF//DTD HTML 2.0//EN\">\r\n<html><head>\r\n<title>201 Created</title>\r\n</head><body>\r\n<h1>Created</h1>\r\n<p>Error opening the file.</p>\r\n</body></html>\r\n");
        send(getClientSocket(),getresponse(),strlen(getresponse()),0);
        return false;
    }
   inputfile.write(bodyBuffer, bytes);
   totalReaded = bytes;
    isFileCreated = true;
    return false;
}

std::string Client::getFileNamePost()
{
    return fileNamePost;
}

bool    isHexadecimal(char c)
{
    char str[] = "0123456789abcdefABCDEF";
    int i = 0;
    while (str[i])
    {
        if (c == str[i])
        {//std::cout << str[i];
                return true;
        }
        ++i;
    }
    return false;
}

int hexToDecimal(const std::string& hexString) 
{
    int decimalValue = 0;

    // Ensure the input is not empty
    if (hexString.empty()) {
        //std::cerr << "Error: Empty hexadecimal string." << std::endl;
        return 0;
    }

    // Check if the hex string starts with "0x" and adjust the starting position accordingly
    size_t start = 0;
    if (hexString.size() >= 2 && hexString.substr(0, 2) == "0x") {
        start = 2;
    }

    // Iterate through each character in the string
    for (size_t i = start; i < hexString.size(); ++i) {
        decimalValue *= 16;  // Multiply the current value by 16

        // Convert hex character to decimal
        if (hexString[i] >= '0' && hexString[i] <= '9') {
            decimalValue += hexString[i] - '0';
        } else if (hexString[i] >= 'A' && hexString[i] <= 'F') {
            decimalValue += hexString[i] - 'A' + 10;
        } else if (hexString[i] >= 'a' && hexString[i] <= 'f') {
            decimalValue += hexString[i] - 'a' + 10;
        } else {
            //std::cerr << "Error: Invalid hexadecimal character '" << hexString[i] << "'." << std::endl;
            return 0;
        }
    }

    return decimalValue;
}

void Client::parseChunked()
{
    int     chunkSize = 0;
    bool     isFirstLine = 0;
    // int     total = 0;
    memset(buffer,0, sizeof(buffer));
    tmpInputfile2.open(tmpFileName, std::ios::binary);
    if(!tmpInputfile2.is_open())
    {
        std::cerr <<"Error opening the temperary file for reading " <<  strerror(errno) << std::endl;
        setResponse();
        setresponse("HTTP/1.1 201 Created", "text/html", "<!DOCTYPE HTML PUBLIC \"-//IETF//DTD HTML 2.0//EN\">\r\n<html><head>\r\n<title>201 Created</title>\r\n</head><body>\r\n<h1>Created</h1>\r\n<p>Error opening the file.</p>\r\n</body></html>\r\n");
        send(getClientSocket(),getresponse(),strlen(getresponse()),0);
        isGetFullBody = 1;////fchek
        return;
    }
    while (!tmpInputfile2.eof())
    {
        memset(buffer,0, sizeof(buffer));
        if (!chunkSize && !isFirstLine)
        {
            
            std::string chunkSizeLine;
            tmpInputfile2.read(buffer, 1);
            while (isHexadecimal(buffer[0]))
            {
                chunkSizeLine += buffer[0];
                tmpInputfile2.read(buffer, 1);
            }
            if (buffer[0] == '\r')
                tmpInputfile2.read(buffer, 1);
            // else
            //     std::cout << "i didnt fine \\r but i found " << buffer[0]<< std::endl;
            // if (buffer[0] == '\n')
            //     tmpInputfile2.read(buffer, 1);
            // else
            //     std::cout << "i didnt fine \\n but i found " << buffer[0]<< std::endl;
            //std::getline(tmpInputfile2, chunkSizeLine);
            chunkSize = hexToDecimal(chunkSizeLine);
            isFirstLine = 1;
            //std::cout << "--------------geting the first chunksize " << chunkSize <<"\n";
            memset(buffer,0, sizeof(buffer));
        }
        else if (!chunkSize && isFirstLine)
        {
            std::string chunkSizeLine;
            tmpInputfile2.read(buffer, 1);
            if (buffer[0] == '\r')
                tmpInputfile2.read(buffer, 1);
            // else
            //     std::cout << "i didnt fine \\r but i found " << buffer[0]<< std::endl;
            if (buffer[0] == '\n')
                tmpInputfile2.read(buffer, 1);
            // else
            //     std::cout << "i didnt fine \\n but i found " << buffer[0]<< std::endl;
            while (isHexadecimal(buffer[0]))
            {
                chunkSizeLine += buffer[0];
                tmpInputfile2.read(buffer, 1);
            }
            if (buffer[0] == '\r')
                tmpInputfile2.read(buffer, 1);
            // if (buffer[0] == '\n')
            //     tmpInputfile2.read(buffer, 1);
            chunkSize = hexToDecimal(chunkSizeLine);
            memset(buffer,0, sizeof(buffer));
            //std::cout << "--------------geting the chunksize " << chunkSize <<"\n";
        }
        // if (chunkSize <= 0)
        //     break;
        if (chunkSize > 1024)
        {
            tmpInputfile2.read(buffer, 1024);
            std::streamsize bytesRead = tmpInputfile2.gcount();
            //std::cout << "bytesRead " << bytesRead << std::endl;
            if (bytesRead > 0)
                inputfile.write(buffer, bytesRead);
            chunkSize = chunkSize - bytesRead;
            //std::cout << "left of the size " << chunkSize <<'\n';    
            // total += bytesRead;
        }
        else
        {
            tmpInputfile2.read(buffer, chunkSize);
            std::streamsize bytesRead = tmpInputfile2.gcount();
            if (bytesRead > 0)
                inputfile.write(buffer, bytesRead);
            // total += bytesRead;
            
            //printRequest(buffer);
            chunkSize = 0;
        }
    }
   // std::cout << "total size writen " << total << std::endl;
}

void Client::methodpost()
{
   // std::cout << "uri: " << request.getResource() << std::endl;
     if(getbestMatchLocation()->auto_upload =="on")
    {      
        if (createFile() && !isGetFullBody)
        {
            //printf("uuuuuuuu\n ");
            if (isChunked)
            {
                //std::cout << "tmp file name: "<< tmpFileName <<"\n";
                tmpInputfile.write(bodyBuffer, bytes);
                totalReaded += bytes;
                if(contentLength < totalReaded)
                {
                    // std::cout << "contentLength " << contentLength << std::endl;
                    // std::cout << "total bytes readedfrom socket " << totalReaded << std::endl;
                    tmpInputfile.close(); 
                    parseChunked();
                    if (getbestMatchLocation()->cgi == "on") 
                    {
                       
                         inputfile.close();
                        handle_cgi(getfilePath(), getFileExtension(getfilePath()), *this);
                        this->setResponse();
                        isGetFullBody = 1;
                        return;
                    }
                    setresponse("HTTP/1.1 201 Created", "text/html", "<!DOCTYPE HTML PUBLIC \"-//IETF//DTD HTML 2.0//EN\">\r\n<html><head>\r\n<title>201 Created</title>\r\n</head><body>\r\n<h1>Created</h1>\r\n<p>The resource has been successfully created.</p>\r\n</body></html>\r\n");
                    send(getClientSocket(),getresponse(),strlen(getresponse()),0);
                    setResponse();
                    isGetFullBody = 1;
                    inputfile.close();
                    if (remove(tmpFileName.c_str()) != 0) 
                    {
                        std::cerr << "Error deleting temporary file: " << strerror(errno) << std::endl;
                    }
                    
                }
                //bytes = 0;
                memset(bodyBuffer,0, 1024);  
            }
            else
            {
               // std::cout << "bytes written " <<  bytes << std::endl;
                inputfile.write(bodyBuffer, bytes);
                totalReaded += bytes;
               // std::cout << "bytes readed "<<  bytes << " totalReaded " << totalReaded << std::endl;
                if(contentLength <= totalReaded)
                {
                    printf("cgi on\n");
                    if (getbestMatchLocation()->cgi == "on") 
                    {
                        handle_cgi(getfilePath(), getFileExtension(getfilePath()), *this);
                        this->setResponse();
                        isGetFullBody = 1;
                        return;
                    }
                   // std::cout << "total bytes readedfrom socket " << totalReaded << std::endl;
                        setresponse("HTTP/1.1 201 Created", "text/html", "<!DOCTYPE HTML PUBLIC \"-//IETF//DTD HTML 2.0//EN\">\r\n<html><head>\r\n<title>201 Created</title>\r\n</head><body>\r\n<h1>Created</h1>\r\n<p>The resource has been successfully created.</p>\r\n</body></html>\r\n");
                        send(getClientSocket(),getresponse(),strlen(getresponse()),0);
                        setResponse();
                        isGetFullBody = 1;
                        inputfile.close();
                }
                bytes = 0;
                memset(bodyBuffer,0, 1024);
            }
        } 
    }
    else if(getbestMatchLocation()->auto_upload == "off")
    {
        setresponse("HTTP/1.1 404 Not Found", "text/html", "<!DOCTYPE HTML PUBLIC \"-//IETF//DTD HTML 2.0//EN\">\r\n<html><head>\r\n<title>404 Not Found</title>\r\n</head><body>\r\n<h1>Not Found</h1>\r\n<p>The requested URL was not found on this server.</p>\r\n</body></html>\r\n");
        send(getClientSocket(),getresponse(),strlen(getresponse()),0);  
    }
}

void Client::methoddelete()
{
    std::cout << "method is delete " << std::endl;
    bool deletionSuccess = deleteResource(getfilePath());
    if (deletionSuccess) 
    {
    setresponse("HTTP/1.1 204 No Content", "text/html", "");
    send(getClientSocket(), getresponse(), strlen(getresponse()), 0);
    }
    else {
    setresponse("HTTP/1.1 404 Not Found", "text/html", "<!DOCTYPE HTML PUBLIC \"-//IETF//DTD HTML 2.0//EN\">\r\n<html><head>\r\n<title>404 Not Found</title>\r\n</head><body>\r\n<h1>Not Found</h1>\r\n<p>The requested resource was not found on this server.</p>\r\n</body></html>\r\n");
    send(getClientSocket(), getresponse(), strlen(getresponse()), 0);
}
}

int handle_cgi(std::string file,std::string exten ,Client &client)
 {
    std::cout << "taksdfhjaklsdfjlj\n";
    Cgi cgi(file, exten,client);
    int status;
    cgi.init();
    // if (client.getParsedRequest().getMethod()== "POST")
    // {
    //     std::ofstream in("in");
    //     in.write(client.getParsedRequest().getBody().c_str(), client.getcontentLength());
    // }
    // create a new process to execute the CGI script
    int out = open("out", O_WRONLY | O_CREAT | O_TRUNC , 0644);
    int pid = fork();
    if (pid < 0)
    {
        cgi._flag = false;
        std::cout << "fork failed 500" << std::endl;
        // exit(1);
    }
    if (pid == 0) {
        if (client.getParsedRequest().getMethod() == "POST")
        {
            std::cerr << "check filenamepost : " << client.getFileNamePost() << std::endl;
            int in = open(client.getFileNamePost().c_str(), O_RDONLY);
            std::cout << "file fd " <<in << std::endl;
            dup2(in, 0);
            close(in);
        }
        dup2(out, 1);
        close(out);
         std::cerr << "cgi.getArgv()[0] : " << cgi.getArgv()[0] << std::endl;
         std::cerr << "cgi.getArgv()[1] : " << cgi.getArgv()[1] << std::endl;
        execve(cgi.getArgv()[0], cgi.getArgv(), cgi.getEnvir());
        // if execve returns, it means there was an error
        perror("execve failed");
        exit(1);
    }
    else sleep(1);
    int wait_ret = waitpid(0, &status, WNOHANG);
    if (wait_ret <= 0)
    {
        sleep(5);
        wait_ret = waitpid(0, &status, WNOHANG);
        if (wait_ret <= 0)
        {
            kill(pid,SIGTERM);
            cgi._flag = false;
            std::cout << "waitpid failed 500" << std::endl;
             client.setresponse("HTTP/1.1 404 Not Found", "text/html", "<!DOCTYPE HTML PUBLIC \"-//IETF//DTD HTML 2.0//EN\">\r\n<html><head>\r\n<title>404 Not Found</title>\r\n</head><body>\r\n<h1>Not Found</h1>\r\n<p>The requested URL was not found on this server.</p>\r\n</body></html>\r\n");
            send(client.getClientSocket(),client.getresponse(),strlen(client.getresponse()),0);
            client.setResponse();
            return(0);
        }
    }
    if (WIFEXITED(status))
    {
        
        if (WEXITSTATUS(status) == 1)
        {
            close(out);
            unlink("out");
            //unlink(client.getFileNamePost().c_str());
            std::cout << "execve failed 500" << std::endl;
           // exit(1);
        }
    }
    close(out);

    return 0;
}