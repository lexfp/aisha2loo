#include <WiFi.h>
#include <vector>
#include "LooMotion.h"
#include "time.h"

/** The code herein written by the Author is released under the terms of the unlicense. https://unlicense.org/
 * @author https://github.com/lexfp 
 */
LooMotion::LooMotion() {
  motionState = 0;
}

void LooMotion::setMotionState(bool state) {
  motionState = state;
  if (motionState == 1) {
    recordTime();
  }
}

bool LooMotion::getMotionState() {
  return motionState;
}


unsigned long LooMotion::getTime() {
  time_t now;
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    Serial.println("Failed to obtain time");
    return (0);
  }
  time(&now);
  return now;
}

void LooMotion::recordTime() {
  times.push_back(getTime());
  //check if greater than certain number
  //Serial.print("Size of times");
  //Serial.println(times.size());
  /**
    for (auto it = times.rbegin(); it != times.rend(); it++)
    {
    Serial.print(*it);
    Serial.print(',');
    }
    Serial.println(' ');
  */
  if (times.size() >= 5) {
    //Serial.println("Max size exceeded - deleting first item in list");
    times.erase(times.begin());
  }
}

String LooMotion::toJSON() {
  bool firstComma = 0;
  String t = "";
  for (auto it = times.rbegin(); it != times.rend(); it++)
  {
    if (firstComma == 0) {
      firstComma = 1;
    } else {
      t += ",";
    }
    t += *it;
  }
  //Serial.println(t);

  String p1 = "{\"motion\":";
  String p2 = ",\"times\":[";
  String p3 = "]}";
  String result =  p1 + motionState + p2 + t + p3;
  return result;
}
