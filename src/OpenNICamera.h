#pragma once

#include <string>
#include <opencv2/opencv.hpp>
#include "OpenNI.h"
#include "Exception.h"

namespace LinLib
{
	using namespace std;
	using namespace openni;

	/// OpenNI Stream class providing color, depth and ir data
	class Stream
	{
		Device *device;
		VideoStream stream;
		VideoFrameRef frame_ref;
		cv::Mat frame_cv;
		bool running;

	public:
		/// Constructor
		Stream(openni::Device *_device, SensorType type)
		{
			running = false;
			device = _device;
			if (stream.create(*device, type) != STATUS_OK)
				throw new Exception("OpenNICamera::Init: " + string(OpenNI::getExtendedError()));
		}

		/// Destructor
		~Stream()
		{
			Stop();
			stream.destroy();
		}

		/// Start streaming
		void Start()
		{
			if (stream.start() != STATUS_OK)
				throw new Exception("OpenNICamera::Init: " + string(OpenNI::getExtendedError()));
			running = true;
		}

		/// Stop streaming
		void Stop()
		{
			if (running)
				stream.stop();
		}

		bool Running()
		{
			return running;
		}

		/// Get image in OpenCV format

		cv::Mat GetFrame()
		{
			cv::Mat yuv_frame;

			stream.readFrame(&frame_ref);

			// perform format conversion
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
			case PIXEL_FORMAT_DEPTH_100_UM:
				frame_cv = cv::Mat(frame_ref.getHeight(), frame_ref.getWidth(), CV_16UC1, (char*)frame_ref.getData());
				break;
			default:
				throw new Exception("Stream::GetFrame: unsupported pixel format.");
			}

			cv::flip(frame_cv, frame_cv, 1);

			return frame_cv;
		}

		void SetVideoMode(int mode)
		{
			const Array<VideoMode>& modes = stream.getSensorInfo().getSupportedVideoModes();

			if (mode < modes.getSize())
			{
				stream.stop();
				stream.setVideoMode(modes[mode]);
				stream.start();
			}
		}

		PixelFormat GetPixelFormat()
		{			
			return stream.getVideoMode().getPixelFormat();
		}
	};


	/// Range camera class using OpenNI interface
	class OpenNICamera
	{
		Device device;
		VideoStream color_stream, depth_stream, ir_stream;
		Status result;
		static int number_of_devices;

		/// Framework initialisation (executed only once)
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
		/// Default constructor
		OpenNICamera()
			: ColorStream(0), DepthStream(0), IrStream(0)
		{
			Init();
		}

		/// Constructor
		OpenNICamera(int device_nr)
			: ColorStream(0), DepthStream(0), IrStream(0)
		{
			Init();
			Open(device_nr);
		}

		/// Destructor
		~OpenNICamera()
		{
			Close();

			if ((--number_of_devices) == 0) //do it only once
				OpenNI::shutdown();
		}

		/// Close the streams and device
		void Close()
		{
			if (ColorStream)
			{
				delete ColorStream;
				ColorStream = 0;
			}

			if (DepthStream)
			{
				delete DepthStream;
				DepthStream = 0;
			}

			if (IrStream)
			{
				delete IrStream;
				IrStream = 0;
			}

			device.close();
		}

		/// Open a specified device
		void Open(int device_nr=-1)
		{
			Array<DeviceInfo> device_list;
			OpenNI::enumerateDevices(&device_list);

			if (device_nr >= device_list.getSize())
				throw new Exception("OpenNICamera::Init: invalid device specified.");

			const char *device_uri = ANY_DEVICE;

			if (device_nr != -1)
				device_uri = device_list[device_nr].getUri();

			if (device.open(device_uri) != STATUS_OK)
				throw new Exception("OpenNICamera::Init: " + string(OpenNI::getExtendedError()));

			ColorStream = new Stream(&device, SENSOR_COLOR);
			DepthStream = new Stream(&device, SENSOR_DEPTH);
			IrStream = new Stream(&device, SENSOR_IR);
		}

		void ListDevices()
		{
			Array<DeviceInfo> device_list;
			OpenNI::enumerateDevices(&device_list);

			if (!device_list.getSize())
				return;

			cerr << "Available devices: " << endl;
			for (int i = 0; i < device_list.getSize(); i++)
			{
				cerr << "[" << i << "] " << device_list[i].getName() << ", " << device_list[i].getVendor() << endl;

				Open(i);

				cerr << " color sensor modes" << endl;
				SensorInfoString(device.getSensorInfo(SENSOR_COLOR));
				cerr << " depth sensor modes" << endl;
				SensorInfoString(device.getSensorInfo(SENSOR_DEPTH));
				cerr << " ir sensor modes" << endl;
				SensorInfoString(device.getSensorInfo(SENSOR_IR));

				Close();
			}
		}

		static string PixelFormatString(PixelFormat format)
		{
			switch (format)
			{
			case PIXEL_FORMAT_DEPTH_1_MM: return "DEPTH_1_MM";
			case PIXEL_FORMAT_DEPTH_100_UM: return "DEPTH_100_UM";
			case PIXEL_FORMAT_SHIFT_9_2: return "SHIFT_9_2";
			case PIXEL_FORMAT_SHIFT_9_3: return "SHIFT_9_3";
			case PIXEL_FORMAT_RGB888: return "RGB888";
			case PIXEL_FORMAT_YUV422: return "YUV422";
			case PIXEL_FORMAT_GRAY8: return "GRAY8";
			case PIXEL_FORMAT_GRAY16: return "GRAY16";
			case PIXEL_FORMAT_JPEG: return "JPEG";
			case PIXEL_FORMAT_YUYV: return "YUYV";
			default: return "NOT SUPPORTED";
			}
		}

		static void SensorInfoString(const SensorInfo* info)
		{
			const Array<VideoMode>& modes = info->getSupportedVideoModes();
			for (int i = 0; i < modes.getSize(); i++)
			{
				cerr << "  [" << i << "] ";
				cerr << modes[i].getFps() << " FPS, ";
				cerr << PixelFormatString(modes[i].getPixelFormat()) << ", ";
				cerr << modes[i].getResolutionX() << "x" << modes[i].getResolutionY() << endl;
			}
		}

		Stream *ColorStream;
		Stream *DepthStream;
		Stream *IrStream;
	};

	int OpenNICamera::number_of_devices = 0;
}
