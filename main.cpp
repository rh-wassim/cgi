
#include "Client.hpp"
#include <list>


// void    handleClients(std::list<Client> &client)
// {
//   for (std::list<Client>::iterator it = client.begin(); it != client.end();++it)
//   {
//     (*it).handleClient();
//   } 
 
// }
int checkClients(std::list<Client> &clients, fd_set  *tmpfdsread ,fd_set  *tmpfdswrite)
{
    int i = 0;
    std::list<Client>::iterator it = clients.begin();
    while (it != clients.end())  
    {
        if (it->isResponseFinished())
        {
            std::cout << "closed socket " << it ->getClientSocket() << std::endl;
            it->closeClientSocket();
            FD_CLR(it ->getClientSocket() , tmpfdsread);   
            FD_CLR(it ->getClientSocket() , tmpfdswrite);    
            it = clients.erase(it);
            i =1;
        }
        else
        {
            ++it;
        }
    }
    if(i == 0)
        return(0);
    else 
        return(1);
}

void startServer(std::deque<server>& serv, std::vector<int> &serverSockets) 
{
    for (size_t i = 0; i < serv.size(); i++)
    {
        int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
        if (serverSocket == -1)
        {
            throw std::runtime_error("Error creating socket\n");
        }
        sockaddr_in serverAddr1;
        serverAddr1.sin_family = AF_INET;
        serverAddr1.sin_addr.s_addr = INADDR_ANY;
        serverAddr1.sin_port = htons(serv[i].port);
        int optval = 1;
        if (setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) == -1) {
            throw std::runtime_error("Error binding socket\n");
        }
        if (bind(serverSocket, reinterpret_cast<sockaddr*>(&serverAddr1), sizeof(serverAddr1)) == -1) {
            close(serverSocket);
            throw std::runtime_error("Error binding socket\n");
        }
        if (listen(serverSocket, 10) == -1) 
        {
            close(serverSocket);
            throw std::runtime_error("Error listening on socket\n");
        }
        std::cout << "Server listening on port " << serv[i].port << "...\n";
        serverSockets.push_back(serverSocket);
    }
}

int main(int ac , char **av)
{
    if (ac != 2){
        std::cout << "----->  ./webserv [configuration file]\n";
        return (1);
    }
    confi cfg;
    cfg.parse_configuration(av[1]);
    std::cout << "---OK---\n";
    std::vector<int> serverSockets;
    startServer(cfg.servers, serverSockets);
    std::list<Client> clientSockets;
    fd_set  writefds;
    fd_set   readfds;
    int maxSocket = serverSockets.back();
        struct timeval timeout;
        FD_ZERO(&readfds);
        FD_ZERO(&writefds);
        for (std::vector<int>::iterator it = serverSockets.begin(); it != serverSockets.end(); ++it) 
        {      
            const int& socket = *it;
            FD_SET(socket, &readfds);
            FD_SET(socket, &writefds);
        }
    signal(SIGPIPE, SIG_IGN);  
    while (true) 
    {
       for (std::vector<int>::iterator iit = serverSockets.begin(); iit != serverSockets.end(); ++iit) 
        {
             fd_set  tmpfdsread = readfds;
            fd_set  tmpfdswrite = writefds;
             timeout.tv_sec = 0;  
            timeout.tv_usec = 0;
            if (select(maxSocket + 1, &tmpfdsread,&tmpfdswrite, 0, &timeout) == -1)
            {
                std::cerr << "Error in select\n";
            }
            if (FD_ISSET(*iit, &tmpfdsread))
            {        
                sockaddr_in clientAddr;
                socklen_t clientAddrSize = sizeof(clientAddr);
                int clientSocket = accept(*iit, reinterpret_cast<sockaddr*>(&clientAddr), &clientAddrSize);
                if (clientSocket == -1)
                {
                    std::cerr << "Error accepting connection\n";
                    continue;
                }
                fcntl(clientSocket, F_SETFL, O_NONBLOCK);
                Client client(clientSocket, cfg);
                FD_SET(clientSocket, &readfds); 
                FD_SET(clientSocket, &writefds);
                clientSockets.push_back(client);
            }
            if (clientSockets.size())
            for (std::list<Client>::iterator it = clientSockets.begin(); it != clientSockets.end();++it)
            {
                if (FD_ISSET((*it).getClientSocket(), &tmpfdsread)) 
                {
                    (*it).handelread();
                }
                if (FD_ISSET((*it).getClientSocket(), &tmpfdswrite) && (*it).isRequestFinished() && !(*it).isResponseFinished())
                {
                    (*it).handleClient();
                }   
            }
        if (clientSockets.size())
        {
            if(checkClients(clientSockets ,&readfds,&writefds ) == 1)         
                maxSocket = clientSockets.back().getClientSocket();
            if (clientSockets.back().getClientSocket() > maxSocket)
                maxSocket = clientSockets.back().getClientSocket();
        }
        else 
            maxSocket = serverSockets.back();
        } 
    }
    for (std::vector<int>::iterator it = serverSockets.begin(); it != serverSockets.end(); ++it) 
    {
        const int& socket = *it;
        close(socket);
    }
    return 0;
}

