#include"zgui.h"
using namespace std;
using namespace cv;

z::Quartz::Quartz(cv::Rect2i r) : z::Widget(r)
{
	mat_tmp_ = background_;
	int basex = 30, basey = 10;
	
	p[0] = 
	{ { basex, basey}
	, { basex + 100, basey}
	, { basex + 110, basey +10}
	, { basex + 100, basey +20}
	, { basex, basey+20}
	, { basex - 10, basey+10}
	, { basex, basey}
	};
	p[3] =
 	{ { basex - 10, basey + 15}
	, { basex, basey + 25}
	, { basex, basey + 125}
	, { basex - 10, basey + 135}
	, { basex - 20, basey + 125}
	, { basex - 20, basey + 25}
	, { basex - 10, basey + 15}
	};
	for(auto &a : p[3]) a += Point2i{0, 2};
	p[1] = p[2] = p[0];
	p[4] = p[5] = p[6] = p[3];
	for(auto &a : p[1]) a += Point2i{0, 135};
	for(auto &a : p[2]) a += Point2i{0, 267};
	for(auto &a : p[4]) a += Point2i{120, 0};
	for(auto &a : p[5]) a += Point2i{0, 135};
	for(auto &a : p[6]) a += Point2i{120, 135};

	number(10);
}

z::Quartz::Quartz() : Quartz({0,0,50,100}) { }

int z::Quartz::number() { return num_; }
void z::Quartz::number(int k)
{ /// 0-9 : number, 10 : blank, 11 : minus
	num_ = k;
	if(k < 0) k = 11;
	if(k > 11) return;
	for(int i=0; i<7; i++) fillConvexPoly(mat_tmp_, p[i], off_);
	for(int i : lcd_on_[k]) fillConvexPoly(mat_tmp_, p[i], on_);
	cv::resize(mat_tmp_, mat_, mat_.size());
	update();
}

