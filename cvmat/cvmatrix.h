#pragma once
#include<string>
#include<optional>
#include<opencv2/opencv.hpp>
#include <opencv2/video/background_segm.hpp>
#include"src/QrCode.hpp"

class CVMat : public cv::Mat
{
public:
	CVMat(const cv::Mat& r);
	CVMat(const qrcodegen::QrCode& r, int scale = 8, int border = 4);
	CVMat() = default;
	std::string info(); 
	//operator cv::Mat();
	void contain_resize(int w, int h);
	void filter(const Mat& m);//apply kernel
	void show(std::string title="OpenCV");
	void restore();
	void save();
	void read_xpm(const char **xpm);
	
	std::pair<int, int> text(std::string text_to_draw,//return width,height of text
			cv::Point lower_left_of_text = {0,20},
			double scale = 1,//multiply
			cv::Scalar bgra_color = {0,0,0,255},
			int thickness = 2, int fontFace = cv::FONT_HERSHEY_SIMPLEX, 
			int lineType = 8, bool bottomleftorigin = false);

	template<class T> int polyline(const T& x, const T& y, cv::Scalar color = {0,0,0,255},
			int thickness = 2, int shift = 0, bool is_closed = true) {
		const int sz = x.size();
		cv::Point pt[sz];
		const cv::Point* p = pt;
		for(int i=0; i<sz; i++) pt[i] = {x[i], y[i]};
		polylines(*this, &p, &sz, 1, is_closed, color, thickness, 8, shift);
		return sz;
	}

	template<class T> T *pixel(int x, int y) {
		T* p = data;
		return p + ((y * cols + x) * channels());
	}

	CVMat imread(std::string filename);
	void imwrite(std::string filename);
	void gray();
	void scale(float x, float y);
	void noise(int scale);//normal distrubution noise
	void median(int ksize);//median is good for salt&pepper noise
	void normalize(float a, float b);
	void diffx();//after normalize to float
	void diffy();
	void edge(int lowthreshold=30, int thresXratio=100);//Canny
	void corner(float k = 0.04, int block = 3, int aperture = 3);//harris gray->
	int draw_detected_corner(float thres = 0.01);
	int detect_line(int threshold=180, int continuous=50, int hop=10);//edge->Hough
	std::optional<std::vector<cv::Point>> get_rect();
	int detect_circle(int canny_threshold=200, int center_threshold=100,//gray->circ 
			int min_radius=0, int max_radius=0);//gradient를 보므로 edge로 하면 안됨.
	int detect_face(cv::Size min={30, 30}, cv::Size max=cv::Size());//gray->face
	int detect_contours(int mode = cv::RETR_EXTERNAL, int method= cv::CHAIN_APPROX_SIMPLE);
	void draw_detected_contours(int min_area=0, int max_point = 10, int thickness=1,int linetype=8, int maxlevel=INT_MAX);
	void draw_detected_line(cv::Scalar color = {0,0,255});
	void draw_detected_circle(cv::Scalar color = {0,0,255});
	void draw_detected_face();
	void fourier(std::string window = "Fourier");//after gray
	cv::Mat show_fourier() const;
	void fourier_filter(double cutoff, bool lowpass = true);
	void inv_fourier();//after fourier
	void fourier_add_qr(cv::Mat m);
	cv::Mat get_plane0();
	cv::Mat histo(std::string windwo = "Histogram") const;//after gray
	template<typename T> void feature();
	void draw_feature();
	std::vector<cv::DMatch> match(const CVMat& r, double thres = 0.5) const;
	void remove_background();
	void rotate(double angle, cv::Point center={-1,-1}, double scale=1);
	void transform3(cv::Point2f src[3], cv::Point2f dst[3], cv::Size sz = {0,0});
	void transform4(cv::Point2f src[4], cv::Point2f dst[4], cv::Size sz = {0,0});
	std::vector<cv::Point> get_points(int k);// ^ affine and perspective transform
	void get_businesscard(std::vector<cv::Point> v);
	std::vector<cv::Vec4i> lines() { return lines_;}
	void title(std::string s) { title_ = s; }
	std::string title() const { return title_; }
	void cartoonize(float sig_s=60, float sig_r = 0.45);
	void resize(cv::Size sz);
	
protected:
	cv::Mat save_, harris_, descriptor_, fourier_;
	std::vector<Mat> bgr_;
	std::vector<cv::Vec4i> lines_, hierachy_;//hierachy for contour
	std::vector<cv::Vec3f> circles_;
	std::vector<cv::Rect> faces_;
	std::vector<std::vector<cv::Point>> contours_;
	std::string title_;

public:
	std::vector<cv::KeyPoint> keypoints_;

private:
	bool fourier_center_low_ = false;
	void template_init();
};


static cv::Mat BLUR = (cv::Mat_<float>(3,3) << 1,1,1,1,1,1,1,1,1) / 9;
static cv::Mat GAUSSIAN = (cv::Mat_<float>(3,3) << 1,2,1,2,4,2,1,2,1) / 16;;
static cv::Mat SHARPEN = (cv::Mat_<float>(3,3) << 0,-1,0,-1,5,-1,0,-1,0);
//static cv::Mat GAUS = cv::getGaussianKernel(9, 0.5, CV_32F);
static cv::Mat SOBELX = (cv::Mat_<float>(3,3) << -1,0,1,-2,0,2,-1,0,1);
static cv::Mat SOBELY = (cv::Mat_<float>(3,3) << -1,-2,-1,0,0,0,1,2,1);
static cv::Mat DIM1 = (cv::Mat_<float>(3,3) << -1,1,-1,1,-1,1,-1,1);
