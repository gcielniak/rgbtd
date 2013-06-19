#pragma once

#include <opencv2/opencv.hpp>
#if WIN32
#include "videoInput.h"
#endif

namespace LinLib
{
	using namespace std;

	class Camera
	{
		CvCapture *capture;

	protected:
		int device_id;
		cv::Mat frame_cv, frame_cv_gray;
		bool convert_to_grayscale;
	
	public:
		///Constructor
		Camera()
			: convert_to_grayscale(false)
		{
			device_id = -1;
			capture = 0;
		}

		Camera(int device_nr)
			: convert_to_grayscale(false)
		{
			Open(device_nr);
		}

		~Camera()
		{
			Close();
		}

		virtual void Open(int device_nr=-1)
		{
			if (device_nr == -1)
				device_nr = 0;

			capture = cvCreateCameraCapture(device_nr);
			if (!capture)
				throw new Exception("Camera::Open: could not initialise the specified device.");

			if (!cvSetCaptureProperty(capture, CV_CAP_PROP_FRAME_WIDTH, 640))
			{
				Close();
				throw new Exception("WinCamera::Open: could not set the specified format.");
			}

			if (!cvSetCaptureProperty(capture, CV_CAP_PROP_FRAME_HEIGHT, 480))
			{
				Close();
				throw new Exception("WinCamera::Open: could not set the specified format.");
			}

			device_id = device_nr;
		}

		virtual void Close()
		{
			if (capture)
				cvReleaseCapture(&capture);
			device_id = -1;
		}

		virtual cv::Mat GetFrame()
		{
			frame_cv = cvQueryFrame(capture);

			if (convert_to_grayscale)
			{
				if ((frame_cv_gray.rows != frame_cv.rows) || (frame_cv_gray.cols != frame_cv.cols))
					frame_cv_gray = cv::Mat(frame_cv.rows, frame_cv.cols, CV_8UC1);
				cv::cvtColor(frame_cv, frame_cv_gray, CV_RGB2GRAY);
				return frame_cv_gray;
			}
			else
				return frame_cv;
		}

		void ConvertToGrayscale(bool value)
		{
			convert_to_grayscale = value;
		}

		bool ConvertToGrayscale()
		{
			return convert_to_grayscale;
		}
	};

#if WIN32
	class WinCamera : public Camera
	{
		videoInput VI;

	public:
		///Constructor
		WinCamera()
		{
			VI.setVerbose(false);
			device_id = -1;
			convert_to_grayscale = false;
		}

		WinCamera(int device_nr)
		{
			VI.setVerbose(false);
			Open(device_nr);
			convert_to_grayscale = false;
		}

		virtual void Open(int device_nr=-1) override
		{
			if (device_nr == -1)
				device_nr = 0;

			if (device_nr >= VI.listDevices(true))
				throw new Exception("WinCamera::Open: invalid device specified.");

			if (!VI.setupDevice(device_nr, 640, 480, VI_COMPOSITE))
				throw new Exception("WinCamera::Open: could not initialise the specified device.");

			if (!VI.setFormat(device_nr, VI_NTSC_M))
				throw new Exception("WinCamera::Open: could not set the specified format.");

			device_id = device_nr;
		}

		virtual void Close() override
		{
			VI.stopDevice(device_id);
			device_id = -1;
		}

		virtual cv::Mat GetFrame() override
		{
			//blocking querry
			while (!VI.isFrameNew(device_id)) ;

			if ((frame_cv.cols != VI.getWidth(device_id)) || (frame_cv.rows != VI.getHeight(device_id)) || (frame_cv.type() != CV_8UC3))
				frame_cv = cv::Mat(VI.getHeight(device_id), VI.getWidth(device_id), CV_8UC3);
				
			if (VI.isFrameNew(device_id))
				VI.getPixels(device_id, frame_cv.data, false, true);

			if (convert_to_grayscale)
			{
				if ((frame_cv_gray.rows != frame_cv.rows) || (frame_cv_gray.cols != frame_cv.cols))
					frame_cv_gray = cv::Mat(frame_cv.rows, frame_cv.cols, CV_8UC1);
				cv::cvtColor(frame_cv, frame_cv_gray, CV_RGB2GRAY);
				return frame_cv_gray;
			}
			else
				return frame_cv;
		}
	};
#endif
}
