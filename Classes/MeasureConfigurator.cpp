#include "Classes/MeasureConfigurator.h"
#include <math.h>

MeasureConfigurator::MeasureConfigurator() {}
MeasureConfigurator::MeasureConfigurator(int horMin, int horMax, int horSection,
                                         int verMin, int verMax, int verSection,
                                         int singleTime, Platform platform) {
    this->_horMax = horMax;
    this->_horMin = horMin;
    this->_horSection = horSection;
    this->_verMin = verMin;
    this->_verMax = verMax;
    this->_verSection = verSection;
    this->_singleTime = singleTime;
    this->_platform = platform;
    _minTemp = _maxTemp = -1;
    _areTempsMeasured = false;
}

int MeasureConfigurator::getHorSection() { return _horSection; }
int MeasureConfigurator::getVerSection() { return _verSection; }
double MeasureConfigurator::getMinTemp() { return _minTemp; }
double MeasureConfigurator::getMaxTemp() { return _maxTemp; }

double** MeasureConfigurator::measure() {
    double** arr = new double*[_verSection];
    for(int i = 0; i < _verSection; ++i) {
        arr[i] = new double[_horSection];
    }
    _platform.setHorAngle(_horMin);
    _platform.setVerAngle(_verMin);

    double currVerAngle = _verMin;
    double currHorAngle = _horMin;
    double verDiff = (double)(_verMax - _verMin) / (_verSection - 1);
    double horDiff = (double)(_horMax - _horMin) / (_horSection - 1);


    for(int v = 0; v < _verSection; ++v) {
        for(int h = 0; h < _horSection; ++h) {
            double temp = _platform.takeObjectMeasure();
            if(!_areTempsMeasured) {
                _minTemp = _maxTemp = temp;
                _areTempsMeasured = true;
            } else if(_minTemp > temp) {
                _minTemp = temp;
            } else if(_maxTemp < temp) {
                _maxTemp = temp;
            }
            arr[v][h] = temp;
            _platform.setHorAngle((int) round(currHorAngle));
            delay(_singleTime);
            currHorAngle += horDiff;
        }
        currHorAngle = _horMin;
        currVerAngle += verDiff;
        _platform.setVerAngle((int) round(currVerAngle));
    }
    return arr;
}
