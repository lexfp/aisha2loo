#include <esp8266-google-home-notifier.h>

/** The code herein written by the Author is released under the terms of the unlicense. https://unlicense.org/
 * @author https://github.com/lexfp 
 */
class GoogleHome
{
  private:
    GoogleHomeNotifier ghn;
  public:
    void init(char googleHomeName[]);
    void notifyTTS(const char text[]);
    void notifyMP3(const char url[]);
};
