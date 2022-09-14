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

z::TextInput::TextInput(cv::Rect2i r) : z::Widget{r}
{
	shade_rect({0, 0, width, height}, 4, highlight_color_, click_color_, highlight_color_);
	gui_callback_[EVENT_KEYBOARD] = bind(&z::TextInput::key_event, this, _1, _2);
}

z::TextInput::~TextInput() {
	hangul_ic_delete(hic);
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
		if(key == 8 && hangul_ic_is_empty(hic)) backspace();
		else {
			hangul_ic_process(hic, key);
			const ucschar *commit = hangul_ic_get_commit_string(hic);
			const ucschar *preedit = hangul_ic_get_preedit_string(hic);
			if(*commit != 0) value_ += GetUnicodeChar(*commit);
			editting = GetUnicodeChar(*preedit);
			if(key == 32) value_ += ' ';
		}
	} else {
		value_ += GetUnicodeChar(*hangul_ic_flush(hic));
		if(key >= 0x20 && key <= 0x7e) value_ += key;
		else if(key == 8) backspace();
		else return;
	}
	shade_rect({0, 0, width, height}, 4, highlight_color_, click_color_, highlight_color_);
	ft2_->putText(mat_, value_ + editting, {10, 0}, height * 0.8, {0, 0, 0}, -1, 4, false);
	show();
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
		if(value_.back() & 0x80) value_.pop_back();
	}
}

void z::TextInput::value(string s)
{
	value_ = s;
	shade_rect({0, 0, width, height}, 4, highlight_color_, click_color_, highlight_color_);
	ft2_->putText(mat_, value_, {10, 0}, height * 0.8, {0, 0, 0}, -1, 4, false);
}
