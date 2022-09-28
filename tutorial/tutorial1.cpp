#include<iostream>
#include"src/zgui.h"
using namespace std;

struct Win : z::AsciiWindow 
{
	Win() : z::AsciiWindow{R"(
	  WHello World----------------
		| T0-------------------
		| |Enter your name|
		| B0-------------
		| |Hello|
		|)"}
	{
		start();
		B[0]->click([this]() { cout << "Hello " << T[0]->value() << endl; });
	}
};

int main() {
	Win win;
	win.loop();
}


