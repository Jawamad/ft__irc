#ifndef CAPCOMMAND_HPP
# define CAPCOMMAND_HPP

#include "ICommand.hpp"

class CapCommand : public ICommand
{
	public:
		void execute(Server &server, Client *client, std::istringstream &args);
};

#endif