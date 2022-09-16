#include<iostream>
#include<filesystem>
#include"cvmat/cvmatrix.h"
#include"zgui.h"
using namespace std;


struct YesNo : z::AsciiWindow
{
	YesNo() : z::AsciiWindow{R"(
		W-----------------------------------
		|
		|
		|    L0--------------------------
		|    ||
		|    L1--------------------------
		|    ||
		|
		|    B0----------    B1----------
		|    |예|            |아니오|
		|)"}
	{
		B[0]->click([this]() { popdown(1); });
		B[1]->click([this]() { popdown(0); });
		organize_accordingto_zindex();
	}

	void set(string s1, string s2) {
		L[0]->text(s1); L[1]->text(s2);
		*this << *L[0]; *this << *L[1];
	}
};

struct Info : z::AsciiWindow
{
	Info() : z::AsciiWindow{R"(
		W-----------------------------------
		|
		|  L0-----------------------------
		|  ||
		|
		|            B0-----------
		|            |확인|
		|
		|)"}
	{
		B[0]->click([this] () {popdown(1); });
		organize_accordingto_zindex();
	}
	void set(string s1) {
		L[0]->text(s1); *this << *L[0]; 
	}
};

struct Win : z::AsciiWindow
{
	Win() : z::AsciiWindow{R"(
		WOpenCV Tuning Shop----------------------------------------
		|
		| T0---------------------B2- B0------ B1------ B3------
		| |파일 이름|            ||  |열기|   |저장|   |종료|
		|
		| B4------- L0-----------------
		| |정보|    ||
		|
		|
		| I0---------------- I1---------------- I2---------------
		| ||                 ||                 ||   
		| |                  |                  |
		| |                  |                  |
		| |                  |                  |
		| |                  |                  |
		| |                  |                  |
		| |                  |                  |
		| |                  |                  |
		| |                  |                  |
		| |                  |                  |
		| B6------ B7------  B8------ B9------  B:------ B;------
		| |저장|   |로드|    |저장|   |로드|    |저장|   |로드|
		|
		|
		| B5------
		| |Gray|
		| B<--------     B=---------
		| |안면인식|     |histogram|
		|
		|
		|
		|
		|
		|
		|
		|
		|
		|
		|
		|
		|)"}
	{
		static vector<string> v;
		for(const filesystem::path &p : filesystem::directory_iterator("./"))
			if(is_regular_file(p) && p.extension() == ".png" || p.extension() == ".jpg")
				v.push_back(p.filename());
		tie2("", 30, *T[0], *B[2], v);//combo box
		wrap("체크포인트1", 20, 10, *I[0], *B[7]);
		wrap("체크포인트2", 20, 10, *I[1], *B[9]);
		wrap("체크포인트3", 20, 10, *I[2], *B[11]);

		start();
		B[0]->click([this](){
			try {
				m.imread(T[0]->value()); m.show();
			} catch(...){}
		});
		B[1]->click([this](){
			filesystem::path p = T[0]->value();
			if(filesystem::exists(p)) {
				yesno.set("파일이 이미 존재합니다.", "덮어쓰시겠습니까?");
				yesno.popup(*this, [this, p](int i) {if(i) m.imwrite(p);});
			} else{
				try {
					m.imwrite(T[0]->value());
				} catch(...) {}
			}
		});
		B[3]->click([this]() {
			yesno.set("종료하시겠습니까?", "");
			yesno.popup(*this, [this] (int i) {if(i) cv::destroyAllWindows();});
		});
		B[4]->click([this]() {
			L[0]->text(m.info());
			L[0]->update();
		});
		B[5]->click([this]() {
			m.gray();
			m.show();
		});
		B[6]->click([this]() {
			*I[0] = m;
			I[0]->update();
			m.copyTo(checkpoint[0]);
		});
		B[8]->click([this]() {
			*I[1] = m;
			I[1]->update();
			m.copyTo(checkpoint[1]);
		});
		B[10]->click([this]() {
			*I[2] = m;
			I[2]->update();
			m.copyTo(checkpoint[2]);
		});
		B[7]->click([this](){
			checkpoint[0].copyTo(m);
			m.show();
		});
		B[9]->click([this](){
			checkpoint[1].copyTo(m);
			m.show();
		});
		B[11]->click([this](){
			checkpoint[2].copyTo(m);
			m.show();
		});
		B[12]->click([this]() {
			m.save();
			m.gray();
			m.detect_face();
			m.restore();
			m.draw_detected_face();
			m.show();
		});
		B[13]->click([this]() {
			m.histo();
		});
	}
	YesNo yesno;
	Info info;
	CVMat m;
	cv::Mat checkpoint[3];
};

int main() {
	Win win;
	win.loop();
}
