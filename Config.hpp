#ifndef CONFIGURATION_HPP
# define CONFIGURATION_HPP

#include <iostream>
#include <fstream>
#include <cstdlib>
#include <sstream>
#include <deque>
#include <map>
#include <string>
#include <cstring>
#include <vector>

struct location
{
    std::string path_location, root, auto_upload, upload_path, auto_index, redirection, cgi_path, cgi, index;
    std::deque<std::string> allow_methods;
};

struct server
{
    std::string host, server_name;
    std::string root;
    int port;
    size_t mx_cl_bd_size;
    std::map<std::string, std::string> err_pages;
    std::deque<location> locations;
};

class confi
{
    public:
        std::deque<server> servers;
        void    split_string(std::string &, location &lc, char c);
        int     parse_configuration(std::string);
};

#endif