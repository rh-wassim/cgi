#include "Cgi.hpp"
     
Cgi::Cgi( string file, string extention, Client &res) 
{
    _flag = true;
    _file = file;
    _exten = extention;
    _req = res.getParsedRequest();
	username = getlogin();
	av = new char *[3];
    res.setresponse("200", "text/html", "");
    _init_line = "HTTP/1.1 200 OK\r\n";
    clientSocket = res.getClientSocket();
}

void    Cgi::free_all()
{
    for (int i = 0; env[i]; i++)
        delete[] env[i];
    delete[] env;
    for (int j = 0; j < 3; j++)
        free(av[j]);
    delete[] av;
}

char** Cgi::convertMapToCharArray(mapstring& map) {
    // Create a char** array to hold the converted data
    char** charArray = new char*[map.size() + 1]; // +1 for the terminating nullptr
    
    // Convert each key-value pair to "KEY=VALUE" format
    size_t i = 0;
    for (mapstring::iterator iter = map.begin(); iter != map.end(); ++iter) {
        string envVar = iter->first + "=" + iter->second;
        // Allocate memory for the current environment variable
        charArray[i] = new char[envVar.size() + 1];
        // Copy the environment variable string to charArray
        std::strcpy(charArray[i], envVar.c_str());
        ++i;
    }
    // Set the last element of the array as nullptr
    charArray[map.size()] = NULL;
    return charArray;
}

void Cgi::fill_env(string file, HttpRequest& request, mapstring& _env) {

    _env["REDIRECT_STATUS"]= "200";
    _env["REQUEST_METHOD"] = _req.getMethod();
    if (_req.getMethod() == "POST")
    {
        std::map<std::string, std::string>::const_iterator contentTypeIt =  request.getHeaders().find("Content-Type");
        if (contentTypeIt != request.getHeaders().end()) // Check if the key exists
        {
            std::cout <<  contentTypeIt->second  << "content type " << std::endl;
            _env["CONTENT_TYPE"] = "image/jpeg"; // Assign the value associated with the key
        }
        
        std::map<std::string, std::string>::const_iterator contentLengthIt = request.getHeaders().find("Content-Length");
        if (contentLengthIt != request.getHeaders().end()) // Check if the key exists
        {
            _env["CONTENT_LENGTH"] = contentLengthIt->second; // Assign the value associated with the key
        }
    }
    _env["GATEWAY_INTERFACE"] = "CGI/1.1";
    _env["DOCUMENT_ROOT"] = "../";
    _env["PATH_INFO"] = file;
    _env["SCRIPT_NAME"] = file;
    _env["SCRIPT_FILENAME"] = file;
    if (request.getMethod() == "GET")
    {
        _env["QUERY_STRING"] = _req.getQuery();
        std::map<std::string, std::string>::const_iterator cookieIt = request.getHeaders().find("Cookie");
        if (cookieIt != request.getHeaders().end())
        {
            _env["HTTP_COOKIE"] = cookieIt->second;
        }
    }
}

void    Cgi::init() {
    // Set environment variables for the CGI script
    fill_env(_file, _req, _env);
	env = convertMapToCharArray(_env);
	if (_exten == ".php")
        execut = "/Users/wassim/Desktop/salam/cgi-bin/php-cgi";
    else
        execut = "/usr/bin/python3";
    av = new char*[3];
    av[0] = strdup(execut.c_str());
    av[1] = strdup(_file.c_str());
    av[2] = NULL;
}

char**	Cgi::getEnvir( void ){
	return (env);
}

char** Cgi::getArgv( void ) {
	return (av);
}

Cgi::~Cgi( void ) 
{
	free_all();
    std::ifstream input("out");
    std::stringstream sss;
    sss << input.rdbuf();
    resp = _init_line + sss.str();
    if (_flag == true)
    {
        send(clientSocket, resp.c_str(), resp.size(), 0);
    }
         
    unlink("out");
    unlink("in");
}

