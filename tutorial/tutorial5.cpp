#include"src/zgui.h"
using namespace std;

struct Win : z::Window 
{
	Win() : z::Window{"Scrolled Window Example", {0, 0, 800, 800} }
	{
		scroll_to({0, 0, 400, 400});// before adding handle
		*this + label + handle;
		start();
	}
	z::Handle handle;
	z::Label label{"Center", {300, 300, 100, 30}};
};

int main() {
	Win win;
	win.loop();
}

