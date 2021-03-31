#include<filesystem>
#include"zgui.h"
#include"cvmatrix.h"
using namespace std;

struct Pop : z::AsciiWindow, z::PopupInterface
{
	Pop() : z::PopupInterface{this}, z::AsciiWindow{R"(
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
		B[0]->click([this]() { quit(1); });
		B[1]->click([this]() { quit(0); });
	}

	void set(string s1, string s2) {
		L[0]->text(s1); L[1]->text(s2);
		update(*L[0]); update(*L[1]);
	}
};

struct Win : z::AsciiWindow
{
	Pop pop;
	cv::Mat m;
	int x, y;
	cv::Scalar color;

	Win() : z::AsciiWindow{R"(
		WSample------------------------------------
		| 
		| T0------------------B0- B1------- B2-----
		| |Open File|         ||  |Open|    |Quit|
		|
		| I0-----------------------------------S1-
		| ||                                   |1 100 1|
		| |                                    |
		| |                                    |
		| |                                    |
		| |                                    |
		| |                                    |
		| |                                    |
		| |                                    |
		| |                                    |
		| |                                    |
		| |                                    |
		| |                                    |
		| |                                    |
		| |                                    |
		| |                                    |
		| |                                    |
		| |                                    |
		| |                                    |
		| |                                    |
		| |                                    |
		| S0----------------------------------
		| |1 100 1|
		|
		|
		|  C0 L0-  C1 L1-  C2 L2-   C3- L3----
		|  |v||R|  || |G|  || |B|   ||  |New|
		|
		|)"}
	{
		vector<string> v;
		for(const filesystem::path &p : filesystem::directory_iterator("./"))
			if(is_regular_file(p) && p.extension() == ".png" || p.extension() == ".jpg")
				v.push_back(p.filename());
		tie("File open", 30, *T[0], *B[0], v);//combo box
		tie(*C[0], *C[1], *C[2]);//radio button
		wrap("RGB", 20, 10, *C[0], *L[2]);
		start();

		B[1]->click([this]() {
				*I[0] = cv::imread(T[0]->value());
				*this << *I[0];
				m = I[0]->mat_.clone();
		});
		B[2]->click([this]() { pop.set("Really?", ""); if(pop.open()) cv::destroyAllWindows(); });
		S[0]->on_change([this](int val) { draw_circle(); });
		S[1]->on_change([this](int val) { draw_circle(); });
		C[3]->on_change([this](bool t) {
				if(t) cv::imshow("new", m);
				else cv::destroyWindow("new");
		});
	}

	void draw_circle()
	{
		if(C[0]->checked()) color = {0,0,255};
		else if(C[1]->checked()) color = {0,255,0};
		else color = {255,0,0};
		I[0]->mat_ = m.clone();
		int x = m.cols * S[0]->value() / 100;
		int y = m.rows * (100 - S[1]->value()) / 100;
		cv::circle(I[0]->mat_, {x, y}, 30, color, 2);
		*this << *I[0];
	}
};


int main()
{
	Win win;
	win.loop();
}
