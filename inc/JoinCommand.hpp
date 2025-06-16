#ifndef JOINCOMMAND_HPP
# define JOINCOMMAND_HPP

#include "ICommand.hpp"

class JoinCommand : public ICommand
{
	public:
		void execute(Server &server, Client *client, std::istringstream &args);
};

#endif