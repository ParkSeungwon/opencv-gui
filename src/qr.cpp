#include<fstream>
#include<opencv2/opencv.hpp>
#include<opencv2/objdetect.hpp>
#include<zbar.h>
#include"QrCode.hpp"

std::string decode(cv::Mat &im)
{// Find and decode barcodes and QR codes
	cv::Mat imGray;
	cv::cvtColor(im, imGray, cv::COLOR_BGR2GRAY);

	zbar::Image image(im.cols, im.rows, "Y800", (uchar *)imGray.data, im.cols * im.rows);
	zbar::ImageScanner scanner;
  scanner.set_config(zbar::ZBAR_QRCODE, zbar::ZBAR_CFG_ENABLE, 1);
  int n = scanner.scan(image);//image may contain multiple qr codes
  return image.symbol_begin()->get_data(); 
}

int main()
{
	const char *text = "https://adnet.zeta2374.com";
	const auto qr = qrcodegen::QrCode::encodeText(text, qrcodegen::QrCode::Ecc::LOW);

	{
		std::ofstream f{"a.svg"};
		f << qr.toSvgString(4);
	}

	system("convert a.svg qr.png");
	cv::Mat mat = cv::imread("qr.png");
	cv::imshow("QR code", mat);

	std::cout << decode(mat) << std::endl;
	cv::waitKey();
}


