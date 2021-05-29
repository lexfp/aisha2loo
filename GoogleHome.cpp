#include "GoogleHome.h"

void GoogleHome::init(char googleHomeName[]) {
  Serial.println("connecting to Google Home...");
  if (ghn.device(googleHomeName, "en") != true) {
    Serial.println(ghn.getLastError());
    return;
  }
  Serial.print("Located Google Home(");
  Serial.print(ghn.getIPAddress());
  Serial.print(":");
  Serial.print(ghn.getPort());
  Serial.println(")");
}

void GoogleHome::notifyTTS(const char text[]) {
  if (ghn.notify(text) != true) {
    Serial.println(ghn.getLastError());
  }
}
void GoogleHome::notifyMP3(const char url[]) {
  if (ghn.play(url) != true) {
    Serial.println(ghn.getLastError());
  }
}
