#ifndef PINGCOMMAND_HPP
# define PINGCOMMAND_HPP

#include "ICommand.hpp"

class PingCommand : public ICommand
{
	public:
		void execute(Server &server, Client *client, std::istringstream &args);
};

#endif