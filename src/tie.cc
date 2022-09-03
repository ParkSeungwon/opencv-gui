#include"zgui.h"
using namespace std;

struct Tie : z::Popup {
	Tie(string title, int font, vector<string> v) : z::Popup{title, {0, 0, 300, font * v.size()}} {
		this->v = v;
		int length = 0;
		for(int i=0, base=0; i<v.size(); i++) {
			int k = ft2_->getTextSize(v[i], font, -1, &base).width;
			if(length < k) length = k;
		}
		cv::resize(mat_, mat_, {length, font * v.size()});
		for(int i=0; i<v.size(); i++) {
			bts.emplace_back(make_shared<z::Button>(v[i], cv::Rect2i{0, i*font, length, 30})); 
			bts.back()->click([i, this](){quit(i);});
			*this + *bts.back();
			update(*bts.back());
		}
	}
	vector<shared_ptr<z::Button>> bts;
	vector<string> v;
	string open(int x, int y) {
		return v[z::Popup::open(cv::WINDOW_AUTOSIZE, x, y)];
	}
};

struct Tie2 : z::Window {
	Tie2(string title, int font, vector<string> v) : z::Window{title, {0,0,1,1}} {
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

void z::Window::tie2(string title, int font, z::TextInput &t, z::Button &b, vector<string> v)
{//combobox
  static vector<string> v2 = v;
	static Tie2 vw{title, font, v2};
	b.text("\u25bc");
	update(b);
	vw.resize({t.x, t.y+font, vw.width, vw.height});
	b.click([&](){vw.popup(*this, [this, &t, v2](int i){t.value(v2[i]); *this << t; });});
}


void z::Window::tie(string title, int font, z::TextInput &t, z::Button &b, vector<string> v, int x, int y)
{//combobox
	static vector<shared_ptr<Tie>> vw;
	vw.emplace_back(make_shared<Tie>(title, font, v));
	b.text("\u25bc");
	update(b);
	int k = vw.size()-1;
	b.click([&, k, x, y](){t.value(vw[k]->open(x, y)); *this << t;});
}

void z::Window::tie(z::TextInput &t, z::Button &b1, z::Button &b2, double start, double step)
{//number input
	b1.text("\u25b5"); b2.text("\u25bf");
	t.value(to_string(start));
	update(b1); update(b2); update(t);
	b1.click([&, step](){t.value(to_string(stod(t.value()) + step)); *this << t;});
	b2.click([&, step](){t.value(to_string(stod(t.value()) - step)); *this << t;});
}

