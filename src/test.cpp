#include<filesystem>
#include<fstream>
#include<iostream>
#include<opencv2/freetype.hpp>
#include"zgui.h"
using namespace std;

struct Win : z::AsciiWindow
{
	int line = 30;
	z::TextBox text{{0, 70, 850, 900}, 30};
	Win() : z::AsciiWindow{R"(
	  WZETA Editor-------------------------------------------------------------------------
		| 
		|  T0------------------B4 B0------  B1------  B3-L0------B2- 
		|  ||                  || |열기|    |저장|    |-||폰트|  |+|   
		|
	)"}
	{
		static vector<string> v;
		for(const filesystem::path &p : filesystem::directory_iterator("./"))
			if(is_regular_file(p) && p.extension() != ".txt" && p.extension() != ".jpg"
					&& p.extension() != ".x")
				v.push_back(p.filename());
		resize({0, 0, 850, 970});
		scrolled_rect_ = *this;
		*this + text;
		tie(*T[0], *B[4], v, 30);

		start();

		B[0]->click([this](){
			ifstream f{T[0]->value()};
			string s;
			for(char c; f >> noskipws >> c; ) s += c;
			text.value(s);
		});
		B[1]->click([this]() {
			ofstream f{T[0]->value()};
			f << text.value();
		});
		B[2]->click([this]() {
			text.set_line(--line);
			text.update();
		});
		B[3]->click([this]() {
			text.set_line(++line);
			text.update();
		});
	}
};

int main() {
	Win win;
	win.loop();
}
