#include <TFT_eSPI.h>
#include "DT_Controller.h"
#include "allocateMem.h"
#include "commandmessenger.h"
#include "RunningAverage.h"
#include "./include/Flap_Trim_Gauge.h"
#include "./include/Needle.h"

#define BACKGROUND_COLOR TFT_BLUE

static TFT_eSPI    tft;
static TFT_eSprite mainGaugeSpr = TFT_eSprite(&tft);
static TFT_eSprite needleSpr = TFT_eSprite(&tft);

RunningAverage RA_FlapNeedleRotationAngle(5);  // Running average of last 5 values of the angle rotation
RunningAverage RA_TrimNeedleRotationAngle(5);  // Running average of last 5 values of the angle rotation

/* **********************************************************************************
    This is just the basic code to set up your custom device.
    Change/add your code as needed.
********************************************************************************** */

DT_Controller::DT_Controller(uint8_t Pin1, uint8_t Pin2)
{
    _pin1 = Pin1;
    _pin2 = Pin2;
}

void DT_Controller::begin()
{
}

void DT_Controller::attach(uint16_t Pin3, char *init)
{
    _pin3 = Pin3;
    tft.init();
    tft.setRotation(0);
    tft.setPivot(120, 120);
    tft.fillScreen(TFT_RED);
    delay(3000); // wait for 3 seconds to show the red screen during startup
    tft.startWrite(); // TFT chip select held low permanently

    mainGaugeSpr.createSprite(FLAP_TRIM_GAUGE_WIDTH, FLAP_TRIM_GAUGE_HEIGHT);
    mainGaugeSpr.setPivot(120, 120);

    needleSpr.createSprite(NEEDLE_WIDTH, NEEDLE_HEIGHT);
    needleSpr.setPivot(NEEDLE_WIDTH / 2, 85);
    needleSpr.fillSprite(BACKGROUND_COLOR);
    needleSpr.pushImage(0, 0, NEEDLE_WIDTH, NEEDLE_HEIGHT, Needle);

    // Initialize runninvg average with 0 values to avoid startup issues
    RA_FlapNeedleRotationAngle.clear();
    RA_TrimNeedleRotationAngle.clear();

}

void DT_Controller::detach()
{
    if (!_initialised)
        return;
    mainGaugeSpr.deleteSprite();
    needleSpr.deleteSprite();
    tft.fillScreen(TFT_BLACK);
    tft.endWrite();
    _initialised = false;
}

void DT_Controller::set(int16_t messageID, char *setPoint)
{
    /* **********************************************************************************
        Each messageID has it's own value
        check for the messageID and define what to do.
        Important Remark!
        MessageID == -2 will be send from the board when PowerSavingMode is set
            Message will be "0" for leaving and "1" for entering PowerSavingMode
        MessageID == -1 will be send from the connector when Connector stops running
        Put in your code to enter this mode (e.g. clear a display)

    ********************************************************************************** */

    // do something according your messageID
    switch (messageID) {
    case -1:
        // tbd., get's called when Mobiflight shuts down
    case -2:
        // tbd., get's called when PowerSavingMode is entered
    case 0:
        setTrim(atof(setPoint));
        break;
    case 1:
        setFlap(atof(setPoint));
        /* code */
        break;
    case 2:
        /* code */
        break;
    default:
        break;
    }
    drawGauge();
}

void DT_Controller::update()
{
    // Do something which is required regulary
}

void DT_Controller::drawGauge()
{
    mainGaugeSpr.pushImage(0, 0, FLAP_TRIM_GAUGE_WIDTH, FLAP_TRIM_GAUGE_HEIGHT, Flap_Trim_Gauge);
    drawFlapGauge();
    drawTrimGauge();
    mainGaugeSpr.pushSprite(0, 0);
}

void DT_Controller::drawFlapGauge()
{
    flapNeedleAngle = scaleValue(flapValue, 0, 1, -45, -135);
    RA_FlapNeedleRotationAngle.addValue(flapNeedleAngle);
    needleSpr.pushRotated(&mainGaugeSpr, RA_FlapNeedleRotationAngle.getAverage(), BACKGROUND_COLOR);
}

void DT_Controller::drawTrimGauge()
{
    trimNeedleAngle = scaleValue(trimValue, -1, 1, 135, 45);
    RA_TrimNeedleRotationAngle.addValue(trimNeedleAngle);
    needleSpr.pushRotated(&mainGaugeSpr, RA_TrimNeedleRotationAngle.getAverage(), BACKGROUND_COLOR);
}

void DT_Controller::setTrim(float value)
{
    trimValue = value;
}

void DT_Controller::setFlap(float value)
{
    flapValue = value;
}

void DT_Controller::setInstrumentBrightness(float value)
{
    float pwmOutput = 0;
    instrumentBrightness = scaleValue(value, 0, 1, 0, 255);
    pwmOutput = sq(instrumentBrightness) / 255.0;  // needed to correct PWM output due to human eye brightness perception
    analogWrite(TFT_BL, pwmOutput);
}

float DT_Controller::scaleValue(float x, float in_min, float in_max, float out_min, float out_max)
{
    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}
