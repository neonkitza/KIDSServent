#include <windows.h>
#include <stdio.h> 
#include <string>
#include <thread>
#include <process.h>
#include <vector>
#include <iostream>

#include "messages .h"

#pragma comment(lib,"ws2_32.lib")

SOCKET sock; // this is the server socket
SOCKADDR_IN i_sock; // this will containt some informations about our server socket
WSADATA Data; // this is to save our socket version
std::vector<Node> neighbours;
int nrNeighbours = 0;
int nrKids = 0;
Node kids[2];
Node me;
bool first = false;
std::string id;
//socketStruct whoToCall;
Node myZero;

void SendThing(void* argv);






int Send(SOCKET sockToSend,char *Buf, int len)
{
	int slen;
	slen = send(sockToSend, Buf, len, 0);
	if (slen < 0)
	{
		printf("cannot send data");
		return 1;
	}
	return slen;
}

int Recive(char *Buf, int len)
{
	int slen;
	slen = recv(sock, Buf, len, 0);
	if (slen < 0)
	{
		printf("cannot recive data");
		return 1;
	}
	return slen;
}

int EndSocket(SOCKET sock)
{
	closesocket(sock);
	WSACleanup();
	return 1;
}
SOCKET ConnectToServent(SOCKADDR_IN* adr)
{
	WSADATA Data;
	WSAStartup(MAKEWORD(2, 2), &Data);
	SOCKET sockOUT = socket(AF_INET, SOCK_STREAM, 0);
	if (sockOUT == INVALID_SOCKET)
	{
		printf("INVALID_SOCKET!\n");
		return 1;
	}
	SOCKADDR_IN i_sockOUT;
	i_sockOUT.sin_family = AF_INET;
	i_sockOUT.sin_addr.s_addr = adr->sin_addr.s_addr;
	i_sockOUT.sin_port = adr->sin_port;
	int ss = connect(sockOUT, (struct sockaddr *)&i_sockOUT, sizeof(i_sockOUT));
	if (ss != 0)
	{
		printf("Cannot connect to another servent!\n");
	}
	/*printf("I should connect to (IP: %d.%d.%d.%d - PORT: %d)\n", int(mp.address.sin_addr.s_addr & 0xFF),
		int((mp.address.sin_addr.s_addr & 0xFF00) >> 8),
		int((mp.address.sin_addr.s_addr & 0xFF0000) >> 16),
		int((mp.address.sin_addr.s_addr & 0xFF000000) >> 24), mp.address.sin_port);
	printf("Succefully connected");*/
	return sockOUT;
}

void GOTOfunc(void* argv)
{
	SOCKADDR_IN* whereTo = (SOCKADDR_IN*)argv;
	MyPacket toSend;
	//toSend.address = myZero.address;
	//toSend.socket = myZero.socket;
	toSend.command = "ZERO";
	SOCKET buddySock = ConnectToServent(whereTo);
	Send(buddySock, (char*)&toSend, sizeof(MyPacket));
	EndSocket(buddySock);
	delete(whereTo);
}

int BSConnect(char *IP, int Port)
{
	WSAStartup(MAKEWORD(2, 2), &Data);
	sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock == INVALID_SOCKET)
	{

		return 1;
	}
	i_sock.sin_family = AF_INET;
	i_sock.sin_addr.s_addr = inet_addr(IP);
	i_sock.sin_port = htons(Port);
	int ss = connect(sock, (struct sockaddr *)&i_sock, sizeof(i_sock));
	if (ss != 0)
	{
		printf("Cannot connect");
		return 0;
	}
	BSPacket mp;
	Recive((char*)&mp, sizeof(BSPacket));

	//dal je goto ili first
	if (mp.command.compare("GOTO"))
	{
		SOCKADDR_IN* whereTo = new SOCKADDR_IN();
		whereTo = &mp.whereTo;


		i_sock.sin_family = AF_INET;
		i_sock.sin_addr.s_addr = htonl(INADDR_ANY);
		i_sock.sin_port = htons(mp.me.sin_port);
		//bind(sock, (LPSOCKADDR)&i_sock, sizeof(i_sock));
		

		_beginthread(GOTOfunc, NULL, (void*)whereTo);
	}
	else if(mp.command.compare("YOUAREZERO"))
	{
		first = true;
		id = "0";


		i_sock.sin_family = AF_INET;
		i_sock.sin_addr.s_addr = htonl(INADDR_ANY);
		i_sock.sin_port = htons(mp.me.sin_port);
		//bind(sock, (LPSOCKADDR)&i_sock, sizeof(i_sock));

	}
	/*
	printf("I should connect to (IP: %d.%d.%d.%d - PORT: %d)\n", int(mp.address.sin_addr.s_addr & 0xFF),
		int((mp.address.sin_addr.s_addr & 0xFF00) >> 8),
		int((mp.address.sin_addr.s_addr & 0xFF0000) >> 16),
		int((mp.address.sin_addr.s_addr & 0xFF000000) >> 24), mp.address.sin_port);
	printf("Succefully connected");*/
	EndSocket(sock);
	return 1;
}


void SendZero(void* argv)
{
	//socketStruct* buddySocket = (socketStruct*)argv;
	SOCKADDR_IN* whereTo = (SOCKADDR_IN*)argv;
	MyPacket toSend;
	toSend.addressToGoTo = myZero.address;
	toSend.myAddress = i_sock;
	toSend.command = "MYZERO";
	SOCKET buddySock = ConnectToServent(whereTo);
	Send(buddySock, (char*)&toSend, sizeof(MyPacket));
	EndSocket(buddySock);
	delete(whereTo);
}

void contactZero(void* argv)
{
	//SOCKADDR_IN* whereTo = (SOCKADDR_IN*)argv;
	MyPacket toSend;
	toSend.addressToGoTo = myZero.address;
	toSend.myAddress = i_sock;
	toSend.command = "ZERO";
	SOCKET buddySock = ConnectToServent(&myZero.address);
	Send(buddySock, (char*)&toSend, sizeof(MyPacket));
	EndSocket(buddySock);

	//delete(whereTo);
}

bool hasEnding(std::string const &fullString, std::string const &ending) {
	if (fullString.length() >= ending.length()) {
		return (0 == fullString.compare(fullString.length() - ending.length(), ending.length(), ending));
	}
	else {
		return false;
	}
}
std::string getIthDigitFromEnd(std::string text, int i)
{
	return text.substr(text.length()-i,text.length()-i+1);
}
std::string getStartID(std::string text)
{
	return text.substr(0, 1);
}
std::string getEndID(std::string text)
{
	return text.substr(text.length() - 1);
}
bool StartsWith(const std::string& text, const std::string& token)
{
	if (text.length() < token.length())
		return false;
	return (text.compare(0, token.length(), token) == 0);
}

bool amIFree()
{
	//if (nrKids < 2) return true;
	int k = 0;
	for (int i = 0; i < neighbours.size(); i++)
	{
		if (getEndID(id).compare("1") || getEndID(id).compare("2"))
		{
			k++;
			nrKids++;
		}
		if (k == 2)
			nrKids = 2;
			return false;
	}
	return true;
}
std::string createID(std::string parentID)
{

}
std::string findMaxNeighbourID()
{
	std::string max = id;
	int maxi = atoi(id.c_str());
	int temp = 0;
	for (int i = 0; i < neighbours.size(); i++)
	{
		temp = atoi(neighbours[i].id.c_str());
		if (temp > maxi)
		{
			maxi = temp;
			max = neighbours[i].id;
		}
	}
	return max;
}
std::string createID(std::string base,std::string end)
{
	return base.substr(base.length() - 1) + end;
}

void whereToGo(void* argv)
{
	SOCKADDR_IN* buddySocket = (SOCKADDR_IN*)argv;
	if (getEndID(id).compare("0"))
	{
		if (amIFree)
		{
			MyPacket toSend;
			nrKids++;
			Node myChild;
			myChild.address = (SOCKADDR_IN&) buddySocket;
			myChild.id = std::to_string(nrKids);

			neighbours.push_back(myChild);

			//kontaktirati buddySocket da nas doda u neighboure

			
		}
		else
		{

			//salji neighbourima koji se zavrsavaju na 0 dal su oni free


			MyPacket toSend;
			toSend.addressToGoTo = myZero.address;
			toSend.myAddress = i_sock;
			//toSend.socket = myZero.socket;
			toSend.command = "FREE";
			SOCKET buddySock = ConnectToServent(&myZero.address);
			Send(buddySock, (char*)&toSend, sizeof(MyPacket));
			EndSocket(buddySock);

			delete(buddySocket);
		}
	}
	else
	{
		/*	std::string max = findMaxNeighbourID();
		if (getEndID(max).compare("1"))
		{
		//fali nam dvojka
		toSend.id = createID(max, "2");
		}
		else if (getEndID(max).compare("2"))
		{
		//puni smo, vrati se
		}
		else if (getEndID(max).compare("0"))
		{
		//idi na taj cvor i ponovi sve ovo
		toSend.command = "ZERO";
		toSend
		}*/
	}
}
void ServerThing(void* argv)
{
	SOCKADDR_IN* buddySocket = (SOCKADDR_IN*)argv;
	MyPacket buddy;
	Recive((char*)&buddy, sizeof(MyPacket));	
	if (buddy.command.compare("ZERO"))
	{
		//vrati buddyu koga da kontaktira
		if (!first)
		{
			_beginthread(SendZero, NULL, (void*)buddySocket);

		}
		else
		{

			_beginthread(whereToGo, NULL, (void*)buddySocket);
		}
	}
	if (buddy.command.compare("MYZERO"))
	{
		myZero.id = buddy.id;
		myZero.address = buddy.addressToGoTo;
		
		_beginthread(contactZero, NULL, NULL);
		delete(buddySocket);
	}
	//EndSocket(buddySocket->socket);
}
int main()
{
	//bootstrap
	BSConnect("127.0.0.1", 9999);
	//zatvori
	//EndSocket(s);
	/*WSAStartup(MAKEWORD(2, 2), &Data);
	sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock == INVALID_SOCKET)
	{

		return 1;
	}
	*/
	if (sock == INVALID_SOCKET)
	{
		return 1;
	}

	bind(sock, (LPSOCKADDR)&i_sock, sizeof(i_sock));
	listen(sock, 100);
	while (1)
	{
		SOCKET myBuddy;
		SOCKADDR_IN i_sock2;
		int so2len = sizeof(i_sock2);
		myBuddy = accept(sock, (sockaddr *)&i_sock2, &so2len);
		//socketStruct *ss = new socketStruct();
		//ss->address = i_sock2;
		//ss->socket = myBuddy;
		_beginthread(ServerThing, NULL, (void*)&i_sock2);

	}

	EndSocket(sock);

	return 0;
}