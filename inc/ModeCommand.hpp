#ifndef MODECOMMAND_HPP
# define MODECOMMAND_HPP

#include "ICommand.hpp"

class Server;

class ModeCommand : public ICommand
{
	public:
		void execute(Server &server, Client *client, std::istringstream &args);
};

#endif