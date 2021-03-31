#include<iostream>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include<CvPlot/imp/cvplot.ipp>
using namespace cv;
using namespace std;

cv::Mat histogram(cv::Mat &mat, int normalize_max)
{//gray 3 channel(rgb same value) to 1channel histogram(256 x normalize_max)
	vector<cv::Mat> bgr;
	cv::Mat hist;
	cv::split(mat, bgr);
	int channel = 0, hist_size = 256;
	float range[] = {0, 256};
	const float *histrange = {range};
	cv::calcHist(&bgr[0], 1, &channel, cv::Mat{}, hist, 1, &hist_size, &histrange);
	//cv::normalize(hist, hist, 0, normalize_max, cv::NORM_MINMAX, -1, cv::Mat{});
	
	return hist;
}
namespace {
	int f() { return 3; }
	int k = f();
}

int main( int argc, char** argv )
{
    Mat src;
    if( argc != 2 || !(src=imread(argv[1], 1)).data ) return -1;
		Mat m = histogram(src, 192);
		auto axes = CvPlot::plot(m, "-v");
		auto mat = axes.render(400, 600);
		imshow("test", mat);
		cout << k << endl;

    waitKey();
}


