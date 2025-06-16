#ifndef NICKCOMMAND_HPP
# define NICKCOMMAND_HPP

#include "ICommand.hpp"

class Server;

class NickCommand : public ICommand
{
	public:
		void execute(Server &server, Client *client, std::istringstream &args);
};

#endif