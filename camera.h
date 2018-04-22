# include <SFML/Graphics.hpp>

using namespace sf;
using namespace std;

View camera;

float cam_x = 675, cam_y = 350;

//void movingCamera(float x, float y);
void movingCamera(float *x, float *y, float expCadr) {
	int cycleExp = expCadr * 100;
	cycleExp = cycleExp % 4;
	
	switch (cycleExp) {
	case 0: cam_x = 665; break;
	case 1: cam_x = 685; break;
	case 2: cam_x = 670; break;
	case 3: cam_x = 680; break;
	}
	
	camera.setCenter(*x, *y);
}

