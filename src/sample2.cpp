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
	struct Filter : z::AsciiWindow
	{
		Filter(Win &w) : win{w}, z::AsciiWindow{R"(
			WFilter----------------------------------------------
			|
			| C0- L0-------- C1- L1--------   C2- L2----------
			| ||  |BLUR|     ||  |GAUSSIAN|   ||  |SHARPEN|
			|
			| C3- L3-------- C4- L4--------   C5- L5----------
			| ||  |SOBELX|   ||  |SOBELY|     ||  |DIM1|
			|
			|                B0----------------
			|                |적용|
			|)"}
		{
			tie(*C[0], *C[1], *C[2], *C[3], *C[4], *C[5]); 
			zIndex(1);
			x = 10; y = 700;
			static cv::Mat *p[6] = {&BLUR, &GAUSSIAN, &SHARPEN, &SOBELX, &SOBELY, &DIM1};
			B[0]->click([this, p]() {
				for(int i=0; i<6; i++) if(C[i]->checked()) win.m.filter(*p[i]);
				win.m.show();
				win.show_info();
			});
		}
		Win &win;
	} filter_win{*this};

	Win() : z::AsciiWindow{R"(
		WOpenCV Tuning Shop----------------------------------------
		|
		| T0---------------------B2- B0------ B1------ B3------
		| |파일 이름|            ||  |열기|   |저장|   |종료|
		|
		|
		| B4------- L0-----------------      
		| |정보|    ||                       
		|
		|                                    
		| I3-----------------------    I4------------------------
		| ||                           ||            
		| |                            |           
		| |                            |           
		| |                            |           
		| |                            |           
		| |                            |           
		| |                            |           
		| |                            |           
		| |                            |           
		| |                            |           
		| |                            |           
		| |                            |           
		| |                            |           
		| |                            |           
		| |                            |           
		|                                   
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
		| |안면인식|     |Filter|
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
		wrap("Histogram", 20, 10, *I[3]);
		wrap("Fourier", 20, 10, *I[4]);
		*this + filter_win;
		start();
		B[0]->click([this](){
			try {
				m.imread(T[0]->value()); 
				m.show();
				show_info();
				show();
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
			show_info();
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
			show_info();
			m.show();
		});
		B[9]->click([this](){
			checkpoint[1].copyTo(m);
			show_info();
			m.show();
		});
		B[11]->click([this](){
			checkpoint[2].copyTo(m);
			show_info();
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
			m.filter(BLUR);
			show_info();
			m.show();
		});
	}
	YesNo yesno;
	Info info;
	CVMat m;
	cv::Mat checkpoint[3];

	void show_info() {
		*I[3] = m.histo();
		I[3]->update();
		L[0]->text(m.info());
		L[0]->update();
		m.save();
		m.gray();
		cv::Mat tmp;
		m.fourier().convertTo(tmp, CV_8UC3, 255, 0);
		*I[4] = tmp;
		I[4]->update();
		m.restore();
	}
};

int main() {
	Win win;
	win.loop();
}
