#include <windows.h>
#include <stdio.h> 
#include <string>
#include <thread>
#include <process.h>
#include <vector>
#include <iostream>

#include "util.h"
#include "messages .h"

#pragma comment(lib,"ws2_32.lib")


void addZeroToID(MyPacket);
void SendMyThing(MyPacket);


SOCKET sock; // this is the server socket
SOCKADDR_IN i_sock; // this will containt some informations about our server socket
WSADATA ServData; // this is to save our socket version
std::vector<Node> neighbours;
int nrNeighbours = 0;
int nrKids = 0;
Node kids[2];
Node me;
bool first = false;
std::string id;
//socketStruct whoToCall;
Node myZero;


int Send(SOCKET sockToSend,char *Buf, int len)
{
	int slen;
	slen = send(sockToSend, Buf, len, 0);
	if (slen < 0)
	{
		printf("cannot send data\n");
		return 1;
	}
	return slen;
}

int Recive(char *Buf, int len,SOCKET sock)
{
	int slen;
	slen = recv(sock, Buf, len, 0);
	int err = WSAGetLastError();
	if (slen < 0)
	{
		printf("cannot recive data\n");
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
	
	toSend.command = "ZERO";
	toSend.addressToGoTo = *whereTo;
	toSend.addressOfOrigin = i_sock;
	toSend.addressToContact = i_sock;

	SendMyThing(toSend);
	//delete(whereTo);
}

void BSConnect(char *IP, int Port)
{
	WSADATA Data;
	WSAStartup(MAKEWORD(2, 2), &Data);
	SOCKET BSsock = socket(AF_INET, SOCK_STREAM, 0);
	if (BSsock == INVALID_SOCKET)
	{

		printf("ERROR\n");
	}
	i_sock.sin_family = AF_INET;
	i_sock.sin_addr.s_addr = inet_addr(IP);
	i_sock.sin_port = htons(Port);
	int ss = connect(BSsock, (struct sockaddr *)&i_sock, sizeof(i_sock));
	if (ss != 0)
	{
		printf("Cannot connect\n");
	}
	BSPacket *mp = new BSPacket();
	Recive((char*)mp, sizeof(BSPacket),BSsock);

	//dal je goto ili first
	if (mp->command.compare("GOTO") == 0)
	{

		i_sock.sin_family = AF_INET;
		i_sock.sin_addr.s_addr = htonl(INADDR_ANY);
		i_sock.sin_port = htons(mp->me.sin_port);
		printf("My port is: %d\n", i_sock.sin_port);
		//bind(sock, (LPSOCKADDR)&i_sock, sizeof(i_sock));
		
		MyPacket toSend;

		toSend.command = "ZERO";
		toSend.addressToGoTo = mp->whereTo;
		toSend.addressOfOrigin = i_sock;
		toSend.addressToContact = i_sock;

		SendMyThing(toSend);
	}
	else if(mp->command.compare("FIRST") == 0)
	{
		first = true;
		id = "0";
		me.id = id;
		
		i_sock.sin_family = AF_INET;
		i_sock.sin_addr.s_addr = htonl(INADDR_ANY);
		i_sock.sin_port = htons(mp->me.sin_port);
		me.address = i_sock;



	}

	/*
	printf("I should connect to (IP: %d.%d.%d.%d - PORT: %d)\n", int(mp.address.sin_addr.s_addr & 0xFF),
		int((mp.address.sin_addr.s_addr & 0xFF00) >> 8),
		int((mp.address.sin_addr.s_addr & 0xFF0000) >> 16),
		int((mp.address.sin_addr.s_addr & 0xFF000000) >> 24), mp.address.sin_port);
	printf("Succefully connected");*/
	
	delete(mp);
	EndSocket(BSsock);
	printf("ENDING CONNECTION WITH SERVER");
}

void SendMyThing(MyPacket toSend)
{
	SOCKET buddySock = ConnectToServent(&toSend.addressToGoTo);
	Send(buddySock, (char*)&toSend, sizeof(MyPacket));
	//EndSocket(buddySock);
	//delete(mp); //mozda ne ovde
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
std::string createID(int layer,char whichOne)
{
	std::string newID = id;
	newID[layer] = whichOne;
	return newID;

}

void broadcastMSG(MyPacket toSend)
{
	//int layer = toSend.layer;
	for (int l = toSend.layer; l < id.size(); l++)
	{
		for (int i = 0; i < neighbours.size(); i++)
		{
			if (neighbours[i].id[l] > id[l])
			{
				//send the msg
				toSend.addressToGoTo = neighbours[i].address;
				toSend.layer = l+1;
				SendMyThing(toSend);
			}
		}
	}

}

int findNeighbourWithMaxId(int layer)
{
	std::string max = id;
	int temp = -1;
	for (int l = layer; l < id.size() - 1; l++)
	{
		for (int i = 0; i < neighbours.size(); i++)
		{
			if (neighbours[i].id[l] > max[l] && (neighbours[i].id[neighbours[i].id.size() - 1] == '1' || neighbours[i].id[neighbours[i].id.size() - 1] == '2'))
			{
				temp = i;
			}
		}
		if (temp != -1) break;
	}
	return temp;
}

void addZeroToMyID()
{
	id = "0" + id;
}

void addZeroToID(MyPacket buddy)
{
	MyPacket toSend;
	if (buddy.id.size() > id.size())
		addZeroToMyID();
	toSend.id = id;
	toSend.layer = buddy.layer;
	toSend.addressOfOrigin = i_sock;
	toSend.command = "ADDZEROTOID";

	broadcastMSG(toSend);
}



void goBack(void* argv)
{
	MyPacket* buddy = (MyPacket*)argv;
}

void whereToGoFromZero(MyPacket* buddy)
{
	//SOCKADDR_IN* buddySocket = (SOCKADDR_IN*)argv;
	MyPacket toSend;
	toSend.layer = 0;
	buddy->layer = 0;
	if (amIFree())
	{
		nrKids++;
		Node myChild;
		myChild.address = buddy->addressToContact;
		myChild.id = createID(0, '0' + nrKids);

		neighbours.push_back(myChild);
		kids[nrKids - 1] = myChild;

		//kontaktirati buddy->addressToContact da nas doda u neighboure


	}
	else
	{
		//if(buddy->layer == 0)
		int neighbourIndex = findNeighbourWithMaxId(toSend.layer);
		if (neighbourIndex == -1)
		{
			//povecati broj cifara svih prethodnih
			addZeroToID(*buddy);

			//mora da se grana i tu mu je mesto
			nrKids++;
			Node myChild;
			myChild.address = buddy->addressToContact;
			myChild.id = createID(0, '0' + nrKids);
			
			neighbours.push_back(myChild);
			kids[nrKids - 1] = myChild;
		}
		else
		{

			toSend.addressToGoTo = neighbours[neighbourIndex].address;
			toSend.addressToContact = buddy->addressToContact;
			toSend.addressOfOrigin = i_sock;
			toSend.layer++;
			toSend.id = id;
			toSend.command = "FREE?";

			SendMyThing(toSend);
		}

	}
}

void FindFree(MyPacket* buddy)
{
	MyPacket toSend;
	toSend.layer = buddy->layer;
	if (amIFree())
	{ 
		nrKids++;
		Node myChild;
		myChild.address = buddy->addressToContact;
		myChild.id = createID(buddy->layer + 1, '0'+nrKids);

		neighbours.push_back(myChild);

		//kontaktirati buddySocket da nas doda u neighboure


	}
	else
	{
		int neighbourIndex = findNeighbourWithMaxId(toSend.layer);
		if (buddy->layer == id.length() - 1 || neighbourIndex == -1)
		{
			toSend.layer--;
			toSend.addressToGoTo = buddy->addressOfOrigin;
			toSend.addressToContact = buddy->addressToContact;
			toSend.addressOfOrigin = i_sock;
			toSend.id = id;
			toSend.command = "NOFREE";

			SendMyThing(toSend);

			//_beginthread(goBack, NULL, (void*)toSend);
			//vracaj nazad
		}
		else
		{

			toSend.addressToGoTo = neighbours[neighbourIndex].address;
			toSend.addressToContact = buddy->addressToContact;
			toSend.addressOfOrigin = i_sock;
			toSend.layer++;
			toSend.id = id;
			toSend.command = "FREE?";

			SendMyThing(toSend);
			
		}

	}
	//delete(buddy);
}
void NoFree(void* argv)
{
	MyPacket* buddy = (MyPacket*)argv;
	MyPacket toSend;
	if (buddy->id[buddy->layer] == '1')
	{
		//iz trenutnog cvora napravimo dvojku ovog layera
		toSend.id = createID(buddy->layer, '2');
		toSend.command = "FOUNDFREE";
		toSend.addressToGoTo = buddy->addressToContact;
		toSend.addressOfOrigin = i_sock;
		
		//kreiramo dvojku na layeru ovog cvora
		//dodamo ga kao neighbour-a
		Node newNode;
		newNode.address = buddy->addressToContact;
		newNode.id = toSend.id;
		neighbours.push_back(newNode);
		

		//TODO: povezemo sa svima sa kojima treba



	}
	if (buddy->id[buddy->layer] == '2')
	{
		//vracamo se nazad sa commandom "NOFREE"
		//toSend.command = "NOFREE";

		if (buddy->layer == 0)
		{
			//EXPANDING SHIT!

			
			//svim postojecim dati da prosire id-jeve 0
			addZeroToMyID();
			addZeroToID(*buddy);

			//kreirati novi sa id-jem 1xxxx;
			Node newNode;
			newNode.address = buddy->addressToContact;
			newNode.id = createID(0,'1');
			neighbours.push_back(newNode);

		}
		else
		{
			toSend.command = "NOFREE";
			toSend.layer = buddy->layer--;
			toSend.addressToGoTo = myZero.address;
			toSend.addressOfOrigin = i_sock;
			toSend.addressToContact = buddy->addressToContact;

			SendMyThing(toSend);
		}
	}
}
void ServerThing(void* argv)
{
	SOCKADDR_IN* buddySocket = (SOCKADDR_IN*)argv;
	MyPacket* buddy = new MyPacket();
	Recive((char*)&buddy, sizeof(MyPacket),sock);	
	MyPacket* toSend = new MyPacket();
	if (buddy->command.compare("ZERO"))
	{
		//vrati buddyu koga da kontaktira
		if (!first)
		{
			
			toSend->addressToGoTo = *buddySocket;
			toSend->addressToContact = myZero.address;
			toSend->addressOfOrigin = i_sock;
			toSend->command = "MYZERO";
			toSend->id = myZero.id;
			
			SendMyThing(*toSend);		

		}
		else
		{
			whereToGoFromZero(buddy);
			//_beginthread(whereToGo, NULL, (void*)buddySocket);
		}
	}
	if (buddy->command.compare("MYZERO"))
	{

		myZero.id = buddy->id;
		myZero.address = buddy->addressToGoTo;

		toSend->addressToGoTo = myZero.address;
		toSend->addressOfOrigin = i_sock;
		toSend->addressToContact = *buddySocket;
		toSend->command = "ZERO";

		SendMyThing(*toSend);

	}
	if (buddy->command.compare("ADDZEROTOID"))
	{
		addZeroToID(*buddy);
		//_beginthread(addZeroToID, NULL, (void*)buddySocket);
	}
	if (buddy->command.compare("FREE?"))
	{
		FindFree(buddy);
		//_beginthread(FindFree, NULL, (void*)buddy);
	}
	if (buddy->command.compare("NOFREE"))
	{
		_beginthread(NoFree, NULL, (void*)buddy);
	}

	//delete(toSend);
	//delete(buddy);
}
int main()
{
	//bootstrap
	BSConnect("127.0.0.1", 9999);
	//EndSocket(ss);

	WSADATA Data;
	WSAStartup(MAKEWORD(2, 2), &ServData);
	sock = socket(AF_INET, SOCK_STREAM, 0);
	printf("Made my listening socket...");

	if (sock == INVALID_SOCKET)
	{
		printf("Could not create socket! Error: %d", WSAGetLastError());
		system("PAUSE");
		return 1;
	}

	bind(sock, (LPSOCKADDR)&i_sock, sizeof(i_sock));
	listen(sock, 100);
	printf("LISTENING TO SOCKET...");
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