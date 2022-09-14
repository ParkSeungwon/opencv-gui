#include<opencv2/opencv.hpp>
#include"zgui.h"
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


HangulInputContext* z::TextInput::hic_ = hangul_ic_new("2");
HanjaTable* z::TextInput::table_ = hanja_table_load(NULL);

z::TextInput::TextInput(cv::Rect2i r) : z::Widget{r}
{
	shade_rect({0, 0, width, height}, 4, highlight_color_, click_color_, highlight_color_);
	gui_callback_[EVENT_KEYBOARD] = bind(&z::TextInput::key_event, this, _1, _2);
}

void z::TextInput::key_event(int key, int)
{
	//if(key == 13) return user_callback_[EVENT_KEYBOARD](0, 0);
	string editting;
	if(key == 234) {
		hangul_mode_ = !hangul_mode_;
		return;
	}
	if(hangul_mode_) {
		if(key == 8 && hangul_ic_is_empty(hic_)) backspace(); //backspace
		else if(key == 228 && !hangul_ic_is_empty(hic_)) { // hanja popup
			editting = GetUnicodeChar(*hangul_ic_get_preedit_string(hic_));
			HanjaList* list = hanja_table_match_exact(table_, editting.data());
			vector<string> v;
			if(list != nullptr) for(int i=0; i<hanja_list_get_size(list); i++) {
				const char* hanja = hanja_list_get_nth_value(list, i);
				cout << hanja << flush;
				v.push_back(hanja);
			}
			hanja_list_delete(list);
			if(!v.empty()) {
				hangul_ic_reset(hic_);
				popup(v);
			}
		} else { // make character
			hangul_ic_process(hic_, key);
			const ucschar *commit = hangul_ic_get_commit_string(hic_);
			const ucschar *preedit = hangul_ic_get_preedit_string(hic_);
			if(*commit != 0) value_ += GetUnicodeChar(*commit);
			editting = GetUnicodeChar(*preedit);
			//if(key == 32) value_ += ' ';
			if(!isalpha(key) && isprint(key)) value_ += key;
		}
	} else {
		value_ += GetUnicodeChar(*hangul_ic_flush(hic_));
		if(isprint(key)) value_ += key;
		else if(key == 8) backspace();
		else return;
	}
	shade_rect({0, 0, width, height}, 4, highlight_color_, click_color_, highlight_color_);
	ft2_->putText(mat_, value() + editting, {10, 0}, height * 0.8, {0, 0, 0}, -1, 4, false);
	show();
}

struct HanjaWin : z::Window
{
	HanjaWin(int button_size) : z::Window { "hanja", {0,0,1,1} }
	{ button_sz = button_size; }
	vector<shared_ptr<z::Button>> bts;
	int button_sz = 10;
	void set_hanja(vector<string> v) {
		bts.clear();
		widgets_.clear();
		resize({ 0, 0, std::min(10 * button_sz, (int)v.size() * button_sz)
			, (v.size() / 10 + 1) * button_sz });
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
	hanj.popup(*parent_, [this, v](int i) { value(value() +v[i]); *parent_ << *this; show();});
}

void z::TextInput::enter(function<void(string)> f)
{
	user_callback_[EVENT_KEYBOARD] = [this, f](int val, int){ if(val == 13) f(value());};
}

string z::TextInput::value()
{
	return value_;
}

void z::TextInput::backspace()
{
	if(!value_.empty()) {
		value_.pop_back();
		if(!value_.empty() && (value_.back() & 0x80)) value_.pop_back();
	}
}

void z::TextInput::value(string s)
{
	value_ = s;
	shade_rect({0, 0, width, height}, 4, highlight_color_, click_color_, highlight_color_);
	ft2_->putText(mat_, value_, {10, 0}, height * 0.8, {0, 0, 0}, -1, 4, false);
}
