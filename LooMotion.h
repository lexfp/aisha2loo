#include <vector>

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
