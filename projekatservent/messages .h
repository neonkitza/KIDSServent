#include <string>
#include <windows.h>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <cstdlib>
#pragma once


struct BSPacket
{
	int command;
	SOCKADDR_IN whereTo;
	SOCKADDR_IN me;
	int width;
	int height;
};

struct PointStruct
{
	int command;
	cv::Mat mat;
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
	cv::Point point_set[10];
	int nrPoints;
	double r;
};

struct socketStruct
{
	SOCKADDR_IN address;
	SOCKET socket;
};