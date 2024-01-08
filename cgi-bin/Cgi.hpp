#ifndef CGI_HPP
#define CGI_HPP

#include <sys/wait.h>
#include <signal.h>

#include "../Client.hpp"

typedef	std::string	string;
typedef	std::map<string, string>	mapstring;

class Client;

#define MAX_INPUT 1024
class Cgi {
	private:
		HttpRequest	_req;
		string		_file;
		string		_exten;
		string		_init_line;
		string 		buf;
    	mapstring   _env;
    	string		resp;
		string		execut;
    	string 		username;
    	char 		**env;
    	char 		**av;
		int			clientSocket;
	public:
		Cgi(string file, string extention, Client& res);
		~Cgi( void );
		void	init( void );
		char 	**getEnvir( void );
		char 	**getArgv( void );
		void 	fill_env( string , HttpRequest &, mapstring &);
		void    free_all();
		char**	convertMapToCharArray(mapstring& );
		bool		_flag;
};

#endif