#include<iostream>
#include<filesystem>
#include"cvmat/cvmatrix.h"
#include"src/zgui.h"
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
			|   ||  |BLUR|     |v| |GAUSSIAN|    ||  |SHARPEN|
			|
			|   C3- L3-------- C4- L4---------   C5- L5----------
			|   ||  |SOBELX|   ||  |SOBELY|      ||  |DIM1|
			|
			|   C8- L8------ T0------B1          C6- L6--------
			|   ||  |Median| |5|     B2          ||  |diffx|   
			|
			|   C9- L9------ T1------B3          C7- L7---------
			|   ||  |Noise|  |10|    B4          ||  |diffy|    
			|    
			|                  B0----------------
			|                  |적용|
			|)"}
		{
			auto f = tie(*C[0], *C[1], *C[2], *C[3], *C[4], *C[5], *C[6], *C[7], *C[8], *C[9]); 
			auto ksize = tie(*T[0], *B[1], *B[2], 3, 2);
			auto scale = tie(*T[1], *B[3], *B[4], 0, 1);
			organize_accordingto_zindex();
			B[0]->click([this, f, ksize, scale]() {
				int selected = f();
				if(selected < 6) win.m.filter(*p[selected]);
				else if(selected == 8) win.m.median(ksize());
				else if(selected == 9) win.m.noise(scale());
				else {
					if(win.m.channels() != 1) {
						win.info.set("Gray로 먼저 변경하세요");
						win.info.popup(*this);
					} else {
						win.m.convertTo(win.m, CV_32F, 1./ 255.);
						if(selected == 6) win.m.diffx();
						else win.m.diffy();
					}
				}
				win.m.show();
				win.show_info();
			});
		}
		Win &win;
		cv::Mat *p[6] = {&BLUR, &GAUSSIAN, &SHARPEN, &SOBELX, &SOBELY, &DIM1};
	} filter_win{*this};

	struct Fourier : z::AsciiWindow
	{
		Fourier(Win &w) : win{w}, z::AsciiWindow{R"(
			WFourier-----------------------------------------------
			|
			|
			|  B0---------------  T0------B1   B3----------------
			|  |Low Pass Filter|  ||      B2   |High pass Filter|
			|
			|            B4---------------------      
			|            |Inverse Fourier|
			|
			|)"}
		{
			auto cutoff = tie(*T[0], *B[1], *B[2], 30, 1);
			organize_accordingto_zindex();

			B[0]->click([this, cutoff]() {
				win.m.fourier_filter(cutoff());
				*win.I[4] = win.m.show_fourier();
				win.I[4]->update();
			});
			B[3]->click([this, cutoff]() {
				win.m.fourier_filter(cutoff(), false);
				*win.I[4] = win.m.show_fourier();
				win.I[4]->update();
			});
			B[4]->click([this]() {
				win.m.inv_fourier();
				win.m.show();
				win.show_info();
			});

		}
		Win &win;
	} fourier{*this};

	struct Face : z::AsciiWindow
	{
		Face(Win &w) : win{w}, z::AsciiWindow{R"(
			WFace-------------------------------------------
			|
			|
			|   L0-----------  T0------B0- L1-- T1------B2-
			|   |Min Size|     |30|    B1- |X|  |30|    B3-
			|
			|   L2-----------  T2------B4- L3-- T3------B6-
			|   |Max Size|     ||      B5- |X|  ||      B7-
			|
			|        B8--------------   L4---------------
			|        |안면 인식|        ||
			|
			|)"}
		{
			auto f1 = tie(*T[0], *B[0], *B[1], 0, 1);
			auto f2 = tie(*T[1], *B[2], *B[3], 0, 1);
			auto f3 = tie(*T[2], *B[4], *B[5], 0, 1);
			auto f4 = tie(*T[3], *B[6], *B[7], 0, 1);
			organize_accordingto_zindex();
			B[8]->click([this, f1, f2, f3, f4]() {
				win.m.save();
				win.m.gray();
				int detected = win.m.detect_face({f1(), f2()}, {f3(), f4()});
				L[4]->text(to_string(detected) + " faces detected");
				L[4]->update();
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
			|   |v| |EXTERNAL|   ||  |LIST|     ||  |CCOMP|  
			|
			|   C3- L3--------  C4- L4-----------  
			|   ||  |TREE|      ||  |FLOODFILL|
			|
			|
			|
			|   C5- L5---------   C6- L6---------  L<-----------
			|   |v| |NONE|        ||  |SIMPLE|     || 
			|                                       
			|   C7- L7---------   C8- L8---------     B0--------
			|   ||  |TC89_L1|     ||  |TC89_KCOS|     |Detect| 
			|
			|
			|
			|   L9-------- T0----B1   L:---------- T1----B3
			|   |min area| |0|   B2   |max point|  |50|  B4
			|   
			|   L;--------- T2----B5   
			|   |max level| |100| B6   
			|
			|)"}
		{
			auto f1 = tie(*C[0], *C[1], *C[2], *C[3], *C[4]);
			auto f2 = tie(*C[5], *C[6], *C[7], *C[8]);
			auto f3 = tie(*T[0], *B[1], *B[2], 0, 1);
			auto f4 = tie(*T[1], *B[3], *B[4], 0, 1);
			auto f5 = tie(*T[2], *B[5], *B[6], 0, 1);
			wrap("Retrieval Mode", 20, 10, *C[0], *L[2], *L[4]);
			wrap("Approximation Mode", 20, 10, *C[5], *L[8]);
			wrap("그리기 조건", 20, 10, *L[9], *B[4], *B[6]);
			organize_accordingto_zindex();

			B[0]->click([this, f1, f2, f3, f4, f5](){
				win.m.save();
				win.m.gray();
				win.m.edge();
				int detected = win.m.detect_contours(mode[f1()], method[f2()]);
				win.m.restore();
				win.m.draw_detected_contours(f3(), f4(), 1, 8, f5());
				win.m.show();
				win.show_info();
				L[12]->text(to_string(detected) + " detected");
				L[12]->update();
			});
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
			| L0----------- T0-----B1  L1-------------- T1-----B3
			| |Threshold|   |180|  B2  |Continuous|     |50|   B4
			|
			| L2----------- T2-----B5  L3-----------------------
			| |Hop|         |10|   B6  ||
			|
			|
			|                B0--------------
			|                |Detect Line|
			|
			|
			|)"}
		{
			auto f1 = tie(*T[0], *B[1], *B[2], 0, 1);
			auto f2 = tie(*T[1], *B[3], *B[4], 0, 1);
			auto f3 = tie(*T[2], *B[5], *B[6], 0, 1);
			organize_accordingto_zindex();
			B[0]->click([this, f1, f2, f3]() {
				win.m.save();
				win.m.gray();
				win.m.edge();
				int detected = win.m.detect_line(f1(), f2(), f3());
				L[3]->text(to_string(detected) + " lines detected");
				L[3]->update();
				win.m.restore();
				win.m.draw_detected_line();
				win.m.show();
				win.show_info();
			});
		}
		Win &win;
	} line{*this};

	struct Circle : z::AsciiWindow
	{
		Circle(Win &w) : win{w}, z::AsciiWindow{R"(
			WCircle---------------------------------------------
			| 
			|
			| L0--------------------  L5--------
			| |Canny threshold|       ||
			| S0---------------------------------------------
			| |0 1000 1|
			|
			| L1--------------------  L6--------
			| |Center threshold|      ||
			| S1---------------------------------------------
			| |0 1000 1|
			|
			| L3-------------         L7--------
			| |최소 반경|             ||
			| S2---------------------------------------------
			| |0 1000 1|
			|
			| L4-------------         L8--------
			| |최대 반경|             ||
			| S3---------------------------------------------
			| |0 1000 1|
			|
			| B0----------------   L2---------------------
			| |Detect|             ||
			|
			|)"}
		{
			S[0]->value(200);
			S[1]->value(100);
			S[2]->value(0);
			S[3]->value(1000);
			S[0]->draw();
			S[1]->draw();
			S[2]->draw();
			S[3]->draw();
			*this << *S[0];
			*this << *S[1];
			*this << *S[2];
			*this << *S[3];
			for(int i=0; i<4; i++) {
				S[i]->on_change([this, i](int v) { 
					L[i+5]->text(to_string(v));
					L[i+5]->update();
				});
				L[i+5]->text(to_string(S[i]->value()));
			}
			organize_accordingto_zindex();

			B[0]->click([this]() { detect_circle(); });
		}

		Win &win;
		void detect_circle() {
			win.m.restore();
			win.m.gray();
			int detected = win.m.detect_circle(S[0]->value(), S[1]->value(), S[2]->value(), S[3]->value());
			L[2]->text(to_string(detected) + " circles detected");
			L[2]->update();
			win.m.restore();
			win.m.draw_detected_circle();
			win.m.show();
		}
	} circle{*this};

	struct Color : z::AsciiWindow{
		Color(Win &w) : win{w}, z::AsciiWindow{R"(
			WColor----------------------------------------------
			|
			|
			|
			|  B0-------    C1- L0-  C2- L1-  C3- L2-
			|  |색 선택|    |v| |R|  |v| |G|  |v| |B|
			|
			|
			|  B1-------    C5- L3-  C6- L4-  C0- L5-  C4- L6-
			|  |색 선택|    |v| |C|  |v| |M|  |v| |Y|  |v| |K|
			|
			|
			|       Z0-------
			|       ||
			|
			|)"}
		{
			wb.resize(*Z[0]);
			*this + wb;
			organize_accordingto_zindex();
			B[0]->click([this]() { 
				rgb(C[3]->checked(), C[2]->checked(), C[1]->checked()); 
				win.m.show();
			});
			B[1]->click([this]() {
				cmyk(C[5]->checked(), C[6]->checked(), C[0]->checked(), C[4]->checked()); 
				win.m.show();
			});
		}
		Win &win;
		z::WButton wb{{0,0,1,1}};
		void rgb(bool b, bool g, bool r) {
			vector<cv::Mat> v;
			cv::split(win.m, v);
			if(!b) v[0] = 0;
			if(!g) v[1] = 0;
			if(!r) v[2] = 0;
			cv::merge(v, win.m);
			win.m.show();
			win.show_info();
		}
		void cmyk(bool C, bool M, bool Y, bool K) {
			for(auto it = win.m.begin<cv::Vec3b>(); it != win.m.end<cv::Vec3b>(); it++) {
				cv::Vec3b &pix = *it;
				double b = pix[0] / 255., g = pix[1] / 255., r = pix[2] / 255., 
							 k = 1 - max(max(b,g),r), c, m, y;
				if(k == 1) c = m = y = 0;
				else c = (1-r-k)/(1-k), m = (1-g-k)/(1-k), y = (1-b-k)/(1-k);
				if(!C) c = 0; if(!M) m = 0; if(!Y) y = 0; if(!K) k = 0;
				pix[2] = static_cast<unsigned char>(255 * (1-c) * (1-k));
				pix[1] = static_cast<unsigned char>(255 * (1-m) * (1-k));
				pix[0] = static_cast<unsigned char>(255 * (1-y) * (1-k));
			}
			win.m.show();
			win.show_info();
		}
	} color{*this};

	struct Corner : z::AsciiWindow
	{
		Corner(Win &w) : win{w}, z::AsciiWindow{R"(
		  WCorner-----------------------------------------------
			|
			|
			|   L0--T0-------B0    L1------ T1-------B2
			|   |k| |0.04|   B1    |Thresh| |0.01|   B3
			|
			|   L2-----    S0---------------------------- L5----
			|   |block|    |1 31 2|                       |3|
			|
			|   L3-------- S1---------------------------- L6----
			|   |aperture| |1 31 2|                       |3|
			|
			|   B4--------------------   L4--------------------
			|   |Harris Corner|          ||
			| L7-------------------------------------------------
			| |-------------------------------------------------|
			|   L8----------- S2--------------------------- L:---
			|   |threshold|   |1 1000 1|                    |30|
			|   
			|   L9----------- S3--------------------------- L;---
			|   |thresXratio| |1 1000 1|                    |100|
			|
			|   B5--------------------   
			|   |Canny Edge|          
			|)"}
		{
			S[0]->value(3); S[1]->value(3); S[2]->value(30); S[3]->value(100);
			for(int i=0; i<4; i++) { S[i]->draw(); *this << *S[i]; }
			auto k = tie(*T[0], *B[0], *B[1], 0., 0.01);
			auto thresh = tie(*T[1], *B[2], *B[3], 0., 0.01);
			organize_accordingto_zindex();

			S[0]->on_change([this](int v) { L[5]->text(to_string(v)); L[5]->update(); });
			S[1]->on_change([this](int v) { L[6]->text(to_string(v)); L[6]->update(); });
			S[2]->on_change([this](int v) { L[10]->text(to_string(v)); L[10]->update(); });
			S[3]->on_change([this](int v) { L[11]->text(to_string(v)); L[11]->update(); });
			B[4]->click([this, k, thresh]() {
				win.m.save();
				win.m.gray();
				win.m.corner(k(), S[0]->value(), S[1]->value());
				win.m.restore();
				int detected = win.m.draw_detected_corner(thresh());
				win.m.show();
				L[4]->text(to_string(detected) + " detected");
				L[4]->update();
			});
			B[5]->click([this]() {
				win.m.gray();
				win.m.edge(S[2]->value(), S[3]->value());
				win.m.show();
				win.show_info();
			});
		}
		Win &win;
	} corner{*this};

	struct Trans : z::AsciiWindow
	{
		Trans(Win& w) : win{w}, z::AsciiWindow{R"(
		  W변형-----------------------------------------------
			|
			|
			|  L0------ T0----B0  L1------ T1----B2  B4-------- 
			|  |ScaleX| ||    B1  |ScaleY| ||    B3  |변환|
			|
			|  L2------ T2------B5                   B7--------
			|  |회전|   ||      B6                   |변환|
			|
			|  
			|  L3---------------- B8--------- B9----
			|  |For New Win Only| |Crop|      |Save|
			| 
			|  E0---------------------------------------------
			|  ||
			|  |
			|  |
			|  |
			|  |
			|  |
			|)"}
		{
			auto scalex = tie(*T[0], *B[0], *B[1], 1., 0.1);
			auto scaley = tie(*T[1], *B[2], *B[3], 1., 0.1);
			auto angle = tie(*T[2], *B[5], *B[6], 0, 5);
			organize_accordingto_zindex();

			B[4]->click([this, scalex, scaley]() {
				win.m.scale(scalex(), scaley());
				win.m.show();
				win.show_info();
			});
			B[7]->click([this, angle]() {
				win.m.rotate(angle());
				win.m.show();
				win.show_info();
			});
			B[8]->click([this]() {
				win.newwin.restore();
				win.newwin.load_matrix(win.newwin.mat_({win.newwin.ul, win.newwin.br}));
				win.newwin.show();
			});
			B[9]->click([this]() {
					cv::imwrite("cropped.png", win.newwin.mat_);
			});
		}
		Win &win;
	} trans{*this};

	struct NewWin : z::Window
	{
		NewWin() : z::Window{"title", {0,0,1,1}}
		{
			gui_callback_[cv::EVENT_LBUTTONDOWN] = [this](int x, int y) {
				ul = cv::Point2i{x, y};
				pressed = true;
			};
			gui_callback_[cv::EVENT_MOUSEMOVE] = [this](int x, int y) {
				if(pressed) {
					restore();
					cv::rectangle(mat_, {ul, cv::Point2i{x, y}}, {255, 0, 0}, 1);
					show();
				}
			};
			gui_callback_[cv::EVENT_LBUTTONUP] = [this](int x, int y) {
				pressed = false;
				restore();
				br = cv::Point2i{x, y};
				cv::rectangle(mat_, {ul, br}, {255, 0, 0}, 1);
				show();
			};
		}
		void save() {
			mat_.copyTo(m);
		}
		void restore() {
			m.copyTo(mat_);
		}
		cv::Mat m;
		bool pressed = false;
		cv::Point2i ul, br;
	} newwin;
	Win() : z::AsciiWindow{R"(
		WOpenCV Tuning Shop----------------------------------------
		|
		| T0---------------------B2- B0------ B1------ B3------
		| |파일 이름|            ||  |열기|   |저장|   |종료|
		|
		|
		| B4------- L0---------------------- B<--- B;----------
		| |정보|    ||                       |New| |Gray|
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
		| B6------ B7------  B8------ B9------  B:------ B5------
		| |저장|   |로드|    |저장|   |로드|    |저장|   |로드|
		|)"}
	{
		static vector<string> v;
		for(const filesystem::path &p : filesystem::directory_iterator("./"))
			if(is_regular_file(p) && p.extension() == ".png" || p.extension() == ".jpg")
				v.push_back(p.filename());
		resize({0,0,width, 1500});
		tie(*T[0], *B[2], v, 30);//combo box
		wrap("체크포인트1", 20, 10, *I[0], *B[7]);
		wrap("체크포인트2", 20, 10, *I[1], *B[9]);
		wrap("체크포인트3", 20, 10, *I[2], *B[5]);
		wrap("Histogram", 20, 10, *I[3]);
		wrap("Fourier", 20, 10, *I[4]);
		tabs(10, 650, face, contour, line, circle, corner);
		tabs(10, 1100, filter_win, color, trans, fourier);
		scroll_to({0,0,width, height});
		start();
		connect_events();
	}

	void connect_events() {
		B[0]->click([this](){
			try {
				m.imread(T[0]->value()); 
				m.show();
//				cv::setMouseCallback(m.title(), mouse_callback2, this);
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
		B[4]->click([this]() { L[0]->text(m.info()); L[0]->update(); });
		B[6]->click([this]() { *I[0] = m; I[0]->update(); m.copyTo(checkpoint[0]); });
		B[8]->click([this]() { *I[1] = m; I[1]->update(); m.copyTo(checkpoint[1]); });
		B[10]->click([this]() { *I[2] = m; I[2]->update(); m.copyTo(checkpoint[2]); });
		B[7]->click([this](){ checkpoint[0].copyTo(m); show_info(); m.show(); });
		B[9]->click([this](){ checkpoint[1].copyTo(m); show_info(); m.show(); });
		B[5]->click([this](){ checkpoint[2].copyTo(m); show_info(); m.show(); });
		B[11]->click([this]() { m.gray(); show_info(); m.show(); });
		B[12]->click([this]() { 
			newwin.load_matrix(cv::imread(T[0]->value())); 
			newwin.save();
			newwin.open();
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
		m.fourier();
		*I[4] = m.show_fourier();
		I[4]->update();
		m.restore();
	}
};

int main() {
	Win win;
	win.loop();
}

//void mouse_callback2(int event, int x, int y, int flags, void *ptr) {
//	Win *p = (Win*)(ptr);
//	cout << "event window" << endl;
//	stringstream ss;
//	ss << "event " << event << " at " << x << ", " << y << '\n';
//	p->log.text(ss.str());
//	p->log.update();
//}
