# aisha2loo
esp32+PIR motion detector for alerting me to when my dog goes to the loo


# License
The code herein written by the Author is released under the terms of the unlicense. https://unlicense.org/

# Required:
1) ESP32
2) Motion sensor (PIR or microwave) - wired to 5v, ground, and pin 27

# Optional:
1) Push button - used for development/testing to simulate triggering motion detection
2) Google home - this will broadcast to google home as well (there are limits to the # of api calls though)
3) web browser - this is the other way to get notified when motion is detected (uses websockets)

# Description
So we got this new puppy and trained her to use the bathroom cause it sucks having to take her out during thunderstorms and such.
Unfortunately, we found out that if we don't clean up after her poops, she takes care of it herself by uh...eating them. 
Without getting into more details, we needed a mechanism by which we can get alerted to when she goes to do her business. 

# Things to note:
1) Building this was pretty easy and straightforward, but...
2) The PIR sensor gave so many false alarms and was the hardest part of the project. I ordered a bunch of microwave detectors, but they didn't arrive in time. I eventually got it tuned pretty well by moving the sensor away from the esp32 itself since the 2.4ghz transmission from the esp32 was triggering the alerts. It also helped to adjust the different knobs on the PIR sensor itself even though they were not correctly documented as to which does what. 
3) I used a push button to simulate triggering motions during development since the PIR detector was a pain to work with. If you want to add one, put it on pin 15 and ground.
4) You'll need either a web browser or google home to be notified of alerts. For the web browser, you'll need to figure out either from your home router the IP address of your esp32 or using the serial monitor tool from the Arduino IDE while it boots up which will print the IP. Unfortunately, there is an api limit to the text to speech, so after the limit is reached, your google home will justs kind of beep. It works, but isn't as cool as a voice notification. Alternatively, you can upgrade the code to cast an mp3 recording which is something I may look into if time permits. 





