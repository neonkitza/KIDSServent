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
	int layer;
	SOCKADDR_IN addressToGoTo; //who to send MyPacket to
	SOCKADDR_IN addressToContact; //who should we contact once we recieve the packet
	SOCKADDR_IN addressOfOrigin; //who sent it

};