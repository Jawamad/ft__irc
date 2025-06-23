#ifndef PASSCOMMAND_HPP
# define PASSCOMMAND_HPP

#include "ICommand.hpp"

class PassCommand : public ICommand
{
	public:
		void execute(Server &server, Client *client, std::istringstream &args);
};

#endif