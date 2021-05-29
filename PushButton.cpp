#include "PushButton.h"
#include <WiFi.h>

/** The code herein written by the Author is released under the terms of the unlicense. https://unlicense.org/
 * @author https://github.com/lexfp 
 */

void PushButton::init(int pin) {
  pushButtonPin = pin;
  pinMode(pushButtonPin, INPUT_PULLUP);
  lastTimeStamp = esp_log_timestamp();
  buttonDown = 0;
}

bool PushButton::checkForClick() {
  int Push_button_state = digitalRead(pushButtonPin);
  // if condition checks if push button is pressed
  if ( Push_button_state != HIGH )
  {
    uint32_t differenceTimeStamp = esp_log_timestamp() - lastTimeStamp;
    if (buttonDown == 0) {
      if (differenceTimeStamp > buttonClickTime)
      {
        //Serial.println("Button Down");  
        buttonDown = 1;
        lastTimeStamp = esp_log_timestamp();
        return 1;
      }
    }
  }
  else
  {
    uint32_t differenceTimeStamp = esp_log_timestamp() - lastTimeStamp;
    if (buttonDown == 1)
    {
      if (differenceTimeStamp > buttonClickTime)
      {
        //Serial.println("Button Up");
        buttonDown = 0;
        lastTimeStamp = esp_log_timestamp();
      }
    }
  }
  return 0;
}
