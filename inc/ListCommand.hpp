#ifndef LISTCOMMAND_HPP
# define LISTCOMMAND_HPP

#include "ICommand.hpp"

class ListCommand : public ICommand
{
	public:
		void execute(Server &server, Client *client, std::istringstream &args);
};

#endif