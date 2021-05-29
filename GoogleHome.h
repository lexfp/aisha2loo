#include <esp8266-google-home-notifier.h>

class GoogleHome
{
  private:
    GoogleHomeNotifier ghn;
  public:
    void init(char googleHomeName[]);
    void notifyTTS(const char text[]);
    void notifyMP3(const char url[]);
};
