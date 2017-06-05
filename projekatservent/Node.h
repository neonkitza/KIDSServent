#pragma once
#include <string>
class Node
{
public:
	std::string id;
	int socket;
	int port;

	Node();
	Node(std::string id);
	~Node();
};

