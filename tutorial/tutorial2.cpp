#include<iostream>
#include"src/zgui.h"
using namespace std;

struct Win : z::AsciiWindow 
{
	Win() : z::AsciiWindow{R"(
		WBasic Widget Gallery------------------
		|  L0-----B0-------C0- T0--------------
		|  |Label||Button| |v| |TextInput| 
		|  S0---------------  P0-------------
		|  |1 100 1|          |333|
		|  E0---------------------------------
		|  |Text Box|
		|  |
		|  |
		|)"}
	{
		start();
	}
};

int main() {
	Win win;
	win.loop();
}
