#include<iostream>
#include"src/zgui.h"
using namespace std;

struct NewWin : z::Window
{
	NewWin() : z::Window{"title", {0,0,1,1}}
	{
		gui_callback_[cv::EVENT_LBUTTONUP] = [this](int x, int y) {
			cv::circle(mat_, {x, y}, 30, {255, 0, 0}, 3);
			show();
		};
	}
};

struct Win : z::AsciiWindow
{
	Win() : z::AsciiWindow{R"(
	  WMatrix Window exmaple------------
		|
		|  B0----------------------
		|  |Window with event|
		|
		|  B1-----------------------
		|  |Just show the image|
		|)"}
	{
		start();
		B[0]->click([this]() { w.load_matrix( cv::imread("Lenna.png")); w.open(); });
		B[1]->click([this]() { imshow("image", cv::imread("len.jpg")); });
	}
	NewWin w;
};

int main() {
	Win win;
	win.loop();
}


