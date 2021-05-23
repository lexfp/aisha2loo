#include <stdint.h>

class PushButton
{
  private:
    const long buttonClickTime = 100;
    bool buttonDown;
    uint32_t lastTimeStamp;
    int pushButtonPin;
  public:
    void init(int pin);
    bool checkForClick();
};
