#include <opencv2/highgui.hpp>
#include <iostream>
#include"zgui.h"
 
using namespace std;
using namespace cv;
#define TITLE "ImageDisplay"
 
 
/*/////////////////////////////////////
@ function: CallBackFunc
*//////////////////////////////////////
void CallBackFunc( int event, int x, int y, int flags, void* userdata )
{
    if ( event == EVENT_LBUTTONDOWN )
    {
        std::cout << "Left button of the mouse is clicked - position (" << x << ", " << y << ")" << std::endl;
				cv::Mat mat = cv::imread("/home/zeta/Pictures/logo/adnet.png");
				cv::imshow(TITLE, mat);
    }
    else if ( event == EVENT_RBUTTONDOWN )
    {
        std::cout << "Right button of the mouse is clicked - position (" << x << ", " << y << ")" << std::endl;
    }
    else if ( event == EVENT_MBUTTONDOWN )
    {
        std::cout << "Middle button of the mouse is clicked - position (" << x << ", " << y << ")" << std::endl;
    }
    else if ( event == EVENT_MOUSEMOVE ) [[likely]]
    {
        std::cout << "Mouse move over the window - position (" << x << ", " << y << ")" << std::endl;
    }
    else if ( event == EVENT_LBUTTONUP )
    {
        std::cout << "Left button of the mouse is released - position (" << x << ", " << y << ")" << std::endl;
				cv::Mat mat = cv::imread("/home/zeta/Pictures/11.jpg");
				cv::imshow(TITLE, mat);
    }
    else if ( event == EVENT_RBUTTONUP )
    {
        std::cout << "Right button of the mouse is released - position (" << x << ", " << y << ")" << std::endl;
    }
    else if ( event == EVENT_MBUTTONUP )
    {
        std::cout << "Middle button of the mouse is released - position (" << x << ", " << y << ")" << std::endl;
    }
    else if ( event == EVENT_LBUTTONDBLCLK )
    {
        std::cout << "Left button of the mouse is double-clicked - position (" << x << ", " << y << ")" << std::endl;
    }
    else if ( event == EVENT_RBUTTONDBLCLK )
    {
        std::cout << "Right button of the mouse is double-clicked - position (" << x << ", " << y << ")" << std::endl;
    }
    else if ( event == EVENT_MBUTTONDBLCLK )
    {
        std::cout << "Middle button of the mouse is double-clicked - position (" << x << ", " << y << ")" << std::endl;
    }
 
    if ( flags & cv::EVENT_FLAG_LBUTTON )
    {
        std::cout << "\tcv::EVENT_FLAG_LBUTTON" << std::endl;
    }
    if ( flags & cv::EVENT_FLAG_RBUTTON )
    {
        std::cout << "\tcv::EVENT_FLAG_RBUTTON" << std::endl;
    }
    if ( flags & cv::EVENT_FLAG_MBUTTON )
    {
        std::cout << "\tcv::EVENT_FLAG_MBUTTON" << std::endl;
    }
    if ( flags & cv::EVENT_FLAG_CTRLKEY )
    {
        std::cout << "\tcv::EVENT_FLAG_CTRLKEY" << std::endl;
    }
    if ( flags & cv::EVENT_FLAG_SHIFTKEY )
    {
        std::cout << "\tcv::EVENT_FLAG_SHIFTKEY" << std::endl;
    }
    if ( flags & cv::EVENT_FLAG_ALTKEY )
    {
        std::cout << "\tcv::EVENT_FLAG_ALTKEY" << std::endl;
    }
 
}
 
void bcall(int, void*) {}
/*/////////////////////////////////////
@ function: main
*//////////////////////////////////////
int main()
{
    /// Read image from file
    cv::Mat img = cv::imread("/home/zeta/Pictures/11.jpg");
  
    /// if fail to read the image
    if ( img.empty() )
    {
        std::cout << "Error loading the image" << std::endl;
        return -1;
    }
  
    /// Create a window
    cv::namedWindow(TITLE, 0 );

  
    /// set the callback function for any mouse event
	 cv::setMouseCallback( "ImageDisplay", CallBackFunc, NULL );
  
    /// show the image
		//cv::setWindowProperty("ImageDisplay", cv::WND_PROP_AUTOSIZE, cv::WINDOW_NORMAL);
		//cv::setWindowProperty("ImageDisplay", cv::WND_PROP_ASPECT_RATIO, cv::WINDOW_KEEPRATIO);
		z::Widget w{{700, 200, 100, 100}};
		//zeta::Button b{{100, 100, 50, 20}};
		cv::Mat mat = cv::imread("/home/zeta/Pictures/11.jpg");
    cv::imshow(TITLE, mat);
  
    /// Wait until user press some key
    waitKey( 0 );
     
    return 0;
}

