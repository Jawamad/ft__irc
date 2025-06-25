#include "../inc/utils.hpp"

std::string intToString(int n) {
	std::ostringstream oss;
	oss << n;
	return oss.str();
}