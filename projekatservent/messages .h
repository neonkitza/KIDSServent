#include <string>
#include <windows.h>
#pragma once

struct BSPacket
{
	std::string command;
	SOCKADDR_IN whereTo;
	SOCKADDR_IN me;
};

typedef struct
{
	std::string id;
	SOCKADDR_IN address;
	//SOCKET socket;

}Node;

struct MyPacket
{
	std::string command;
	std::string id;
	SOCKADDR_IN addressToGoTo; //destination
	SOCKADDR_IN myAddress; //origin (aka me)

};