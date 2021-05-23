#include <stdint.h>

/** The code herein written by the Author is released under the terms of the unlicense. https://unlicense.org/
 * @author https://github.com/lexfp 
 */
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
