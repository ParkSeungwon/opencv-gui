#include<thread>
#include<cmath>
#include<filesystem>
#include<iostream>
#include"cvmatrix.h"
#include"zgui.h"
using namespace std;

struct Pop : z::AsciiWindow, z::PopupInterface
{
	Pop()
		: z::PopupInterface{this}
		, z::AsciiWindow{R"(
			WReally?--------------
			|
			|  B0-----   B1-----
			|  |Yes|     |No|
			|)", 15, 21}
	{
		B[0]->click([this](){quit(1);});
		B[1]->click([this](){quit(0);});
	}
};

struct Message : z::AsciiWindow, z::PopupInterface
{
	Message()
		: z::PopupInterface{this}
		, z::AsciiWindow{R"(
			W메세지--------------------
			|
			|   L0--------------------
			|   ||
			|   L1--------------------
			|   ||
			|
			|        B0------
			|        |OK|
			|
			|)", 15, 21}
	{
		B[0]->click([this]() {quit(1);});
	}
	void set(string s1, string s2)
	{
		L[0]->text(s1); L[1]->text(s2);
		update(*L[0]); update(*L[1]);
	}
};

struct Mywin : z::AsciiWindow
{
	Pop pop;
	Message msg;
	CVMat m = cv::imread("Lenna.jpg");
	vector<string> v;

	Mywin()
		: z::AsciiWindow{R"(
			WLenna Test----------------------------------------
			| T0----------------------B1- B2------
			| |open file|             ||  |open|
			|
			| C0 L0-----  S0----------------- L1--
			| || |gray|   |0 100 1|           |0|
			| B0-----     S1----------------- L2--
			| |Quit|      |0 200 1|           |0|
			|
			| L3-----   T1--------------B4   B7---  B9------
			| |Rotate|  ||              B5   |save| |restore|
			|
			| B3---------------   B6----------
			| |Detect Contours|   |Fourier|
			|
			| S2-------------------- L4------L5----
			| |1 100 1|              |quant| ||
			|
			| B8-------
			| |tear|
			|)", 15, 21}//no tabs should be used inside ascii art
	{//constructor starts here
		for(const filesystem::path &p : filesystem::directory_iterator("./"))
			if(is_regular_file(p) && p.extension() == ".png" || p.extension() == ".jpg")
				v.push_back(p.filename());
		tie("open file", 30, *T[0], *B[1], v, 700, 150);//make combobox
		tie(*T[1], *B[4], *B[5], 0, 1);//make dial box
		wrap("Edge Detection", 20, 10, *S[0], *S[1], *L[1], *L[2]);
		start(cv::WINDOW_AUTOSIZE);
		cv::moveWindow(title_, 500, 100);

		B[0]->click( [this]() {
			if(pop.open(cv::WINDOW_AUTOSIZE, 600, 200) == 1) cv::destroyAllWindows();
		} );
		C[0]->on_change( [this](bool t) {
			if(t) {
				m.restore();
				m.gray();
				m.show();
			} else {
				m.restore();
				m.show();
			}
		} );
		S[0]->on_change( [this](int val) {
			m.restore();
			m.gray();
			m.edge(val, S[1]->value());
			L[1]->text(to_string(val));
			*this << *L[1];
			m.show();
		} );
		S[1]->on_change( [this](int val) {
			m.restore();
			m.gray();
			m.edge(S[0]->value(), val);
			L[2]->text(to_string(val));
			*this << *L[2];
			m.show();
		} );
		T[1]->enter( [this](std::string val) {
			m.rotate(stod(val));
			m.show();
		} );
		B[2]->click( [this]() {
			m = cv::imread(T[0]->value());
			m.show();
			m.move_window(500, height + 150);
			m.save();
		} );
		B[3]->click( [this]() {
			m.restore();
			m.gray();
			m.detect_contours();
			m.restore();
			m.draw_detected_contours();
			m.show();
		} );
		B[6]->click( [this]() {
			if(m.channels() != 1) {
				msg.set("푸리엔 변환은", "1 채널이어야 합니다.");
				msg.open();
			} else {
				m.dft();
				m.dft_shuffle();
				m.lowpass();
				m.dft_show("Fourier");
				m.dft_shuffle();
				m.idft();
				m.show("idft");
			}
		} );
		B[7]->click( [this]() { m.save(); } );
		B[9]->click( [this]() { m.restore(); } );
		S[2]->on_change( [this](int val) {//정량화
			m.restore();
			if(m.channels() != 1) {
				msg.set("양자변환은", "채널이 1이어야 함.");
				msg.open();
			} else {
				L[5]->text(to_string(val)); *this << *L[5];
				for(int y = 0; y < m.rows; y++) for(int x = 0; x < m.cols; x++)
					m.at<char>(y, x) = (m.at<char>(y, x) / val) * val + val / 2;
				m.show("quant");
			}
		} );
		B[8]->click( [this]() {//tear
			m.restore();
			for(int i=0; i<20; i++) {
				m.tear();
				m.show("tear");
				cv::waitKey(30);
			}
		} );
	}
};

int main()
{
	Mywin win, win2;
	cout << cv::getWindowImageRect(win.title()) << endl;
	return win.loop();
}

