NAME = webserv

SRC = main.cpp ConfigFile.cpp HttpRequest.cpp Client.cpp cgi-bin/Cgi.cpp #Responce.cpp

OBJ = $(SRC:.cpp=.o)

CXX = c++

CXXFLAGS = -Wall -Werror -Wextra -std=c++98 -g3 -fsanitize=address

RM = rm -rf

all : $(NAME)

$(NAME) : $(OBJ) 
	$(CXX) $(CXXFLAGS) $(OBJ) -o $(NAME)

clean :
	$(RM) $(OBJ)

fclean : clean
	$(RM) $(NAME)

re : fclean all