#ifndef USERCOMMAND_HPP
# define USERCOMMAND_HPP

#include "ICommand.hpp"

class UserCommand: public ICommand
{
	public:
		void execute(Server &server, Client *client, std::istringstream &args);
};

#endif