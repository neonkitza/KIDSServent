#include <windows.h>
#include <stdio.h> 
#include <string>
#include <thread>
#include <process.h>
#include <vector>
#include <iostream>
#include <algorithm>
#include "commands.h"
#include "util.h"
#include "messages .h"
#include <time.h>

#pragma comment(lib,"ws2_32.lib")

//port chaosGame-a = 10000
#define chaosPort 10000

void startChaos(void* argv);
void drawImg(cv::Mat &img);

using namespace std;

void addZeroToID(MyPacket);
void SendMyThing(MyPacket);

SOCKET sock; // this is the server socket
SOCKET fractalSock;
SOCKADDR_IN i_sock; // this will containt some informations about our server socket
SOCKADDR_IN i_sock_Fractal;
WSADATA ServData; // this is to save our socket version
std::vector<Node> neighbours;
int nrNeighbours = 0;
int nrKids = 0;
Node kids[2];
bool first = false;
char id[20];
int layer;
//socketStruct whoToCall;
Node myZero;
int howManyInSystem = 0;

bool draw = false;
bool working = false;
bool quitting = false;

int nrOfPoints, width, height;
double distanceP;
cv::Mat display_image;
//vector<POINT> startingPoints;


void beginChaos(void* argv);

void printID(char id[])
{
	std::cout << "my ID: ";
	for (int i = 0; i < 20; i++) {
		std::cout << id[i];
	}
	std::cout << std::endl;
}

int Send(SOCKET sockToSend, char *Buf, int len)
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

int Recive(char *Buf, int len, SOCKET sock)
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
	i_sockOUT.sin_addr.s_addr = inet_addr("127.0.0.1");
	i_sockOUT.sin_port = adr.sin_port;

	int ss = connect(sockOUT, (struct sockaddr *)&i_sockOUT, sizeof(i_sockOUT));
	if (ss != 0)
	{

		printf("Cannot connect to another servent!\n");
		std::cout << "error connecting to another servent: " << WSAGetLastError() << std::endl;
	}

	return sockOUT;
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

	printf("My port is: %d\n", i_sock.sin_port);

	int ss = connect(BSsock, (struct sockaddr *)&i_sock, sizeof(i_sock));
	if (ss != 0)
	{
		printf("Cannot connect\n");
	}
	BSPacket mp;
	Recive((char*)&mp, sizeof(BSPacket), BSsock);
	height = mp.height;
	width = mp.width;
	//dal je goto ili first
	if (mp.command == GOTO)
	{

		i_sock.sin_family = AF_INET;
		i_sock.sin_addr.s_addr = htonl(INADDR_ANY);
		i_sock.sin_port = mp.me.sin_port;
		printf("My port is: %d\n", i_sock.sin_port);
		printf("I should GOTO: %d\n", mp.whereTo.sin_port);
		//bind(sock, (LPSOCKADDR)&i_sock, sizeof(i_sock));

		MyPacket toSend;

		toSend.command = ZERO;
		toSend.addressToGoTo = mp.whereTo;
		toSend.addressOfOrigin = i_sock;
		toSend.addressToContact = i_sock;
		//me.address = i_sock;
		myZero.address = i_sock;
		//myZero.id

		SendMyThing(toSend);
	}
	else if (mp.command == YOUAREZERO)
	{
		first = true;
		howManyInSystem = 1;
		for (int i = 0; i < 20; i++)
			id[i] = '0';
		printID(id);
		i_sock.sin_family = AF_INET;
		i_sock.sin_addr.s_addr = htonl(INADDR_ANY);
		i_sock.sin_port = mp.me.sin_port;

		printf("My port is: %d\n", i_sock.sin_port);
	}
	EndSocket(BSsock);
}

void SendMyThing(MyPacket toSend)
{
	printf("Sending command: %d", toSend.command);
	std::cout << " to port :" << toSend.addressToGoTo.sin_port << std::endl;

	SOCKET buddySock = ConnectToServent(toSend.addressToGoTo);
	Send(buddySock, (char*)&toSend, sizeof(MyPacket));
	EndSocket(buddySock);
}


bool amIFree()
{
	if (nrKids == 2) return false;
	int k = 0;
	for (int i = 0; i < neighbours.size(); i++)
	{
		if (id[19] == '1' || id[19] == '2')
		{
			k++;
			nrKids++;
		}
		if (k == 2)
		{
			nrKids = 2;
			return false;
		}
	}
	return true;
}
void createID(int layer, char whichOne, char* newID)
{
	strncpy(newID, id, 20);
	newID[layer] = whichOne;
}

void broadcastMSG(MyPacket toSend)
{
	//int layer = toSend.layer;
	for (int l = layer; l < 20; l++)
	{
		for (int i = 0; i < neighbours.size(); i++)
		{
			if (neighbours[i].id[l] > id[l])
			{
				//send the msg
				toSend.addressToGoTo = neighbours[i].address;
				toSend.layer = l + 1;
				SendMyThing(toSend);
			}
		}
	}

}

int findNeighbourWithMaxId(int& layer)
{
	char max[20];
	strncpy(max, id, 20);
	int temp = -1;
	for (int l = layer; l < 19; l++)
	{
		for (int i = 0; i < neighbours.size(); i++)
		{
			if (neighbours[i].id[l] > max[l] && neighbours[i].id[19] == '0')//maybe
			{
				temp = i;
				strncpy(max, neighbours[i].id, 20);
			}
		}
		if (temp != -1)
		{
			layer = l;
			break;
		}
	}

	return temp;
}


void sendToNodesUnderMe(MyPacket* buddy, MyPacket toSend)
{
	for (int l = layer; l < 20; l++)
	{
		for (int i = 0; i < neighbours.size(); i++)
		{
			if (neighbours[i].id[l] > id[l])
			{
				//send the msg
				toSend.addressToGoTo = neighbours[i].address;
				toSend.layer = l + 1;
				SendMyThing(toSend);
			}
		}
	}
}
void addPointStuff(MyPacket& toSend)
{
	toSend.r = r100 / 100.0;
	toSend.nrPoints = point_set.size();

	for (int i = 0; i < nrOfPoints; i++)
	{
		toSend.point_set[i] = point_set[i];
	}
}
void whereToGoFromZero(MyPacket* buddy)
{
	MyPacket toSend;
	toSend.layer = 19;
	buddy->layer = 19;
	if (amIFree())
	{
		nrKids++;
		howManyInSystem++;
		Node myChild;
		myChild.address = buddy->addressToContact;

		createID(toSend.layer, '0' + nrKids, myChild.id);
		strncpy(toSend.id, myChild.id, 20);

		
		kids[nrKids - 1] = myChild;

		addPointStuff(toSend);

		toSend.command = FREE;
		toSend.addressToGoTo = buddy->addressToContact;
		toSend.addressOfOrigin = i_sock;
		//kontaktirati buddy->addressToContact da nas doda u neighboure
		SendMyThing(toSend);



		//naci sve sa kojima treba da smo spojeni i reci im da nas spoje
		//to su oni koji su na istom layeru kao novi cvor
		MyPacket connectMe;
		connectMe.addressOfOrigin = i_sock;
		for (int i = 0; i < neighbours.size(); i++)
		{
			if (id[toSend.layer] != neighbours[i].id[toSend.layer])
			{
				connectMe.addressToGoTo = neighbours[i].address;
				connectMe.command = CONNECTME;
				strncpy(connectMe.id, myChild.id, 20);
				//posalji
				SendMyThing(connectMe);
			}
		}
		neighbours.push_back(myChild);

		if (howManyInSystem == 3)
		{
			for (int i = 0; i < nrKids; i++)
			{
				connectMe.addressToGoTo = kids[i].address;
				connectMe.command = STARTCHAOS;
				//posalji
				SendMyThing(connectMe);
			}
		}


	}
	else
	{
		int tempLayer = 0;
		int neighbourIndex = findNeighbourWithMaxId(tempLayer);
		if (neighbourIndex == -1)
		{

			//mora da se grana i tu mu je mesto

			Node myChild;
			myChild.address = buddy->addressToContact;
			toSend.layer = buddy->layer - 1; //valjda 18
			createID(toSend.layer, '1', myChild.id);
			strncpy(toSend.id, myChild.id, 20);

			addPointStuff(toSend);

			toSend.command = FREE;
			toSend.addressToGoTo = buddy->addressToContact;
			toSend.addressOfOrigin = i_sock;
			SendMyThing(toSend);

			MyPacket connectMe;
			connectMe.addressOfOrigin = i_sock;
			for (int i = 0; i < neighbours.size(); i++)
			{
				if (id[toSend.layer] != neighbours[i].id[toSend.layer])
				{
					connectMe.addressToGoTo = myChild.address;
					connectMe.command = CONNECTME;
					strncpy(connectMe.id, myChild.id, 20);
					//posalji
					SendMyThing(connectMe);
				}
			}
			neighbours.push_back(myChild);

		}
		else
		{
			//kontaktiraj neighboura kog si nasao
			toSend.addressToGoTo = neighbours[neighbourIndex].address;
			toSend.addressToContact = buddy->addressToContact;
			toSend.addressOfOrigin = i_sock;
			toSend.layer = tempLayer;
			strncpy(toSend.id, id, 20);

			toSend.command = AREYOUFREE;

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

		strncpy(myChild.id, id, 20);
		createID(toSend.layer+1, '0' + nrKids, myChild.id);
		strncpy(toSend.id, myChild.id, 20);

		
		kids[nrKids - 1] = myChild;

		addPointStuff(toSend);

		toSend.command = FREE;
		toSend.addressToGoTo = buddy->addressToContact;
		toSend.addressOfOrigin = i_sock;
		//kontaktirati buddy->addressToContact da nas doda u neighboure
		SendMyThing(toSend);

		toSend.layer++;
		//naci sve sa kojima treba da smo spojeni i reci im da nas spoje
		//to su oni koji su na istom layeru kao novi cvor
		MyPacket connectMe;
		connectMe.addressOfOrigin = i_sock;
		for (int i = 0; i < neighbours.size(); i++)
		{
			if (id[toSend.layer] != neighbours[i].id[toSend.layer])
			{
				connectMe.addressToGoTo = neighbours[i].address;
				connectMe.command = CONNECTME;
				strncpy(connectMe.id, myChild.id, 20);
				//posalji
				SendMyThing(connectMe);
			}
		}
		neighbours.push_back(myChild);

	}
	else
	{
		int neighbourIndex = findNeighbourWithMaxId(toSend.layer);
		if (neighbourIndex == -1)
		{

			

			
			//vracaj nazad
			if (id[toSend.layer] == '2')
			{
				toSend.layer = buddy->layer;
				toSend.addressToGoTo = buddy->addressOfOrigin;
				toSend.addressToContact = buddy->addressToContact;
				toSend.addressOfOrigin = i_sock;
				strncpy(toSend.id, id, 20);
				toSend.command = NOFREE;

				SendMyThing(toSend);
			}
			else
			{
				//pravi se dvojka

				Node myChild;
				myChild.address = buddy->addressToContact;
				//toSend.layer = buddy->layer; //valjda
				createID(toSend.layer, '2', myChild.id);
				strncpy(toSend.id, myChild.id, 20);

				
				addPointStuff(toSend);

				toSend.command = FREE;
				toSend.addressToGoTo = buddy->addressToContact;
				toSend.addressOfOrigin = i_sock;
				SendMyThing(toSend);

				MyPacket connectMe;
				connectMe.addressOfOrigin = i_sock;
				for (int i = 0; i < neighbours.size(); i++)
				{
					if (id[toSend.layer] != neighbours[i].id[toSend.layer])
					{
						connectMe.addressToGoTo = myChild.address;
						connectMe.command = CONNECTME;
						strncpy(connectMe.id, myChild.id, 20);
						//posalji
						SendMyThing(connectMe);
					}
				}
				neighbours.push_back(myChild);

			}

		}
		else
		{
			//toSend.layer++;
			toSend.addressToGoTo = neighbours[neighbourIndex].address;
			toSend.addressToContact = buddy->addressToContact;
			toSend.addressOfOrigin = i_sock;
			strncpy(toSend.id, id, 20);

			toSend.command = AREYOUFREE;

			SendMyThing(toSend);

		}

	}
	//delete(buddy);
}
void FoundFree(MyPacket* buddy)
{
	printf("FOUND FREE!!!!\n");

	myZero.address = buddy->addressOfOrigin;
	strncpy(id, buddy->id, 20);
	strncpy(myZero.id, buddy->id, 20);
	myZero.id[buddy->layer] = '0';

	std::cout << "My zero's id: ";
	printID(myZero.id);

	Node newNode;
	newNode.address = buddy->addressOfOrigin;
	strncpy(newNode.id, buddy->id, 20);

	std::cout << "My id: ";
	printID(id);

	//izvuci iz buddy-a chaos stuff
	
	r100 = buddy->r * 100;
	nrOfPoints = buddy->nrPoints;

	point_set.clear();
	for (int i = 0; i < nrOfPoints; i++)
	{
		point_set.push_back(buddy->point_set[i]);
	}
	
}
void ConnectMe(MyPacket* buddy)
{
	Node myBuddy;
	myBuddy.address = buddy->addressOfOrigin;
	strncpy(myBuddy.id, buddy->id, 20);
	neighbours.push_back(myBuddy);


	std::cout << "Connected buddy id: " << myBuddy.id << " and port: " << myBuddy.address.sin_port << std::endl;
	printID(myBuddy.id);
}

void NoFree(MyPacket* buddy)
{
	MyPacket toSend;
	if (buddy->id[buddy->layer] == '1')
	{
		//iz trenutnog cvora napravimo dvojku ovog layera
		addPointStuff(toSend);
		toSend.command = FREE;
		toSend.addressToGoTo = buddy->addressToContact;
		toSend.addressOfOrigin = i_sock;
		toSend.layer = buddy->layer;



		//kreiramo dvojku na layeru ovog cvora
		//dodamo ga kao neighbour-a
		Node newNode;
		newNode.address = buddy->addressToContact;
		//kreairamo id
		strncpy(toSend.id, id, 20);
		createID(buddy->layer, '2', toSend.id);
		strncpy(newNode.id, toSend.id, 20);
		

		SendMyThing(toSend);
		//TODO: povezemo sa svima sa kojima treba

		MyPacket connectMe;
		connectMe.addressOfOrigin = i_sock;


		for (int i = 0; i < neighbours.size(); i++)
		{
			if (id[toSend.layer + 1] != neighbours[i].id[toSend.layer + 1])
			{
				connectMe.addressToGoTo = neighbours[i].address;
				connectMe.command = CONNECTME;
				strncpy(connectMe.id, toSend.id, 20);
				connectMe.layer = toSend.layer + 1;
				//posalji
				SendMyThing(connectMe);
			}
		}
		neighbours.push_back(newNode);



	}
	if (buddy->id[buddy->layer] == '2')
	{
		//vracamo se nazad sa commandom "NOFREE"
		toSend.layer = buddy->layer - 1;
		toSend.addressToGoTo = myZero.address;
		toSend.addressToContact = buddy->addressToContact;
		toSend.addressOfOrigin = i_sock;
		//toSend.id = id;
		strncpy(toSend.id, id, 20);
		toSend.command = NOFREE;


		if (first)
		{
			//EXPANDING SHIT!



			//kreirati novi sa id-jem 1xxxx;
			Node newNode;
			newNode.address = buddy->addressToContact;
			createID(buddy->layer - 1, '1', toSend.id);
			strncpy(newNode.id, toSend.id, 20);
			//newNode.id[buddy->layer-1] = '1'; //FIX
			addPointStuff(toSend);
			toSend.command = FREE;


			//SendMyThing(toSend);

			neighbours.push_back(newNode);
		}
		SendMyThing(toSend);
	}
}
void recvImg(cv::Mat& img, SOCKET socket)
{
	int  imgSize = img.total()*img.elemSize();

	uchar* sockData = (uchar *)malloc(sizeof(int) * imgSize);
	//uchar sockData[imgSize];

	//Receive data here
	int bytes = 0;
	for (int i = 0; i < imgSize; i += bytes) {
		if ((bytes = recv(socket, (char*)sockData + i, imgSize - i, 0)) == -1) {
			printf("recv POINTS failed\n");
		}
	}

	// Assign pixel value to img

	int ptr = 0;
	for (int i = 0; i < img.rows; i++) {
		for (int j = 0; j < img.cols; j++) {
			img.at<cv::Vec3b>(i, j) = cv::Vec3b(sockData[ptr + 0], sockData[ptr + 1], sockData[ptr + 2]);
			ptr = ptr + 3;
		}
	}

	delete(sockData);
}

void switchWithBuddy(MyPacket* buddy,SOCKET socket)
{
	MyPacket toSend;
	toSend.command = STOPCHAOS;
	cv::Mat  img = cv::Mat::zeros(height, width, CV_8UC3);
	recvImg(img, socket);

	if (buddy->id[19] == '0')
	{
		first = true;
		//onda nas menja
		//nula salje QUITTING najvecem detetu
		strncpy(id, buddy->id, 20);
		i_sock = buddy->addressOfOrigin;
		if (id[19] == '1')
		{
			nrKids = 0;
		}
		else
		{
			nrKids = 1;
		}
		toSend.addressToGoTo = buddy->addressToContact;
		//pokrenuti prozor/chaos
		_beginthread(startChaos, NULL, NULL);
	}
	else 
	{
		nrKids--;
		if (buddy->id[19] == '1')
		{
			toSend.addressToGoTo = kids[1].address;
			strncpy(toSend.id, id, 20);
			createID(19, '1', toSend.id);

			kids[0] = kids[1];
		}
		else
		{
			toSend.addressToGoTo = kids[0].address;
			
		}
		SendMyThing(toSend);
	}
}

void ServerThing(void* argv)
{
	socketStruct* buddySocket = (socketStruct*)argv;
	MyPacket* buddy = new MyPacket();
	Recive((char*)buddy, sizeof(MyPacket), buddySocket->socket);
	MyPacket* toSend = new MyPacket();
	if (buddy->command == ZERO)
	{
		//vrati buddyu koga da kontaktira
		if (!first)
		{
			toSend->addressToGoTo = buddy->addressOfOrigin;
			toSend->addressToContact = myZero.address;
			toSend->addressOfOrigin = i_sock;
			toSend->command = MYZERO;
			strncpy(toSend->id, myZero.id, 20);

			SendMyThing(*toSend);
			delete(toSend);
		}
		else
		{
			whereToGoFromZero(buddy);
		}
	}
	else if (buddy->command == MYZERO)
	{
		strncpy(myZero.id, buddy->id, 20);
		myZero.address = buddy->addressToContact;

		toSend->addressToGoTo = myZero.address;
		toSend->addressOfOrigin = i_sock;
		toSend->addressToContact = i_sock;
		toSend->command = ZERO;

		SendMyThing(*toSend);

		delete(toSend);

	}
	else if (buddy->command == AREYOUFREE)
	{
		FindFree(buddy);
		//_beginthread(FindFree, NULL, (void*)buddy);
	}
	else if (buddy->command == NOFREE)
	{
		NoFree(buddy);
	}
	else if (buddy->command == FREE)
	{
		FoundFree(buddy);
	}
	else if (buddy->command == CONNECTME)
	{
		ConnectMe(buddy);
	}
	else if (buddy->command == STARTCHAOS)
	{
		_beginthread(beginChaos, NULL, NULL);
	}
	else if (buddy->command == QUITTING)
	{
		//treba da nas zameni buddy
		switchWithBuddy(buddy,buddySocket->socket);
	}
	else if (buddy->command == POINTS)
	{
		cv::Mat  img = cv::Mat::zeros(height, width, CV_8UC3);
		
		recvImg(img, buddySocket->socket);

		drawImg(img);
	}
	else if (buddy->command == STOPCHAOS)
	{
		working = false;
		draw = false;
	}
	//delete(toSend);
	//delete(buddy);
}
void sendImg(MyPacket toSend)
{
	SOCKET imgSock = ConnectToServent(toSend.addressToGoTo);
	display_image = display_image.reshape(0, 1);
	int  imgSize = display_image.total()*display_image.elemSize();

	int bytes = send(imgSock, (char*)display_image.data, imgSize, 0);

	EndSocket(imgSock);
}
void sendImgToDraw()
{
	MyPacket toSend;
	toSend.command = POINTS;
	toSend.addressOfOrigin = i_sock;
	strncpy(toSend.id, id, 20);
	toSend.addressToGoTo = myZero.address;

	SendMyThing(toSend);
	
	sendImg(toSend);

}
void waitForQuit(void* argv)
{

	std::cout << "Q to quit";
	std::string s = " ";
	while(s[0]!='q')
		std::cin >> s;
	if (s[0] == 'q' || s[0] == 'Q')
	{
		quitting = true;
		working = false;
		MyPacket toSend;
		toSend.command = QUITTING;
		toSend.addressOfOrigin = i_sock;
		strncpy(toSend.id, id, 20);

		if (id[19] == '0')
		{
			int neighbourIndex = findNeighbourWithMaxId(toSend.layer);
			if (neighbourIndex == -1)
			{
				if (nrKids != 0)
				{
					toSend.addressToGoTo = kids[nrKids - 1].address;
					toSend.addressToContact = kids[0].address;

					SendMyThing(toSend);

					//saljemo bajtove slike
					sendImg(toSend);

				}
				else return; //GG
			}
			else
			{
				//treba da posalje tom neighbouru da nadje onog sa najvecim id-em da zameni nulu

				//ali posto min spec = 3 cvora, ovo nije preterano bitno :p
			}
		}
		else
		{
			//poslati nuli da quitujemo i ono sto smo sracunali do sada
			toSend.addressToGoTo = myZero.address;
			SendMyThing(toSend);

			sendImg(toSend);
		}

	}

	//poslati na cvor sa najvecim id-em da treba da nas zameni
	

}

int main()
{
	//bootstrap
	BSConnect("127.0.0.1", 9999);

	display_image = cv::Mat(width, height, CV_8UC3, cv::Scalar(0, 0, 0));
	if (first)
	{
		draw = true;
		_beginthread(startChaos, NULL, NULL);
		
	}
	else
	{
		cv::Mat display_image(width, height, CV_8UC3, cv::Scalar(0, 0, 0));

	}

	_beginthread(waitForQuit, NULL, NULL);
	

	WSADATA Data;
	WSAStartup(MAKEWORD(2, 2), &ServData);
	sock = socket(AF_INET, SOCK_STREAM, 0);
	printf("Made my listening socket...\n");

	if (sock == INVALID_SOCKET)
	{
		printf("Could not create socket! Error: %d\n", WSAGetLastError());
		system("PAUSE");
		return 1;
	}

	bind(sock, (LPSOCKADDR)&i_sock, sizeof(i_sock));
	listen(sock, 10000);
	printf("LISTENING TO PORT: %d\n", i_sock.sin_port);
	while (1)
	{
		if(quitting) break;
		SOCKET myBuddy;
		SOCKADDR_IN i_sock2;
		int so2len = sizeof(i_sock2);
		myBuddy = accept(sock, (sockaddr *)&i_sock2, &so2len);
		if (myBuddy != -1)
		{
			socketStruct* ss = new socketStruct();
			ss->address = i_sock2;
			ss->socket = myBuddy;
			if (ss->address.sin_port != 9999) 
			{
				_beginthread(ServerThing, NULL, (void*)ss);
			}
		}

	}

	EndSocket(sock);

	return 0;
}