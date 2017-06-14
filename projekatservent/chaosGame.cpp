#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>
#include <vector>
#include <cstdlib>
#include <Windows.h>
#include <process.h>
#include "commands.h"
#include "messages .h"
#include "util.h"
#include <WinBase.h>
#pragma comment(lib,"ws2_32.lib")

using namespace cv;
using namespace std;
void sendImgToDraw();
const char * window_name = "Chaos game (c) Jay Bosamiya";
int max_iterations = (int)1e5;
int display_every = (int)1e2;

vector<Point> point_set;
Point centre;
int r100 = 50;
bool colouring_on = true;
double distance;
void display_fractal();
void display_points();
void recalculate_centre();

void delete_point() {
	if (point_set.size() >= 1) {
		point_set.pop_back();
	}
	recalculate_centre();
	display_points();
}

void recalculate_centre() {
	centre = Point(0, 0);
	for (vector<Point>::iterator it = point_set.begin(); it != point_set.end(); it++) {
		centre += *it;
	}
	centre = centre * (1.0 / point_set.size());
}

void waiter(int delay = 0) {
start_of_waiter:
	char c = waitKey(delay);
	switch (c) {
	case 'f':
		display_fractal();
		break;
	case 'c':
		colouring_on = !colouring_on;
		display_fractal();
		break;
	case 'q':
		exit(0);
		break;
	case 'x':
		delete_point();
		break;
	default:
		if (delay == 0) {
			goto start_of_waiter;
		}
		break;
	}
}

void create_fractal(Mat &img, Point p) {
	if (point_set.size() < 1) {
		return;
	}

	if (display_every < 1) {
		display_every = 1;
	}

	RNG rng;
	double r = r100 / 100.0;

	vector<Vec3b> colour_palette;
	colour_palette.push_back(Vec3b(173, 35, 35));
	colour_palette.push_back(Vec3b(42, 75, 215));
	colour_palette.push_back(Vec3b(29, 105, 20));
	colour_palette.push_back(Vec3b(129, 74, 25));
	colour_palette.push_back(Vec3b(129, 38, 192));
	colour_palette.push_back(Vec3b(160, 160, 160));
	colour_palette.push_back(Vec3b(129, 197, 122));
	colour_palette.push_back(Vec3b(157, 175, 255));
	colour_palette.push_back(Vec3b(41, 208, 208));
	colour_palette.push_back(Vec3b(255, 146, 51));
	colour_palette.push_back(Vec3b(255, 238, 51));
	colour_palette.push_back(Vec3b(233, 222, 187));
	colour_palette.push_back(Vec3b(255, 205, 243));

	for (int i = 0; i < 100; i++) {
		int selection = rng.uniform(0, point_set.size());
		Point towards = point_set[selection];
		p = r*p + (1 - r)*towards;

		if (colouring_on && selection < colour_palette.size()) {
			img.at<Vec3b>(p)[0] = colour_palette[selection][0];
			img.at<Vec3b>(p)[1] = colour_palette[selection][1];
			img.at<Vec3b>(p)[2] = colour_palette[selection][2];
		}
		else {
			img.at<Vec3b>(p)[0] = 255;
			img.at<Vec3b>(p)[1] = 255;
			img.at<Vec3b>(p)[2] = 255;
		}

		if (draw && first) {
			imshow(window_name, img);
			//draw = false;
		}
		//Sleep(1000);
		//BANE PLS
	}
}
void drawImg(Mat &img)
{
	imshow(window_name, img);
}
void display_fractal() {
	//Mat display_image(width, height, CV_8UC3, Scalar(0, 0, 0));

	if (point_set.size() < 1) {
		waiter();
		return;
	}

	create_fractal(display_image, centre);

	waiter();
}

void display_points() {

	for (vector<Point>::iterator it = point_set.begin(); it != point_set.end(); it++) {
		circle(display_image, *it, 5, Scalar(255, 255, 255), -1, 8);
	}
	if (point_set.size() >= 1) {
		circle(display_image, centre, 5, Scalar(0, 255, 0), -1, 8);
		circle(display_image, *(point_set.rbegin()), 5, Scalar(0, 0, 255), -1, 8);
	}

	imshow(window_name, display_image);
	waiter();
}

void on_mouse(int event, int x, int y, int, void*) {
	if (event == EVENT_LBUTTONDOWN) {
		point_set.push_back(Point(x, y));
		recalculate_centre();
		display_points();
	}
	else if (event == EVENT_RBUTTONDOWN) {
		centre = Point(x, y);
		display_points();
	}
}
void beginChaos(void* argv)
{
	
	//Mat display_image(width, height, CV_8UC3, Scalar(0, 0, 0));
	recalculate_centre();
	create_fractal(display_image, centre);
	/*while (working)
	{
		//display_fractal();
	}*/
	
}

int startWindow()
{
	namedWindow(window_name, CV_WINDOW_AUTOSIZE);

	cout << "Usage:\n"
		"\tLeft Click: Add new point\n"
		"\tRight Click: Define starting point\n"
		"\tKeypress x: Delete latest point\n"
		"\tKeypress f: Run the chaos game\n"
		"\tKeypress c: Toggle colouring\n"
		"\tKeypress q: Quit\n";

	setMouseCallback(window_name, on_mouse);
	createTrackbar("r*100: ", window_name, &r100, 100, NULL);
	createTrackbar("Draw speed: ", window_name, &display_every, 100, NULL);
	return 0;
}
void startChaos(void* argv)
{
	working = true;
	startWindow();
	display_points();
}