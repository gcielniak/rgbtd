# CDCapture
=====

## Installation
### Asus Xtion Pro on Windows 8 and USB3

This sensor seems to have problems on Windows 8 and USB3 ports - the problems might be present on other OSs but have not been yet confirmed. The working solution requires two steps:
- first, upgrade the sensor's firmware with a USB3 fix by following steps presented in this project: https://github.com/nh2/asus-xtion-fix. This has to be done on a Windows machine with working USB port!
- then change the default USBInterface value in the driver settings file `OpenNI2\Drivers\PS1080.ini`: `UsbInterface=2` (; is used as a comment line so needs to be removed).
