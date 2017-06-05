#include <windows.h>
#include <stdio.h> 
#include <string>
#include <thread>
#include <process.h>
#pragma comment(lib,"ws2_32.lib")

SOCKET sock; // this is the socket that we will use it 
SOCKADDR_IN i_sock; // this will containt some informations about our socket
WSADATA Data; // this is to save our socket version
Node* neighbours;
int nrNeighbours = 0;
bool zero = false;
Node me;
std::string myZero = "";
socketStruct whoToCall;


typedef struct
{
	std::string id;
	SOCKADDR_IN address;
	SOCKET socket;

}Node;

typedef struct {
	SOCKADDR_IN address;
	SOCKET socket;
}socketStruct;

/*int amIFree()
{
int k;
if (kids == 2) return "NO";
for(int i = 0; i < nuberofnei)
return 0;
}*/



int Send(char *Buf, int len, int Client)
{
	int slen;
	slen = send(sock, Buf, len, 0);
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

int EndSocket()
{
	closesocket(sock);
	WSACleanup();
	return 1;
}

int threadFunc(void* vArgv)
{

}

void splitString(std::string str, std::string* (&splitStr))
{
	char * pch;
	int i = 0;
	printf("Splitting string \"%s\" into tokens:\n", str);
	pch = strtok((char*)str.c_str(), "|");
	while (pch != NULL)
	{
		printf("%s\n", pch);
		splitStr[i] = pch;
		i++;
		pch = strtok(NULL, "|");
	}
	splitStr[i] = "!";
}
void serventMeme(void *vArgs)
{
	std::string str = (char*)vArgs;
	std::string* split;
	splitString(str, split);
}
int ConnectToServent(SOCKADDR_IN adr)
{
	WSAStartup(MAKEWORD(2, 2), &Data);
	sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock == INVALID_SOCKET)
	{
		printf("INVALID_SOCKET!\n");
		return 1;
	}
	i_sock.sin_family = AF_INET;
	i_sock.sin_addr.s_addr = adr.sin_addr.s_addr;
	i_sock.sin_port = adr.sin_port;
	int ss = connect(sock, (struct sockaddr *)&i_sock, sizeof(i_sock));
	if (ss != 0)
	{
		printf("Cannot connect to another servent!\n");
	}
	std::string recvString;
	std::string *splitStr;
	Recive((char*)&recvString, 256);
	_beginthread(serventMeme, NULL, (std::string*)&recvString);
	splitString(recvString, splitStr);

	/*printf("I should connect to (IP: %d.%d.%d.%d - PORT: %d)\n", int(mp.address.sin_addr.s_addr & 0xFF),
	int((mp.address.sin_addr.s_addr & 0xFF00) >> 8),
	int((mp.address.sin_addr.s_addr & 0xFF0000) >> 16),
	int((mp.address.sin_addr.s_addr & 0xFF000000) >> 24), mp.address.sin_port);*/
	printf("Succefully connected");
	return 1;
}

int Connect(char *IP, int Port, std::string &portToContact)
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
	std::string recvString;
	std::string *splitStr;
	Recive((char*)&recvString, 256);
	_beginthread(serventMeme, NULL, (std::string*)&recvString);
	splitString(recvString, splitStr);
	while (splitStr != "sdasd")
		/*printf("I should connect to (IP: %d.%d.%d.%d - PORT: %d)\n", int(mp.address.sin_addr.s_addr & 0xFF),
		int((mp.address.sin_addr.s_addr & 0xFF00) >> 8),
		int((mp.address.sin_addr.s_addr & 0xFF0000) >> 16),
		int((mp.address.sin_addr.s_addr & 0xFF000000) >> 24), mp.address.sin_port);*/
		printf("Succefully connected");
	return 1;
}


int main()
{
	//bootstrap
	std::string portToContact = "";
	Connect("127.0.0.1", 9999, portToContact);
	//zatvori
	EndSocket();
	//primi gde ides i konektuj se

	//
	EndSocket();
	system("PAUSE");

	return 0;
}