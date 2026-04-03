#pragma once

#include "Arduino.h"

class DT_Controller
{
public:
    DT_Controller(uint8_t Pin1, uint8_t Pin2);
    void begin();
    void attach(uint16_t Pin3, char *init);
    void detach();
    void set(int16_t messageID, char *setPoint);
    void update();

private:
    bool    _initialised;
    uint8_t _pin1, _pin2, _pin3;

    // Functions
    float scaleValue(float x, float in_min, float in_max, float out_min, float out_max);
    void  setTrim(float value);
    void  setFlap(float value);
    void  setInstrumentBrightness(float value);
    void  drawGauge();
    void  drawFlapGauge();
    void  drawTrimGauge();

    // Variables
    float    instrumentBrightness      = 255;
    float    flapValue = 0;
    float    trimValue = 0;
    float    flapNeedleAngle = 0;
    float    trimNeedleAngle = 0;
};