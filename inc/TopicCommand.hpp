#ifndef TOPICCOMMAND_HPP
# define TOPICCOMMAND_HPP

#include "ICommand.hpp"

class Server;

class TopicCommand : public ICommand
{
	public:
		void execute(Server &server, Client *client, std::istringstream &args);
};

#endif