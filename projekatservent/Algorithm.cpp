#include <windows.h>
#include <stdio.h> 
#include <string>
#include <thread>
#include <process.h>
#pragma comment(lib,"ws2_32.lib")

SOCKET sock; // this is the server socket
SOCKADDR_IN i_sock; // this will containt some informations about our server socket
WSADATA Data; // this is to save our socket version
Node* neighbours;
int nrNeighbours = 0;
int kids = 0;
Node me;
bool first = false;
std::string id;
socketStruct whoToCall;
Node myZero;

void SendThing(void* argv);



struct AcceptPacket
{
	std::string command;
	socketStruct whereTo;
	socketStruct me;
};

typedef struct
{
	std::string id;
	//SOCKADDR_IN address;
	//SOCKET socket;

}Node;
struct MyPacket
{
	std::string command;
	std::string id;
	SOCKADDR_IN address;
	SOCKET socket;

};
typedef struct {
	SOCKADDR_IN address;
	SOCKET socket;
}socketStruct;
/*
int amIFree()
{
	int k;
	if (kids == 2) return "NO";
	for(int i = 0; i < nuberofnei)
	return 0;
}
*/
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
SOCKET ConnectToServent(SOCKADDR_IN adr)
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
	i_sockOUT.sin_addr.s_addr = adr.sin_addr.s_addr;
	i_sockOUT.sin_port = adr.sin_port;
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
	socketStruct* whereTo = (socketStruct*)argv;
	MyPacket toSend;
	//toSend.address = myZero.address;
	//toSend.socket = myZero.socket;
	toSend.command = "ZERO";
	SOCKET buddySock = ConnectToServent(whereTo->address);
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
	AcceptPacket mp;
	Recive((char*)&mp, sizeof(MyPacket));

	//dal je goto ili first
	if (mp.command.compare("GOTO"))
	{
		socketStruct* whereTo = new socketStruct();
		whereTo->address = mp.whereTo.address;
		whereTo->socket = mp.whereTo.socket;


		i_sock.sin_family = AF_INET;
		i_sock.sin_addr.s_addr = htonl(INADDR_ANY);
		i_sock.sin_port = htons(mp.me.address.sin_port);
		bind(sock, (LPSOCKADDR)&i_sock, sizeof(i_sock));
		

		_beginthread(GOTOfunc, NULL, (void*)whereTo);
	}
	else if(mp.command.compare("YOUAREZERO"))
	{
		first = true;
		id = "0";


		i_sock.sin_family = AF_INET;
		i_sock.sin_addr.s_addr = htonl(INADDR_ANY);
		i_sock.sin_port = htons(mp.me.address.sin_port);
		bind(sock, (LPSOCKADDR)&i_sock, sizeof(i_sock));

	}
	/*
	printf("I should connect to (IP: %d.%d.%d.%d - PORT: %d)\n", int(mp.address.sin_addr.s_addr & 0xFF),
		int((mp.address.sin_addr.s_addr & 0xFF00) >> 8),
		int((mp.address.sin_addr.s_addr & 0xFF0000) >> 16),
		int((mp.address.sin_addr.s_addr & 0xFF000000) >> 24), mp.address.sin_port);
	printf("Succefully connected");*/
	return 1;
}


void ClientThing(void* argv)
{
	SOCKET buddySocket = (SOCKET)argv;
	MyPacket mp;
	Recive((char*)&mp, sizeof(MyPacket));
	MyPacket buddy;
	buddy.address = mp.address;
	buddy.socket = mp.socket;
	if (mp.command.compare("ZERO"))
	{
		//vrati buddyu koga da kontaktira
		MyPacket toSend;
		toSend.address = myZero.address;
		toSend.socket = myZero.socket;
		toSend.command = "";
		Send(mp.socket,(char*)&toSend, sizeof(MyPacket));
	}

}

void SendZero(void* argv)
{
	socketStruct* buddySocket = (socketStruct*)argv;
	MyPacket toSend;
	toSend.address = myZero.address;
	toSend.socket = myZero.socket;
	toSend.command = "MYZERO";
	ConnectToServent(buddySocket->address);
	Send(buddySocket->socket, (char*)&toSend, sizeof(MyPacket));
	EndSocket(buddySocket->socket);
	delete(buddySocket);
}

void contactZero(void* argv)
{
	socketStruct* buddySocket = (socketStruct*)argv;
	MyPacket toSend;
	toSend.address = myZero.address;
	toSend.socket = myZero.socket;
	toSend.command = "ZERO";
	SOCKET buddySock = ConnectToServent(myZero->address);
	Send(buddySock, (char*)&toSend, sizeof(MyPacket));
	EndSocket(buddySock);

	delete(buddySocket);
}


void whereToGo(void* argv)
{
	for (int i; i < nrNeighbours; i++)
	{

	}
}
void ServerThing(void* argv)
{
	socketStruct* buddySocket = (socketStruct*)argv;
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
		myZero.socket = buddy.socket;
		myZero.address = buddy.address;
		//socketStruct* ss = new socketStruct();
		//buddy je 0 onog koji nam je poslao
		_beginthread(contactZero, NULL, (void*)buddySocket);
	}
	//EndSocket(buddySocket->socket);
}
int main()
{
	//bootstrap
	BSConnect("127.0.0.1", 9999);
	//zatvori
	/*WSAStartup(MAKEWORD(2, 2), &Data);
	sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock == INVALID_SOCKET)
	{

		return 1;
	}
	*/
	while (1)
	{
		SOCKET myBuddy;
		SOCKADDR_IN i_sock2;
		int so2len = sizeof(i_sock2);
		myBuddy = accept(sock, (sockaddr *)&i_sock2, &so2len);
		socketStruct *ss = new socketStruct();
		ss->address = i_sock2;
		ss->socket = myBuddy;
		_beginthread(ServerThing, NULL, (void*)ss);

	}

	EndSocket(sock);

	return 0;
}