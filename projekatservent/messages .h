#include <string>
#include <windows.h>
#pragma once


struct BSPacket
{
	int command;
	SOCKADDR_IN whereTo;
	SOCKADDR_IN me;
};


typedef struct
{
	char id[20];
	SOCKADDR_IN address;
	//SOCKET socket;

}Node;

struct MyPacket
{
	int command;
	char id[20];
	int layer;
	SOCKADDR_IN addressToGoTo; //who to send MyPacket to
	SOCKADDR_IN addressToContact; //who should we contact once we recieve the packet
	SOCKADDR_IN addressOfOrigin; //who sent it

};

struct socketStruct
{
	SOCKADDR_IN address;
	SOCKET socket;
};