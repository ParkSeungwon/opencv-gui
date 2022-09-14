#include<iostream>
#include"zgui.h"
using namespace std;


struct Win2 : z::AsciiWindow
{
	Win2() : z::AsciiWindow{R"(
		WMessage---------------------------
		|
		|    L0--------------------------
		|    ||
		|    L1--------------------------
		|    ||
		|
		|    B0----------    B1----------
		|    |Yes|           |No|
		|)"}
	{
		B[0]->click([this]() { cout << '0' << flush; });
		B[1]->click([this]() { cout << '1' << flush; });
		organize_accordingto_zindex();
	}

	void set(string s1, string s2) {
		L[0]->text(s1); L[1]->text(s2);
		*this << *L[0]; *this << *L[1];
	}
};

struct Win : z::Window
{
	Win() : z::Window{"win", {0, 0, 500, 500}}
	{
		*this + win2;
		win2.x = 10;
		win2.y = 100;
		start();
	}
	Win2 win2;
};

int main() {
	Win win;
	win.loop();
}
