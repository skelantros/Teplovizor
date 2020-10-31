#include "Platform.h"

Platform::Platform(int horizontal, int vertical) {
    _hor.attach(horizontal);
	_ver.attach(vertical);
    _irTemp = Adafruit_MLX90614();
	_irTemp.begin();
}

void Platform::setHorAngle(int angle) {
  _horAngle = angle;
  _hor.write(angle);
}
void Platform::setVerAngle(int angle) {
  _verAngle = angle;
  _ver.write(angle);
}
void Platform::rotateHorizontally(int angle) {
    setHorAngle(_horAngle + angle);
}
void Platform::rotateVertically(int angle) {
    setVerAngle(_verAngle + angle);
}

double Platform::takeObjectMeasure() {
  return _irTemp.readObjectTempC();
}

double Platform::takeAmbientMeasure() {
  return _irTemp.readAmbientTempC();
}