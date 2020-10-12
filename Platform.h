#include <Servo.h>
class Platform {
private:
	Servo _hor, _ver;
	int _horAngle, _verAngle;
public:
	Platform(Servo horizontal, Servo vertical);
	void init();
	void rotateHorizontally(int angle);
	void rotateVertically(int angle);
}