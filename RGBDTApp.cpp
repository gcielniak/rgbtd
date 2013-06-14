#include <iostream>
#include "OpenNICamera.h"
#include "Camera.h"
#include "LBPFeature.h"
#include "CDTSensor.h"

using namespace std;

int main()
{
	try
	{
		LinLib::CDTEmulator device;

		device.ThermalDelay(10);
		device.Init();
		device.Path("D:\\research\\projects\\RGBDT\\RGBDTApp\\images\\");

		while (cvWaitKey(1) != 27)
		{
			device.GrabAllImages();
			if (device.ColorFrame().data)
				cv::imshow("color", device.ColorFrame());
			if (device.DepthFrame().data)
				cv::imshow("depth", device.DepthFrame());
			if (device.ThermalFrame().data)
				cv::imshow("thermal", device.ThermalFrame());
		}
	}
	catch (LinLib::Exception *e)
	{
		cout << e->what() << endl;
	}

	return 0;
}
