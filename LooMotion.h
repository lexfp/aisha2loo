#include <vector>

/** The code herein written by the Author is released under the terms of the unlicense. https://unlicense.org/
 * @author https://github.com/lexfp 
 */
class LooMotion
{
  private:
    bool motionState;
    std::vector<unsigned long> times;
    const int MAX_HISTORY = 5;
    unsigned long getTime();
    void recordTime();
  public:
    LooMotion();
    void setMotionState(bool state);
    bool getMotionState();
    String toJSON();
};
