#include <Servo.h>
#include "Adafruit_MLX90614.h"
class Platform {
private:
  Servo _hor, _ver;
  int _horAngle, _verAngle;
  Adafruit_MLX90614 _irTemp;
public:
  Platform(int horizontal, int vertical);
  Platform();
  void rotateHorizontally(int angle);
  void rotateVertically(int angle);
  void setHorAngle(int angle);
  void setVerAngle(int angle);
  double takeObjectMeasure();
  double takeAmbientMeasure();
};