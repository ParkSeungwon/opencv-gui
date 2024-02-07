#include<iostream>
#include<opencv2/opencv.hpp>
#include"zgui.h"
using namespace std;

z::Label::Label(string s, cv::Rect2i r) : z::Widget{r}
{/// Label will be created with text at the position r
	text(s);
}

string z::Label::text() const
{/// gets the text of a label
	return text_;
}

void z::Label::text(string s)
{/// sets the text as s
	text_ = s;
	mat_ = background_color_;
  int baseline = 0;
	auto sz = getTextSize(text_, cv::FONT_HERSHEY_SIMPLEX, height * 0.025, 1, &baseline);
	putText(mat_, text_, {0, (height + sz.height)/2}, cv::FONT_HERSHEY_SIMPLEX, height * 0.025, {0, 0, 0}, 1, cv::LINE_AA, false);
}
