#include<opencv2/opencv.hpp>
#include"zgui.h"
using namespace std;
/*
  asc2 art example below, never use tab.
	Wtitle-------------------------------------------
	|     L0-------
	|			|Test Label|
	|     S0----------------- L1--
	|     |0 100 1|           ||
	|
	|     B0----    B1------   B2-----
	|     |Popup|   |Cancel|   |Click|
	|     C0 L2--
	|     T0--------
	|     ||
	|
	|     I0---------------------
	|     ||
	|     |
	|     |
	|     |
	|
*/

z::AsciiWindow::AsciiWindow(const char *p, int unit_width, int unit_height, int margin)
	: z::Window{"", {0, 0, 1, 1}}
{/// @param p compose GUI with ascii art, never use tab.
 /// @param unit_width width in pixel that one ascii art character will occupy. 
 /// 				If you want to make a bigger widget increase this.
 /// @param unit_height height in pixel that one ascii art character will occupy
 /// @param margin margin space around window 
	spdlog::debug("{} parsing started {}", z::source_loc(), p);
	uw_ = unit_width; uh_ = unit_height; margin_ = margin;
	width = 1;
	while(*p != 'W') p++;
	p++;
	while(*p != '-') title_ += *p++;
	while(*p == '-') p++, width++;
	width += title_.size();

	for(; *p; p++) if(*p == '|') {//construct 2 dim array of asc2 art
		string s;
		while(*p != '\n' && *p) s += *p++;
		if(s.size() < width) s.resize(width, ' ');
		art_.push_back(s);
	}

	for(auto &a : art_) { // CJK character correction
		string s;
		bool prev_cjk_ext = false;
		int count = 0, to_del = 0, to_attach = 0;
		for(const auto &b : a) {
			if(to_del && b == ' ') {
				to_attach++;
				to_del--;
			} else s += b;
			if((b & 0xc0) == 0x80) {
				if(!prev_cjk_ext) count++;
				prev_cjk_ext = true;
			} else {
				if(b == '|') {
					if(count != 0) to_del = count;
					count = 0;
				}
				prev_cjk_ext = false;
			}
		}
		for(int i=0; i<to_attach; i++) s += ' ';
		a = s;
		cout << a << endl;
	}

	parsed_ = art_;//for parse check
	for(auto &a : parsed_) for(auto &b : a) b = ' ';
	height = art_.size() * unit_height;
	width *= unit_width;
	resize({x, y, width, height});
	scrolled_rect_ = *this;

	parse_art();
	spdlog::debug("{} parsing end", z::source_loc());
	//add to window
	for(auto &a : B) *this + *a.get();
	for(auto &a : L) *this + *a.get();
	for(auto &a : S) *this + *a.get();
	for(auto &a : C) *this + *a.get();
	for(auto &a : T) *this + *a.get();
	for(auto &a : I) *this + *a.get();
	for(auto &a : P) *this + *a.get();
	for(auto &a : E) *this + *a.get();
	for(auto &a : Z) { *this + *a.get(); a->zIndex(-1); }
}

void z::AsciiWindow::parse_art()
{///parse asc2 art and populate member CBLSTI
	bool retry = false;
	for(int y=0; y<art_.size(); y++) for(int x=0; x<art_[y].size(); x++) 
		if(isalpha(art_[y][x]) && parsed_[y][x] != 'v')
			if(!parse_widget_area(y, x)) retry = true;//if L2 -> 2 is not in order
	if(retry) parse_art();
}

int z::AsciiWindow::get_size(char c)
{///return vector size of designated widget type
	switch(c) {
		case 'B': return B.size();
		case 'L': return L.size();
		case 'S': return S.size();
		case 'C': return C.size();
		case 'T': return T.size();
		case 'I': return I.size();
		case 'P': return P.size();
		case 'Z': return Z.size();
		case 'E': return E.size();
		default: throw 0;
	}
}

array<int, 3> get_slider_param(string s)
{///start stop step -> int
	array<int, 3> r;
	stringstream ss;
	ss << s;
	ss >> r[0] >> r[1] >> r[2];
	return r;
}

bool z::AsciiWindow::parse_widget_area(int y, int x)
{///parse a rect of asc2 art widget inside window
	string text;//get text between | |
	if(art_[y+1][x] == '|') for(int x2 = x; art_[y+1][++x2] != '|';) {
		text += art_[y+1][x2];
		parsed_[y+1][x2] = 'v';//disable text
	}
	if(art_[y][x+1] - '0' != get_size(art_[y][x])) return false;//if order is not right

	parsed_[y][x] = 'v';//been here
	int h = 1, w = 2;
	for(int y2 = y; art_[++y2][x] == '|';) h++;//calculate widget size
	for(int x2 = x+1; art_[y][++x2] == '-';) w++;
	cv::Rect2i r{x*uw_+margin_, y*uh_+margin_, w*uw_ - 2*margin_, h*uh_ - 2*margin_};
	switch(art_[y][x]) {
		case 'B': B.emplace_back(make_shared<z::Button>(text, r)); break;
		case 'L': L.emplace_back(make_shared<z::Label>(text, r)); break;
		case 'S': { auto [a, b, c] = get_slider_param(text);
								S.emplace_back(make_shared<z::Slider>(r, a, b, c)); }
							break;
		case 'C': C.emplace_back(make_shared<z::CheckBox>(r)); if(text != "") C.back()->checked(true); break;
		case 'T': T.emplace_back(make_shared<z::TextInput>(r)); T.back()->value(text); break;
		case 'I': I.emplace_back(make_shared<z::Image>(r));
							if(text != "") *I.back() = cv::imread(text);
							break;
		case 'P': P.emplace_back(make_shared<z::Progress>(r)); break;
		case 'Z': Z.emplace_back(make_shared<z::Widget>(r)); break;
		case 'E': E.emplace_back(make_shared<z::TextBox>(r, h/2)); break;
	}
	return true;
}
