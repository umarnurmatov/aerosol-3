#include "devices.hpp"

namespace modules
{
class Encoder
{
public:
  void init(); 

  // Updates state
  // State can be obtained using isClick, isRight and isLeft
  void tick();
  
  // State won't be updated untill next tick()
  int32_t isClick();

  // State won't be updated untill next tick()
  int32_t isRight();

  // State won't be updated untill next tick()
  int32_t isLeft();

private:
  static void IRAM_ATTR readEncoderISR();
  int32_t enc_pos, enc_prev_pos = 0;
  bool _isClick, _isRight, _isLeft;
};

} // namespace modules
