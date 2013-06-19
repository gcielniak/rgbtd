#pragma once

#include <string>
#include <opencv2/opencv.hpp>
#include "OpenNI.h"
#include "Exception.h"

namespace LinLib
{
	using namespace std;
	using namespace openni;

	class Stream
	{
		const Device *device;
		VideoStream stream;
		VideoFrameRef frame_ref;
		cv::Mat frame_cv;
		bool running;
	
	public:
		Stream(const openni::Device *_device, SensorType type)
		{
			running = false;
			device = _device;
			if (stream.create(*device, type) != STATUS_OK)
				throw new Exception("OpenNICamera::Init: " + string(OpenNI::getExtendedError()));
		}

		~Stream()
		{
			Stop();
			stream.destroy();
		}

		void Start()
		{
			running = true;
			if (stream.start() != STATUS_OK)
				throw new Exception("OpenNICamera::Init: " + string(OpenNI::getExtendedError()));
		}

		void Stop()
		{
			if (running)
				stream.stop();
		}

		cv::Mat GetFrame()
		{
			cv::Mat yuv_frame;

			stream.readFrame(&frame_ref);

			switch (stream.getVideoMode().getPixelFormat())
			{
				case PIXEL_FORMAT_YUV422:
					frame_cv = cv::Mat(frame_ref.getHeight(), frame_ref.getWidth(), CV_8UC3);
					yuv_frame = cv::Mat(frame_ref.getHeight(), frame_ref.getWidth(), CV_8UC2, (char*)frame_ref.getData());
					cv::cvtColor(yuv_frame, frame_cv, CV_YUV2BGR_Y422);
					break;
				case PIXEL_FORMAT_GRAY16:
					frame_cv = cv::Mat(frame_ref.getHeight(), frame_ref.getWidth(), CV_16UC1, (char*)frame_ref.getData());
					break;
				case PIXEL_FORMAT_GRAY8:
					frame_cv = cv::Mat(frame_ref.getHeight(), frame_ref.getWidth(), CV_8UC1, (char*)frame_ref.getData());
					break;
				case PIXEL_FORMAT_RGB888:
					frame_cv = cv::Mat(frame_ref.getHeight(), frame_ref.getWidth(), CV_8UC3, (char*)frame_ref.getData());
					cv::cvtColor(frame_cv, frame_cv, CV_BGR2RGB);
					break;
				case PIXEL_FORMAT_DEPTH_1_MM:
					frame_cv = cv::Mat(frame_ref.getHeight(), frame_ref.getWidth(), CV_16UC1, (char*)frame_ref.getData());
					break;
				default:
					throw new Exception("Stream::GetFrame: unsupported pixel format.");
			}

			cv::flip(frame_cv, frame_cv, 1);

			return frame_cv;
		}
	};

	class OpenNICamera
	{
		Device device;
		VideoStream color_stream, depth_stream, ir_stream;
		Status result;
		static int number_of_devices;

		static void Init()
		{
			if (!number_of_devices) //do it only once
			{
				if (OpenNI::initialize() != STATUS_OK)
					throw new Exception("OpenNICamera::Init: " + string(OpenNI::getExtendedError()));
				number_of_devices++;
			}
		}
	
	public:
		///Constructor
		OpenNICamera()
		{
			Init();
		}

		OpenNICamera(int device_nr)
		{
			Init();
			Open(device_nr);
		}

		~OpenNICamera()
		{
			Close();

			if ((--number_of_devices) == 0) //do it only once
				OpenNI::shutdown();
		}

		void Close()
		{
			delete ColorStream;
			delete DepthStream;
			delete IrStream;
			device.close();
		}

		void Open(int device_nr=-1)
		{
			Array<DeviceInfo> device_list;
			OpenNI::enumerateDevices(&device_list);

			if (device_nr >= device_list.getSize())
				throw new Exception("OpenNICamera::Init: invalid device specified.");

			const char *device_uri = ANY_DEVICE;

			if (device_nr!=-1)
				device_uri = device_list[device_nr].getUri();

			if (device.open(device_uri) != STATUS_OK)
				throw new Exception("OpenNICamera::Init: " + string(OpenNI::getExtendedError()));

			ColorStream = new Stream(&device, SENSOR_COLOR);
			DepthStream = new Stream(&device, SENSOR_DEPTH);
			IrStream = new Stream(&device, SENSOR_IR);
		}

		static int NumberOfDevices()
		{
			Array<DeviceInfo> device_list;
			OpenNI::enumerateDevices(&device_list);
			return device_list.getSize();
		}

		Stream *ColorStream;
		Stream *DepthStream;
		Stream *IrStream;
	};

	int OpenNICamera::number_of_devices = 0;
}
