#ifndef INVITECOMMAND_HPP
# define INVITECOMMAND_HPP

#include "ICommand.hpp"

class Server;

class InviteCommand : public ICommand
{
	public:
		void execute(Server &server, Client *client, std::istringstream &args);
};

#endif