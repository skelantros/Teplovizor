#include "Platform.h"

void Platform::init() {
    _hor.write(0);
    _ver.write(90);
    _horAngle = 0;
	_verAngle = 90; 
}

Platform::Platform(Servo horizontal, Servo vertical) {
    _hor = horizontal;
	_ver = vertical;
	init();
}

void Platform::rotateHorizontally(int angle) {
    _horAngle += angle;
    _hor.write(_horAngle);
}
void Platform::rotateVertically(int angle) {
    _verAngle += angle;
    _ver.write(_verAngle);
}
