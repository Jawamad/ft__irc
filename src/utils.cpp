#include "../inc/utils.hpp"
#include <sstream>

std::string intToString(int n)
{
	std::ostringstream oss;
	oss << n;
	return oss.str();
}