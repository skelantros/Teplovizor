#ifndef MEASURE_CONF_HEADER
#define MEASURE_CONF_HEADER

#include "Platform.h"

class MeasureConfigurator {
    // время "единичного" замера (замера в одном узле) в миллисекундах
    int _singleTime;
    // предельные вертикальные и горизонтальные углы замеров
    int _horMin, _horMax, _verMin, _verMax;
    // число точек дробления вертикального и горизонтального измерений
    // определяет разрешение тепловой карты
    int _horSection, _verSection;
    // платформа, связанная с конфигуратором
    Platform _platform;

    double _minTemp, _maxTemp;
    bool _areTempsMeasured;
public:
    MeasureConfigurator();
    MeasureConfigurator(int horMin, int horMax, int horSection,
                        int verMin, int verMax, int verSection,
                        int singleTime, Platform platform);
    
    double** measure();
    int getHorSection();
    int getVerSection();
    double getMinTemp();
    double getMaxTemp();
};

#endif