#include <pybind11/embed.h>
#include <pybind11/stl.h>
#include"CvPlot/cvplot.h"
#include"src/zgui.h"
namespace py = pybind11;
using namespace py::literals;
using namespace std;

struct Win : z::AsciiWindow
{
	Win() : z::AsciiWindow{R"(
	  WPlotting-------------------------------------------------------
		| 
		| I0----------------------------------------------------------
		| ||
		| |
		| |
		| |
		| |
		| |
		| |
		| |
		| |
		| |
		| |
		| |
		| |
		| |
		| |
		| |
		| |
		| |
		| |
		| |
		| |
		| |
		|
		| E0----------------------------------------------------------
		| ||
		| |
		| |
		| |
		| |
		| |
		| |
		| |
		| |
		| |
		| |
		| |
		| |
		|
		| T0------------  B0-------------------  B1-------------------
		| |b-o|           |Plot|                 |Quit|
		|)"}
	{
		E[0]->value("import numpy as np\nx=np.linspace(0, 10, 30)\ny=np.sin(x)");
		start();
		B[0]->click([this]() {
			try {
				auto locals = py::dict();
				py::exec(E[0]->value(), py::globals(), locals);
				auto x = locals["x"].cast<std::vector<float>>();
				auto y = locals["y"].cast<std::vector<float>>();
				vector<cv::Point2f> point;
				for(int i=0; i<x.size(); i++) point.push_back({x[i], y[i]});
				auto axes = CvPlot::plot(point, T[0]->value());
				*I[0] = axes.render(I[0]->height, I[0]->width);
				I[0]->update();
			} catch(const exception &e) {
				cerr << e.what() << endl;
			} catch(...) {
				cerr << "error occured " << endl;
			}
		});
		B[1]->click([this](){cv::destroyAllWindows();});
	}
};

int main() {
	py::scoped_interpreter guard{};
	Win win;
	win.loop();
}
