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
	ft2_->putText(mat_, text_, {0, 0}, height * 0.8, {0, 0, 0}, -1, 4, false);
}
