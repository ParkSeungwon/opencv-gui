#include<iostream>
#include"src/zgui.h"
using namespace std;

struct Win : z::AsciiWindow 
{
	Win() : z::AsciiWindow{R"(
		WCombined Widgets----------------
		|  T0-------------B0  T1------B1
		|  ||             ||  ||      B2
		|
		|
		|    C0-   C1-   C2-    B3------
		|    ||    ||    |v|    |values|
		|)"}
	{
		static vector<string> v{"abc", "def", "ghi"};
		tie(*T[0], *B[0], v, 30);
		auto f1 = tie(*T[1], *B[1], *B[2], 0, 1);
		auto f2 = tie(*C[0], *C[1], *C[2]);
		wrap("Radio Button", 20, 10, *C[0], *C[1], *C[2]);
		start();
		B[3]->click([this, f1, f2]() {
			cout << "combo box value : " << T[0]->value() << endl;
			cout << "Number spinner value: " << f1() << endl;
			cout << "Radio button value: " << f2() << endl;
		});
	}
};


int main() {
	Win win;
	win.loop();
}
