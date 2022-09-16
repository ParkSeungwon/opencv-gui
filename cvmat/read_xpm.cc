#include<sstream>
#include<map>
#include"cvmatrix.h"
using namespace std;
using namespace cv;

Vec3b change(const char* p)
{///change color code 7B543E -> uchar 232,12,122
	unsigned char c[6];
	if(*p == '#') {
		for(int i=0; i<6; i++) {
			if(p[i+1] <= '9') c[i] = p[i+1] - '0';
			else c[i] = p[i+1] - 'A' + 10;
		}
		for(int i=0; i<6; i+=2) c[i] = c[i] * 16 + c[i+1];
		return {c[4], c[2], c[0]};
	} else return {0,0,0};
}

void CVMat::read_xpm(const char** xpm) {
	stringstream ss;
	ss << xpm[0];
	size_t w, h, color, ch;//ch = how many characters per color
	ss >> w >> h >> color >> ch;

	map<string, Vec3b> color_map;
	for(int i=0; i<color; i++)//make colormap
		color_map[string(xpm[i+1], ch)] = change(xpm[i+1] + ch + 3);
	create(w, h, CV_8UC3);//header creation
	for(int y=0; y<h; y++) for(int x=0; x<w; x++)//populate pixels
		at<Vec3b>(y, x) = color_map[string(xpm[color+1+y] + x*ch, ch)];
}

