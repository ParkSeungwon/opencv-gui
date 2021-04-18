#include<opencv2/opencv.hpp>
#include<opencv2/objdetect.hpp>
#include<zbar.h>
#include"QrCode.hpp"
using qrcodegen::QrCode;
using qrcodegen::QrSegment;

struct decodedObject {
	std::string type;
	std::string data;
	std::vector<cv::Point> location;
};

// Find and decode barcodes and QR codes
std::vector<decodedObject> decode(cv::Mat &im)
{
	zbar::ImageScanner scanner; // Create zbar scanner
  scanner.set_config(zbar::ZBAR_QRCODE, zbar::ZBAR_CFG_ENABLE, 1); // Configure scanner

	cv::Mat imGray; // Convert image to grayscale
	cv::cvtColor(im, imGray, cv::COLOR_BGR2GRAY);

  // Wrap image data in a zbar image
	zbar::Image image(im.cols, im.rows, "Y800", (uchar *)imGray.data, im.cols * im.rows);
  int n = scanner.scan(image); // Scan the image for barcodes and QRCodes

	std::vector<decodedObject> decodedObjects;
  for(auto symbol = image.symbol_begin(); symbol != image.symbol_end(); ++symbol) {
    decodedObject obj;
    obj.type = symbol->get_type_name();
    obj.data = symbol->get_data();
    decodedObjects.push_back(obj);
  }
	return decodedObjects;
}

int main()
{
	const char *text = "https://adnet.zeta2374.com";              // User-supplied text
	
	// Make and print the QR Code symbol
	const QrCode qr = QrCode::encodeText(text, QrCode::Ecc::LOW);
	{
		std::ofstream f{"a.svg"};
		f << qr.toSvgString(4);
	}

	system("convert a.svg qr.png");
	cv::Mat mat = cv::imread("qr.png");
	cv::imshow("QR code", mat);

	for(const auto &[type, data, loc] : decode(mat)) {
		std::cout << type << std::endl;
		std::cout << data << std::endl;
	}
	cv::waitKey();
}


