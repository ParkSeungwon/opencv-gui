#include<opencv2/opencv.hpp>
#include"zgui.h"
#define BACKSPACE 8//65288 //8
#define HANGUL 234//65514 //234
#define HANJA 228//65508 //228
#define LEFT 150//65361
#define RIGHT 152//65263
#define UP 151//65362
#define DOWN 153//65264
#define DEL 255
#define HOME 149
#define END 156
#define PGUP 154
#define PGDN 155
#define CAPS 229
#define ENTER 13
using namespace std;
using namespace placeholders;

string GetUnicodeChar(unsigned int code) {
		char chars[5];
    if (code <= 0x7F) {
        chars[0] = (code & 0x7F); chars[1] = '\0';
    } else if (code <= 0x7FF) {
        // one continuation byte
        chars[1] = 0x80 | (code & 0x3F); code = (code >> 6);
        chars[0] = 0xC0 | (code & 0x1F); chars[2] = '\0';
    } else if (code <= 0xFFFF) {
        // two continuation bytes
        chars[2] = 0x80 | (code & 0x3F); code = (code >> 6);
        chars[1] = 0x80 | (code & 0x3F); code = (code >> 6);
        chars[0] = 0xE0 | (code & 0xF); chars[3] = '\0';
    } else if (code <= 0x10FFFF) {
        // three continuation bytes
        chars[3] = 0x80 | (code & 0x3F); code = (code >> 6);
        chars[2] = 0x80 | (code & 0x3F); code = (code >> 6);
        chars[1] = 0x80 | (code & 0x3F); code = (code >> 6);
        chars[0] = 0xF0 | (code & 0x7); chars[4] = '\0';
    } else {
        // unicode replacement character
        chars[2] = 0xEF; chars[1] = 0xBF; chars[0] = 0xBD;
        chars[3] = '\0';
    }
		return chars;
}

bool is_character_head(char c) {
	return (c & 0xc0) != 0x80;
}
std::pair<std::string, std::string> divide_utf_string(std::string s, int pos) {
	int i = 0; 
	std::string fore, aft, tmp;
	for(char c : s) {
		if(is_character_head(c)) i++;
		if(i <= pos) fore +=c;
		else aft += c;
	}
	return {fore, aft};
}
int count_utf_string(std::string s)
{
	int k = 0;
	for(char c : s) if(is_character_head(c)) k++;
	return k;
}
std::string pop_back_utf(std::string &s)
{
	int l = count_utf_string(s);
	if(l == 0) return "";
	auto [f, b] = divide_utf_string(s, l - 1);
	s = f;
	return b;
}
std::string pop_front_utf(std::string &s)
{
	if(s.size() == 0) return "";
	auto [f, b] = divide_utf_string(s, 1);
	s = b;
	return f;
}

HangulInputContext* z::TextInput::hic_ = hangul_ic_new("2");
HanjaTable* z::TextInput::table_ = hanja_table_load(NULL);
bool z::TextInput::hangul_mode_ = false;

z::TextInput::TextInput(cv::Rect2i r) : z::Widget{r}
{
	draw();
	gui_callback_[EVENT_KEYBOARD] = bind(&z::TextInput::key_event, this, _1, _2);
	gui_callback_[EVENT_ENTER] = [this](int, int) { show_cursor(); };
	gui_callback_[EVENT_LEAVE] = [this](int, int) { flush(); draw(); };
	gui_callback_[cv::EVENT_LBUTTONUP] = [this](int xpos, int) {
		std::string s = value();
		int baseline = 0, i = 0, k = count_utf_string(s);
		while(10 + ft2_->getTextSize(divide_utf_string(s, i).first, height * 0.8, -1, &baseline).width < xpos - x 
			&& i <= k) i++;
		std::tie(fore_, back_) = divide_utf_string(s, i-1);
		editting_ = pop_front_utf(back_);
		draw();
		show_cursor();
	};
}

void z::TextInput::flush() {
	if(!hangul_ic_is_empty(hic_)) {
		hangul_ic_flush(hic_);
		fore_ += editting_;
		editting_ = pop_front_utf(back_);
		draw();
	}
}

void z::TextInput::hanja()
{
	//editting_ = GetUnicodeChar(*hangul_ic_get_preedit_string(hic_));
	cout << "edittin : " << editting_ << endl;
	HanjaList* list = hanja_table_match_exact(table_, editting_.data());
	vector<string> v;
	if(list != nullptr) for(int i=0; i<hanja_list_get_size(list); i++) {
		const char* hanja = hanja_list_get_nth_value(list, i);
		v.push_back(hanja);
	}
	hanja_list_delete(list);
	if(!v.empty()) {
		hangul_ic_reset(hic_);
		popup(v);
	}
}

void z::TextInput::hangul()
{
	hangul_mode_ = !hangul_mode_;
	flush();
	show_cursor();
}

void z::TextInput::key_event(int key, int)
{
	switch(key) {
		case HANGUL: hangul(); return;
		case LEFT: move_cursor(false); return;
		case RIGHT: move_cursor(true); return;
		case DEL: del(); return;
		case HANJA: hanja(); return;
	}

	if(hangul_mode_) {
		if(key == BACKSPACE && hangul_ic_is_empty(hic_)) backspace(); //backspace
		else { // make character
			if(hangul_ic_is_empty(hic_)) back_ = editting_ + back_;
			hangul_ic_process(hic_, key);
			const ucschar *commit = hangul_ic_get_commit_string(hic_);
			const ucschar *preedit = hangul_ic_get_preedit_string(hic_);
			if(*commit != 0) fore_ += GetUnicodeChar(*commit);
			editting_ = GetUnicodeChar(*preedit);
			if(!isalpha(key) && isprint(key)) {
				fore_ += key; // for space 1 2 3 etc
				back_ = editting_ + back_;
			}
		}
	} else {
		if(isprint(key)) fore_ += key;
		else if(key == BACKSPACE) backspace();
		else return;
	}
	draw();
	show_cursor();
}
	
void z::TextInput::show_cursor() {
	int baseline = 0;
	auto sz1 = ft2_->getTextSize(fore_, height * 0.8, -1, &baseline);
	auto sz2 = ft2_->getTextSize(fore_ + editting_, height * 0.8, -1, &baseline); 
	cv::rectangle(mat_, cv::Rect2i{{sz1.width + 10, 0}, 
			cv::Point2i{std::max(10 + sz2.width, 20 + sz1.width), height}}, {0,0,0}, 1);
}
void z::TextInput::move_cursor(bool right) 
{
	hangul_ic_flush(hic_);
	if(right) {
		fore_ += editting_;
		editting_ = pop_front_utf(back_);
	} else {
		if(fore_ != "") {
			back_ = editting_ + back_;
			editting_ = pop_back_utf(fore_);
		}
	}
	draw();
	show_cursor();
}

struct HanjaWin : z::Window
{
	HanjaWin(int button_size) : z::Window { "", {0,0,1,1} }
	{ button_sz = button_size; }
	vector<shared_ptr<z::Button>> bts;
	int button_sz = 10;
	void set_hanja(vector<string> v) {
		bts.clear();
		widgets_.clear();
		resize({ 0, 0, std::min(10 * button_sz, (int)v.size() * button_sz)
			, ((v.size() -1) / 10 + 1) * button_sz });
		mat_ = background_color_;
		for(int i=0; i<v.size(); i++) {
			std::shared_ptr<z::Button> p = make_shared<z::Button>(v[i], 
					cv::Rect2i{(i % 10) * button_sz, i / 10 * button_sz, button_sz, button_sz}); 
			p->click([i, this](){popdown(i);});
			*this + *p;
			bts.push_back(p);
		}
		organize_accordingto_zindex();
	}
};

void z::TextInput::popup(vector<string> v)
{
	static HanjaWin hanj{50};
	hanj.set_hanja(v);
	hanj.x = x + 50;
	hanj.y = y + 50;
	hanj.popup(*parent_, [this, v](int i) { 
		fore_ += v[i]; 
		editting_ = pop_front_utf(back_);
		draw();
		show_cursor();
		update();
	});
}

void z::TextInput::enter(function<void(string)> f)
{/// register a function that will be called when user stroke ENTER key.
	user_callback_[EVENT_KEYBOARD] = [this, f](int val, int){ if(val == 13) f(value());};
}

string z::TextInput::value() const
{
	return fore_ + editting_ + back_;
}

void z::TextInput::backspace()
{
	while(!fore_.empty()) {
		char c = fore_.back();
		fore_.pop_back();
		if((c & 0xc0) != 0x80) break;
	}
	draw();
	show_cursor();
}

void z::TextInput::del() {
	editting_ = pop_front_utf(back_);
	draw();
	show_cursor();
}

void z::TextInput::value(string s)
{
	fore_ = s;
	editting_ = "";
	back_ = "";
	draw();
}

void z::TextInput::draw()
{
	mat_ = white;
	//else shade_rect({0, 0, width, height}, 4, highlight_color_, click_color_, highlight_color_);
	int baseline = 0;
	std::pair<std::string, std::string> div;
	std::string s = fore_ + editting_ + back_;
	for(int i=0; i<count_utf_string(s) + 1; i++) {
		div = divide_utf_string(s, i);
		if(ft2_->getTextSize(div.first, height * 0.8, -1, &baseline).width > width - 20) break;
	}
	ft2_->putText(mat_, div.first, {10, 0}, height * 0.8, {0, 0, 0}, -1, 4, false);
	int sz = div.first.size();
	std::string a, b, c;
	if(sz > fore_.size()) {
		a = ""; b = "";
		c = back_.substr(sz - fore_.size() - editting_.size());
	} else a = fore_.substr(sz), b = editting_, c = back_;
	on_overflow(a, b, c); 
}

z::TextInput2::TextInput2(cv::Rect2i r) : z::TextInput{r}
{/// should update manually because user callback does not asusume graphical change
	gui_callback_[EVENT_KEYBOARD] = bind(&TextInput2::keyboard_callback, this, _1, _2);
}

void z::TextInput2::keyboard_callback(int key, int)
{
	bool run_key_event = true;
	switch(key) {
		case DEL: run_key_event = !del(); break;
		case BACKSPACE: run_key_event = !backsp(); break;
	}
	if(run_key_event) key_event(key, 0); 
	switch(key) {
		case DOWN: down(); break;
		case UP: up(); break;
		case ENTER: new_line(); break;
	}
}

bool z::TextInput2::backsp()
{
	if(fore_ != "" || it_ == contents_ptr_->begin()) return false;
	if(prev_ == nullptr) {
		set_iter(contents_ptr_->erase(it_));
		it_--;
		line({it_->value(), editting_, back_, end_new_line_});
		draw();
		show_cursor();
	} else {
		focus(false);
		prev_->line({prev_->value(), editting_, back_, end_new_line_});
		*prev_->it_ = prev_->line();
		prev_->draw(); 
		prev_->focus(true);
		prev_->update();
		set_iter(contents_ptr_->erase(it_));
		if(!is_end()) line(*it_);
		else line({"", "", "", false});
		draw();
		auto it = it_;
		if(next_ != nullptr && it != contents_ptr_->end()) next_->down_stream(++it);
	}
	return true;
}

bool z::TextInput2::del()
{
	if(editting_ + back_ != "") return false;
	auto it = it_;
	if(++it == contents_ptr_->end()) return false;
	set_iter(contents_ptr_->erase(it_));
	std::string s = it_->fore + it_->editting + it_->back;
	auto t = pop_front_utf(s);
	line({fore_, t, s, it_->new_line});
	*it_ = line();
	draw();
	it = it_;
	if(next_ != nullptr) next_->down_stream(++it);
	return true;
}

void z::TextInput2::new_line() {
	Line next_line = {"", editting_, back_, false};
	if(next_ != nullptr) {
		line({fore_, "", "", true});
		*it_ = line();
		auto it = it_;
		it = contents_ptr_->insert(++it, next_line);
		next_->down_stream(it);
		next_->focus(true);
		focus(false);
	} else {
		*it_ = {fore_, "", "", true};
		prev_->up_stream(it_++);
		set_iter(contents_ptr_->insert(it_, next_line));
		line(*it_);
	}
	draw();
}

void z::TextInput2::focus(bool v) {
	z::Widget::focus(v);
	if(!v) *it_ = line();
}

void z::TextInput2::set_iter(iter it) {
	it_ = it;
	if(is_end()) activated(false);
	else activated(true);
}

void z::TextInput2::down() {
	if(next_ != nullptr) {
		if(next_->activated()) {
			next_->focus(true);
			focus(false);
		}
	} else {//scroll
		auto it = it_;
		if(++it == contents_ptr_->end()) return;
		*it_ = line();
		prev_->up_stream(it_++);
		line(*it_);
		draw();
		focus(true);
	}
}

void z::TextInput2::up() {
	if(prev_ != nullptr) {
		prev_->focus(true);
		focus(false);
	} else {
		if(contents_ptr_->begin() == it_) return;
		*it_ = line();
		next_->down_stream(it_--);
		line(*it_);
		draw();
		focus(true);
	}
}

void print(z::Line a) {
	spdlog::debug("Line {} {} {} {}", a.fore, a.editting, a.back, a.new_line);
}

void z::TextInput2::down_stream(iter reset_it, int level)
{ /// next->down_stream(it)
	set_iter(reset_it);
	if(!is_end()) line(*it_);
	else line({"", "", "", false});
	draw();
	*parent_ << *this;
	if(next_ != nullptr && reset_it != contents_ptr_->end()) next_->down_stream(++reset_it, level + 1);
}

void z::TextInput2::up_stream(iter reset_it)
{ /// recursion to upside
	set_iter(reset_it);
	line(*it_);
	draw();
	*parent_ << *this;
	if(prev_ != nullptr) prev_->up_stream(--reset_it);
}

bool z::TextInput2::empty() const
{
	return fore_ == "" && editting_ == "" && back_ == "" && !end_new_line_;
}
void z::TextInput2::line(z::Line l)
{
	fore_ = l.fore; editting_ = l.editting; back_ = l.back; end_new_line_ = l.new_line;
}
z::Line z::TextInput2::line() const
{
	return {fore_, editting_, back_, end_new_line_};
}

z::TextBox::TextBox(cv::Rect2i r, int lines) : z::Window{"", r}
{
	contents_.push_back(Line{});
	int h = height / lines;
	for(int i=0; i<lines; i++) 
		inputs_.emplace_back(make_shared<z::TextInput2>(cv::Rect2i{0, i*h, width, h}));
	z::TextInput2 *prev_input = nullptr, **next_of_prev_input = nullptr;
	auto contents_it_ = contents_.begin();
	for(auto it : inputs_) {
		it->contents_ptr_ = &contents_;
		*this + *it; 
		it->set_iter(contents_it_);
		if(contents_it_ != contents_.end()) contents_it_++;
		it->prev_ = prev_input;
		prev_input = it.get();
		if(next_of_prev_input) *next_of_prev_input = it.get();
		next_of_prev_input = &it->next_;
	}
	organize_accordingto_zindex();
}

//void z::TextBox::sync() {
//	for(auto a : inputs_) {
//		if(a->it_ != contents_.end()) *a->it_ = a->line();
//		else if(!a->empty()) {
//			contents_.push_back(a->line());
//			a->it_ = --contents_.end();
//		}
//	}
//}
//
//void z::TextBox::draw()
//{
//	
//}
