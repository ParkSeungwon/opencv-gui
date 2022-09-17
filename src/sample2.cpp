#include<iostream>
#include<filesystem>
#include<spdlog/spdlog.h>
#include<source_location>
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
			WFilter--------------------------------------------------
			|
			|
			|   C0- L0-------- C1- L1---------   C2- L2----------
			|   ||  |BLUR|     ||  |GAUSSIAN|    ||  |SHARPEN|
			|
			|   C3- L3-------- C4- L4---------   C5- L5----------
			|   ||  |SOBELX|   ||  |SOBELY|      ||  |DIM1|
			|
			|                  B0----------------
			|                  |적용|
			|)"}
		{
			tie(*C[0], *C[1], *C[2], *C[3], *C[4], *C[5]); 
			zIndex(1);
			x = 10; y = 700;
			organize_accordingto_zindex();
			B[0]->click([this]() {
				for(int i=0; i<6; i++) if(C[i]->checked()) win.m.filter(*p[i]);
				win.m.show();
				win.show_info();
			});
		}
		Win &win;
		cv::Mat *p[6] = {&BLUR, &GAUSSIAN, &SHARPEN, &SOBELX, &SOBELY, &DIM1};
	} filter_win{*this};

	struct Face : z::AsciiWindow
	{
		Face(Win &w) : win{w}, z::AsciiWindow{R"(
			W안면 인식-----------------------------------------
			|
			|
			|   L0-----------  T0------B0- L1-- T1------B2-
			|   |Min Size|     |30|    B1- |X|  |30|    B3-
			|
			|   L2-----------  T2------B4- L3-- T3------B6-
			|   |Max Size|     ||      B5- |X|  ||      B7-
			|
			|          B8--------------
			|          |안면 인식|
			|
			|)"}
		{
			x = 10; y = 700; zIndex(2);
			auto f1 = tie(*T[0], *B[0], *B[1], 0, 1);
			auto f2 = tie(*T[1], *B[2], *B[3], 0, 1);
			auto f3 = tie(*T[2], *B[4], *B[5], 0, 1);
			auto f4 = tie(*T[3], *B[6], *B[7], 0, 1);
			spdlog::info("Welcome");
			spdlog::error("error {}", 1);
			organize_accordingto_zindex();
			B[8]->click([this, f1, f2, f3, f4]() {
				win.m.save();
				win.m.gray();
				win.m.detect_face({f1(), f2()}, {f3(), f4()});
				win.m.restore();
				win.m.draw_detected_face();
				win.m.show();
			});
		}
		Win &win;
	} face{*this};

	struct Contour : z::AsciiWindow
	{
		Contour(Win& w) : win{w}, z::AsciiWindow{R"(
			WContour---------------------------------------------
			|
			|
			|
			|   C0- L0---------  C1- L1-------  C2- L2-------
			|   ||  |EXTERNAL|   ||  |LIST|     ||  |CCOMP|  
			|
			|   C3- L3--------  C4- L4-----------  
			|   ||  |TREE|      ||  |FLOODFILL|
			|
			|
			|   C5- L5---------   C6- L6---------  
			|   ||  |NONE|        ||  |SIMPLE|       
			|                                         B0------
			|   C7- L7---------   C8- L8---------     |Detect|
			|   ||  |TC89_L1|     ||  |TC89_KCOS|
			|
			|
			|   L9-------- T0----B1   L:---------- T1----B3
			|   |min area| |0|   B2   |max point|  |10|  B4
			|   
			|   L;--------- T2----B5
			|   |max level| |100| B6
			|)"}
		{
			x = 10; y = 700;
			auto f1 = tie(*C[0], *C[1], *C[2], *C[3], *C[4]);
			auto f2 = tie(*C[5], *C[6], *C[7], *C[8]);
			auto f3 = tie(*T[0], *B[1], *B[2], 0, 1);
			auto f4 = tie(*T[1], *B[3], *B[4], 0, 1);
			auto f5 = tie(*T[2], *B[5], *B[6], 0, 1);
			wrap("Retrieval Mode", 20, 10, *C[0], *L[2], *L[4]);
			wrap("Approximation Mode", 20, 10, *C[5], *L[8]);
			wrap("조건", 20, 10, *L[9], *B[4], *B[6]);
			B[0]->click([this, f1, f2, f3, f4, f5](){
				win.m.save();
				win.m.gray();
				win.m.edge();
				win.m.detect_contours(mode[f1()], method[f2()]);
				win.m.restore();
				win.m.draw_detected_contours(f3(), f4(), 1, 8, f5());
				win.m.show();
				win.show_info();
			});
			organize_accordingto_zindex();
		}
		Win &win;
		int mode[5] = {cv::RETR_EXTERNAL, cv::RETR_LIST, cv::RETR_CCOMP, cv::RETR_TREE, cv::RETR_FLOODFILL};
		int method[4] = {cv::CHAIN_APPROX_NONE, cv::CHAIN_APPROX_SIMPLE, 
			cv::CHAIN_APPROX_TC89_L1, cv::CHAIN_APPROX_TC89_KCOS};
	} contour{*this};

	struct Line : z::AsciiWindow
	{
		Line(Win &w) : win{w}, z::AsciiWindow{R"(
			WLine------------------------------------------------
			|
			|
			|  L0----------- T0----B1  L1-------------- T1----B3
			|  |Threshold|   |180| B2  |Continuous|     |50|  B4
			|
			|  L2----------- T2----B5
			|  |Hop|         |10|  B6
			|
			|
			|                B0--------------
			|                |Detect Line|
			|
			|
			|)"}
		{
			x = 10, y = 700;
			auto f1 = tie(*T[0], *B[1], *B[2], 0, 1);
			auto f2 = tie(*T[1], *B[3], *B[4], 0, 1);
			auto f3 = tie(*T[2], *B[5], *B[6], 0, 1);
			B[0]->click([this, f1, f2, f3]() {
				win.m.save();
				win.m.gray();
				win.m.edge();
				win.m.detect_line(f1(), f2(), f3());
				win.m.restore();
				win.m.draw_detected_line();
				win.m.show();
				win.show_info();
			});
		}
		Win &win;
	} line{*this};

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
		| B5---   B<--------
		| |필터|  |안면인식|
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
		*this + face + filter_win + contour + line;
		*this << face << filter_win << contour << line;
		tabs(filter_win, face, contour, line);
		start();
		connect_events();
	}

	void connect_events() {
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
		B[5]->click([this]() {
			filter_win.zIndex(100);
			face.zIndex(2);
			organize_accordingto_zindex();
			show();
		});
		B[12]->click([this]() {
			face.zIndex(100);
			filter_win.zIndex(2);
			organize_accordingto_zindex();
			show();
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
