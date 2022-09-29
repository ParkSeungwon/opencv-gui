#include<iostream>
#include"src/zgui.h"
using namespace std;

struct Popup : z::AsciiWindow
{
	Popup() : z::AsciiWindow {R"(
		W-----------
		|
		|  B0-----
		|  |OK|
		|)"}
	{
		organize_accordingto_zindex();
		B[0]->click([this]() { popdown(7); });
	} 
};

struct Win : z::AsciiWindow 
{
	Popup pop;
	Win() : z::AsciiWindow{R"(
	  WPopup Example----------
		|
		|   B0--------------
		|   |Popup|
		|
		|)"}
	{
		start();
		B[0]->click( [this]() { 
			pop.popup ( *this , [](int i) { cout << i << endl;} );//7
		});
	}
};

int main() {
	Win win;
	win.loop();
}
