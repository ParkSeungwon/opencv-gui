#include<opencv2/highgui.hpp>
#include<opencv2/core.hpp>
#include<opencv2/videoio.hpp>
using namespace std;

int main(int ac, char **av)
{
	cv::Mat m{1024, 50, CV_8UC4};
	int b = 0, g = 0, r = 255;
	//cv::rectangle(m, {10, 10 , 100, 50}, {255, 0, 0});
	//for(int i=0; i<256; i++) cv::rectangle(m, {0, i * 2, 50, 2}, {r--, b, g++});
	//for(int i=256; i<512; i++) cv::rectangle(m, {0, i * 2, 50, 2}, {r, b++, g--});
	cv::imshow("bar", m);
	cv::waitKey();
}
