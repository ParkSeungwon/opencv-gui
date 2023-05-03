#include<opencv2/opencv.hpp>
#include"src/zgui.h"

class Quartz : public z::Widget
{
public:
	Quartz(cv::Rect2i r);
	Quartz();
	void number(int i);
	int number();
protected:
	int num_ = 10;
	cv::Mat mat_tmp_ = cv::Mat::zeros(310, 160, CV_8UC3);
	cv::Scalar on_{0x1a, 0x27, 0x26}, off_{0x4e, 0xd7, 0xd8}, background_{0x53, 0xE7, 0xEA};
	std::vector<cv::Point2i> p[7];
	std::vector<int> lcd_on_[12] = 
	{ { 0, 2, 3, 4, 5, 6 }
	, { 4, 6 }
	, { 0, 1, 2, 4, 5 }
	, { 0, 1, 2, 4, 6 }
	, { 1, 3, 4, 6 }
	, { 0, 1, 2, 3, 6 }
	, { 0, 1, 2, 3, 5, 6 }
	, { 0, 4, 6 }
	, { 0, 1, 2, 3, 4, 5, 6 }
	, { 0, 1, 2, 3, 4, 6 }
	, {  } // null
	, { 1 } // minus
	};
};

