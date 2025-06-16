#include "../inc/ICommand.hpp"

ICommand::ICommand()
{}
ICommand::ICommand(ICommand& obj)
{
	*this = obj;
}
ICommand& ICommand::operator=(ICommand& obj)
{
	(void) obj;
	return *this;
}
ICommand::~ICommand()
{}