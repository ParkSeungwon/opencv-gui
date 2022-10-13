#include"CvPlot/cvplot.h"
#include"src/zgui.h"
using namespace std;

struct Win : z::AsciiWindow
{
	Win() : z::AsciiWindow{R"(
	  WIntegration with CvPlot----------------------
		|
		|  I0------------------------------ B0------
		|  |Lenna.png|                      |Quit|
		|  |
		|  |                                B1------
		|  |                                |Plot|
		|  |
		|  |                  
		|  |                 
		|  |
		|  |
		|  |
		|  |
		|  |
		|  |
		|  |
		|
		|)", 20, 30}
	{
		start();
		B[0]->click([this]() { cv::destroyAllWindows(); });
		B[1]->click([this]() { 
			auto axes = CvPlot::plot(std::vector<double>{ 3, 3, 4, 6, 4, 3 }, "-o");
			*I[0] = axes.render(I[0]->height, I[0]->width);
			I[0]->update();
		});
	}
};

int main() {
	Win win;
	win.loop();
}
