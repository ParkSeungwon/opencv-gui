#include<cstdlib>
#include<fstream>
#include"option.h"
#include"zgui.h"
using namespace std;

struct Mywin : z::AsciiWindow
{
	Mywin(double scale) : z::AsciiWindow{R"(
		WPrecepts--------------------------------------------
		|
		|                            C3-L3-------------
		|      C0-L0---------        || |暝想 2H|
		|      || |108-1|            C4-L4-------------
		|      C1-L1---------        || |經典 읽기|
		|      || |108-2|            C5-L5-------------
		|      C2-L2---------        || |운동|
		|      || |108-3|            C6-L6-------------
		|                            || |걷기|
		|     
		|
		|     B0-------------        B1---------------
		|     |새로운 시작|     |저장 후 종료|
		|
		|)", static_cast<int>(scale * 20), static_cast<int>(scale * 30)}
	{
		string home{getenv("HOME")}, s;
		bool tf;
		ifstream f{home + "/Dropbox/precepts.txt"};
		for(int i=0; i<7; i++) f >> tf, getline(f, s), C[i]->checked(tf), update(*C[i]);
		wrap("百八拜", 30*scale, 20*scale, *C[0], *L[2]);
		start();
		cv::moveWindow(title_, 700, 500);

		B[0]->click( [this]() { 
				for(int i=0; i<7; i++) C[i]->checked(false), update(*C[i]);
				show();
		} );
		B[1]->click( [this, home]() {
				ofstream f{home + "/Dropbox/precepts.txt"};
				for(int i=0; i<7; i++) f << C[i]->checked() << ' ' << L[i]->text() << endl;
				close();
		} );
	}
};

int main(int ac, char **av)
{
	CMDoption co{
		{"scale", "show scale", 1.0}	
	};
	if(!co.args(ac, av)) return 0;

	Mywin win(co.get<double>("scale"));
	win.loop();
}
