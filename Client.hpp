#ifndef CLIENT_HPP
#define CLIENT_HPP

#include "HttpRequest.hpp"
#include <sstream>
#include "cgi-bin/Cgi.hpp"

class Cgi;

class Client
{
    private:
    int             clientSocket;
    std::string     requestBuffer;
    bool            isGetFullReq;
    bool            isSentFullRes;
    confi           cfg;
    std::string     response;
    const location* bestMatchLocation;
    std::string     bestmatch;
    size_t          bestMatchLength;
    HttpRequest     request;
    std::string     filePath;
    bool            methodAllowed;
    bool            isSentHreders;
    bool            isSetRequest;
    std::ifstream   fileStream;
    struct location loc;
    std::ofstream   inputfile;
    std::ofstream    tmpInputfile;
    std::ifstream    tmpInputfile2;
    int             fileDescriptor;
    char            bodyBuffer[1024];
    char            buffer[1024];
    bool            isFileCreated;
    bool            isGetFullBody;
    int             bytes;
    bool            isChunked;
    std::string     tmpFileName;
    int             totalReaded;
    int             contentLength;
    std::string     fileNamePost;
    std::map<std::string, std::string> err_page;
public:
    Client(){}
    Client(int Socket, confi &cf);
    std::string getExtensionByContentType(const std::string& contentType);
    void methoddelete();
    void handleDirectoryAutoIndex();
    bool sendHredes2(std::string  fullPath);
    void methodGetFile();
    void methodGet();
    void notfound();
    bool handleErrorResponse(const std::string& errorCode) ;
    void methodnotallow();
    void checkopen();
    void    getRequest();
    void handelread();
    
    bool    isRequestFinished();
    bool    isResponseFinished();
    int     getClientSocket()const;
    void    closeClientSocket();
    void    setResponse();
    void    setParsedRequest();
    HttpRequest&    getParsedRequest();
    ~Client();
    void methodpost();
    void    handleClient();
    ///****************
    bool sendHredes();
    std::string getfilePath();
    std::string &getbestmatch();
    const location*  getbestMatchLocation();
    void setClientResponce();
    void setresponse(const std::string& status,const std::string& contentType,const std::string& content);
    const char *getresponse();
    void matchinglocation();
    void split(const std::string& input, char delimiter, std::string& part1, std::string& part2);
    std::string getFileExtension(const std::string& filePath);
    bool deleteResource(std::string fileName);
    std::string concatenatePaths(const std::string& path1, const std::string& path2);
    std::string toLowerCase(const std::string& str);
    std::string getContentTypeByExtension(const std::string& extension);
    bool    createFile();
    void        parseChunked();
    int getcontentLength();
    std::string getFileNamePost();

    Client(const Client& other)
        // : clientSocket(other.clientSocket),
        //   requestBuffer(other.requestBuffer),
        //   isGetFullReq(other.isGetFullReq),
        //   isSentFullRes(other.isSentFullRes),
        //   cfg(other.cfg),  // Assuming confi has an appropriate copy constructor
        //   response(other.response),
        //   bestMatchLocation(other.bestMatchLocation),
        //   bestmatch(other.bestmatch),
        //   bestMatchLength(other.bestMatchLength),
        //   request(other.request),
        //   filePath(other.filePath),
        //   methodAllowed(other.methodAllowed),
        //   isSentHreders(other.isSentHreders),
        //   isSetRequest (other.isSetRequest)
         // fileStream(other.fileStream)  // Assuming std::ifstream is copyable
    {
       // std::cout << "i segfault in the constroctor\n";
            clientSocket = other.clientSocket;
            requestBuffer = other.requestBuffer;
            isGetFullReq = other.isGetFullReq;
            isSentFullRes = other.isSentFullRes;
            cfg = other.cfg;  // Assuming confi has an appropriate assignment operator
            response = other.response;
            bestMatchLocation = other.bestMatchLocation;
            bestmatch = other.bestmatch;
            bestMatchLength = other.bestMatchLength;
            request = other.request;
            filePath = other.filePath;
            methodAllowed = other.methodAllowed;
            isSentHreders = other.isSentHreders;
            isSetRequest = other.isSetRequest;
            fileNamePost = other.fileNamePost;
    }
    Client& operator=(const Client& other)
    {
        if (this != &other) // Self-assignment check
        {
            // Copy members here similar to the copy constructor
            clientSocket = other.clientSocket;
            requestBuffer = other.requestBuffer;
            isGetFullReq = other.isGetFullReq;
            isSentFullRes = other.isSentFullRes;
            cfg = other.cfg;  // Assuming confi has an appropriate assignment operator
            response = other.response;
            bestMatchLocation = other.bestMatchLocation;
            bestmatch = other.bestmatch;
            bestMatchLength = other.bestMatchLength;
            request = other.request;
            filePath = other.filePath;
            methodAllowed = other.methodAllowed;
            isSentHreders = other.isSentHreders;
            isSetRequest = other.isSetRequest;
            fileNamePost = other.fileNamePost;
            //fileStream = other.fileStream;  // Assuming std::ifstream is copyable
        }
        return *this;
    }
};
int handle_cgi(std::string file,std::string exten, Client& );
void printParsedRequest(HttpRequest &request);

#endif
