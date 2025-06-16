#ifndef KICKCOMMAND_HPP
# define KICKCOMMAND_HPP

#include "ICommand.hpp"

class Server;

class KickCommand : public ICommand
{
	public:
		void execute(Server &server, Client *client, std::istringstream &args);
};

#endif