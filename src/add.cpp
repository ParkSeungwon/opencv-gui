#include<filesystem>
#include"zgui.h"
#include"cvmatrix.h"
using namespace std;

struct Win : z::AsciiWindow
{
	Win() : z::AsciiWindow{R"(
		Wadd weighted------------------
		|
		|  T0-------------B0- I0-- I1-
		|  |imagefile1|   ||  ||   || 
		|  
		|  T1-------------B1-I2-
    |  |imagefile2|   || ||
		|
		|  S0-------------- L0-------
    |  |0 100 1|        |weight|
		|  S1-------------- L1-------
		|  |0 128 1|        |salt|
		|  B2----------    B3-----
		|  |Histogram|     |Quit|
		|
		|  C0-L2-----
		|  || |gray|
		|)", 15, 21}
	{
		resize({0, 0, 1054, 1054});
		I[0]->resize({10, 532, 512, 512});
		I[1]->resize({532, 532, 512, 512});
		I[2]->resize({532, 10, 512, 512});
		wrap("", 0, 1, *I[0]);
		wrap("", 0, 1, *I[1]);
		wrap("", 0, 1, *I[2]);
		vector<string> v;
		for(const filesystem::path &p : filesystem::directory_iterator("./"))
			if(is_regular_file(p) && p.extension() == ".png" || p.extension() == ".jpg")
				v.push_back(p.filename());
		tie("File1", 30, *T[0], *B[0], v);
		tie("File2", 30, *T[1], *B[1], v);
		T[0]->value("\u1100\u1161\u11a8\uac00\u3131\u3157"); update(*T[0]);
		start();

		T[0]->enter( [this](string val) { *I[0] = m1 = cv::imread(val); *this << *I[0]; });
		T[1]->enter( [this](string val) { *I[1] = m2 = cv::imread(val); *this << *I[1]; });
		S[0]->on_change( [this](int val) {
				float f = val / 100.f;
				*I[2] = m3 = m1 * f + m2 * (1 - f);
				*this << *I[2];
				L[0]->text(to_string(val));
				*this << *L[0];
		});
		B[2]->click( [this]() { CVMat h = m3; h.histo(); });
		B[3]->click( [this]() { cv::destroyAllWindows(); });
		C[0]->on_change( [this](bool t) { m = m3; if(t) m.gray(); m.show(); m.save(); } );
		S[1]->on_change( [this](int val) {
				m.restore();
				m.salt(val);
				m.show();
				L[1]->text(to_string(val));
				*this << *L[1];
		} );
	}

	CVMat m;
	cv::Mat m1, m2, m3;
};

int main()
{
	Win win;
	win.loop();
}
