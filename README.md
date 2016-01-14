#About
Arduino based remote for VESC with nRF24

Library https://github.com/RollingGecko/VescUartControl required

To the VESC is a message send with joystick information and buttons pressed. VESC handels the message like a message needed for the nunchuk app. For deltails to the nunchuk app on the vesc please refer to:

http://vedder.se/2015/01/vesc-open-source-esc/

The firmware of the VESC must be adapted, so that the UART message can be handled. Pull request is following soon. So far use please my fork:

https://github.com/RollingGecko/bldc.git

#Specification
* Remote to VESC over nRF24 Radio
* Reading of X/Y joystick positions and 2 buttons and transmit to vesc
* Vesc is connected over UART to remote Ardunio nano
* All measured values from VESC like input voltage, current etc. is send to remote
* TX is master and RX slave: Sending direction  nRF24 from TX to RX. Feedback only over acknowledgement message

#Feature List
* 

#ToDo
* Remaining Capacity of battery on VESC site not yet implemented
* Vibration alarm at defined rest capacity state (in defined time intervalls)
* Displaying traveled km over 4 RGB LED 

#Wish list
Coming soon.

#Implementation Example 


Based on a Nunchuk and a Arduino nano. Beside that 4 2812 LED for state indication and alert vibrator are implemented. Joystick and buttons are from the original nunchuck.

![](https://raw.githubusercontent.com/RollingGecko/ArduBoardControler/gh-pages/images/Pictures/ReadyTX.jpg)

![](https://raw.githubusercontent.com/RollingGecko/ArduBoardControler/gh-pages/images/Pictures/TXinside1.jpg)

![](https://raw.githubusercontent.com/RollingGecko/ArduBoardControler/gh-pages/images/Pictures/TXinside.jpg)
