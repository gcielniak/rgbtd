# CDCapture

This software captures raw image data from depth sensors such as Kinect or ASUS Xtion. To build the software the following components are required:
 - C++ compiler;
 - CMake;
 - OpenNI2 SDK;
 - OpenCV library;
 - Boost library.

The following setups were tested:
 - OS X 10.9.4, XCode 5.1.1, CMake 3.0.2, OpenNI2 2.2.0.33, OpenCV 2.4.9, Boost 1.56.0.

## Installation
### Mac OS X
 - install XCode (should include CMake, I think);
 - install Homebrew which should simplify installation of other libraries: `ruby -e "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/master/install)"`; then `brew tap homebrew/science`;
 - install OpenNI2: `brew install openni2`; add the following environment variables to your `~/.bash_profile`:
   - `export OPENNI2_INCLUDE=/usr/local/include/ni2`
   - `export OPENNI2_REDIST=/usr/local/lib/ni2`
   - `export OPENNI2_LIB=/usr/local/lib/ni2`
 - install OpenCV: `brew install opencv`;
 - install Boost: `brew install boost`.
 
To compile:
 - `mkdir build`,`cd build`,`cmake .. -G "Unix Makefiles"`,`make`. 

### Asus Xtion Pro on Windows 8 and USB3

This sensor seems to have problems on Windows 8 and USB3 ports - the problems might be present on other OSs but have not been yet confirmed. The working solution requires two steps:
- first, upgrade the sensor's firmware with a USB3 fix by following steps presented in this project: https://github.com/nh2/asus-xtion-fix. This has to be done on a Windows machine with working USB port!
- then change the default USBInterface value in the driver settings file `OpenNI2\Drivers\PS1080.ini`: `UsbInterface=2` (; is used as a comment line so needs to be removed).
