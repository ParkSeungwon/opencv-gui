#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/objdetect.hpp>

int main(int argc, char** argv) {
    if (argc != 2) {
        std::cerr << "사용법: " << argv[0] << " <이미지 파일 경로>" << std::endl;
        return -1;
    }

    std::string imagePath = argv[1];
    cv::Mat image = cv::imread(imagePath);
    if (image.empty()) {
        std::cerr << "이미지를 불러올 수 없습니다: " << imagePath << std::endl;
        return -1;
    }

    cv::QRCodeDetector qrDecoder;
    std::string decodedText = qrDecoder.detectAndDecodeCurved(image);

    if (decodedText.empty()) {
        std::cout << "QR 코드를 찾을 수 없습니다." << std::endl;
    } else {
        std::cout << "디코딩된 QR 코드: " << decodedText << std::endl;
    }

    return 0;
}
