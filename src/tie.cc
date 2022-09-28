#include"zgui.h"
using namespace std;

struct Tie : z::Window {
	Tie(int font, vector<string> v) : z::Window{"", {0,0,1,1}} {
		int length = 0;
		for(int i=0, base=0; i<v.size(); i++) {
			int k = ft2_->getTextSize(v[i], font, -1, &base).width;
			if(length < k) length = k;
		}
		resize({0, 0, length, font * v.size()});
		for(int i=0; i<v.size(); i++) {
			std::shared_ptr<z::Button> p = make_shared<z::Button>(v[i], cv::Rect2i{0, i*font, length, font}); 
			p->click([i, this](){popdown(i);});
			*this + *p;
			bts.push_back(p);
		}
	}
	vector<shared_ptr<z::Button>> bts;
};

void z::Window::tie(z::TextInput &t, z::Button &b, const vector<string> &v, int font)
{/// create a combobox by tying textinput and button b.
 /// a popup window will appear with buttons of all the string in vector v.
	static Tie vw{font, v};
	b.text("\u25bc");
	*this << b;
	vw.x = t.x; vw.y = t.y + font;
	//vw.resize({t.x, t.y+font, vw.width, vw.height});
	b.click([&](){vw.popup(*this, [this, &t, &v](int i){t.value(v[i]); t.update();});});
}

//void z::Window::tie(z::TextInput &t, z::Button &b1, z::Button &b2, double start, double step)
//{//number input
//	b1.text("\u25b2"); b2.text("\u25bc");
//	if(t.value() == "") t.value(to_string(start));
//	*this << b1; *this << b2; *this << t;
//	b1.click([&, step](){t.value(to_string(stod(t.value()) + step)); *this << t; show();});
//	b2.click([&, step](){t.value(to_string(stod(t.value()) - step)); *this << t; show();});
//}

