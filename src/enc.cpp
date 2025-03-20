#include "enc.hpp"

void modules::Encoder::init()
{
  devices::enc.areEncoderPinsPulldownforEsp32=false;
  devices::enc.begin();
  devices::enc.setup(readEncoderISR);
  devices::enc.setBoundaries(0, 1000, true); //minValue, maxValue, circleValues true|false (when max go to min and vice versa)
  devices::enc.disableAcceleration();
}

void modules::Encoder::tick()
{
  enc_pos = devices::enc.readEncoder();
  _isClick = devices::enc.isEncoderButtonClicked();
  _isRight = enc_pos - enc_prev_pos > 0 ? true : false;
  _isLeft = enc_pos - enc_prev_pos < 0 ? true : false;
  enc_prev_pos = enc_pos;
}

int32_t modules::Encoder::isClick()
{
  return _isClick;
}
int32_t modules::Encoder::isRight()
{
  return _isRight;
}
int32_t modules::Encoder::isLeft()
{
  return _isLeft;
}

void IRAM_ATTR modules::Encoder::readEncoderISR()
{
  devices::enc.readEncoder_ISR();
}
