#pragma once

namespace LinLib
{
	class CDTSensor
	{
	protected:
		cv::Mat color_frame, depth_frame, thermal_frame;
		bool use_color, use_depth, use_thermal;
		std::list<cv::Mat> color_buffer, depth_buffer, thermal_buffer;
		unsigned int color_delay, depth_delay, thermal_delay;

		void BufferFrames()
		{
			if (use_color)
			{
				if (color_delay)
				{
					if (color_buffer.size() >= color_delay)
					{
						color_buffer.front().release();
						color_buffer.pop_front();
					}
					color_buffer.push_back(color_frame.clone());
					color_frame = color_buffer.front();
				}
			}

			if (use_depth)
			{
				if (depth_delay)
				{
					if (depth_buffer.size() >= depth_delay)
					{
						depth_buffer.front().release();
						depth_buffer.pop_front();
					}
					depth_buffer.push_back(depth_frame.clone());
					depth_frame = depth_buffer.front();
				}
			}

			if (use_thermal)
			{
				if (thermal_delay)
				{
					if (thermal_buffer.size() >= thermal_delay)
					{
						thermal_buffer.front().release();
						thermal_buffer.pop_front();
					}
					thermal_buffer.push_back(thermal_frame.clone());
					thermal_frame = thermal_buffer.front();
				}
			}
		}

	public:
		CDTSensor()
			: use_color(true), use_depth(true), use_thermal(true),
		color_delay(0), depth_delay(0), thermal_delay(0) {}

		virtual void Init() {}

		void UseColor(bool value) { use_color = value; }
		bool UseColor() { return use_color; }
		void UseDepth(bool value) { use_depth = value; }
		bool UseDepth() { return use_depth; }
		void UseThermal(bool value) { use_thermal = value; }
		bool UseThermal() { return use_thermal; }

		void ColorDelay(unsigned int value) { color_delay = value; }
		unsigned int ColorDelay() { return color_delay; }
		void DepthDelay(unsigned int value) { depth_delay = value; }
		unsigned int DepthDelay() { return depth_delay; }
		void ThermalDelay(unsigned int value) { thermal_delay = value; }
		unsigned int ThermalDelay() { return thermal_delay; }

		virtual void GrabAllImages() = 0;
		cv::Mat &ColorFrame() { return color_frame; }
		cv::Mat &DepthFrame() { return depth_frame; }
		cv::Mat &ThermalFrame() { return thermal_frame; }
	};

	class CDTDevice : public CDTSensor
	{
		LinLib::OpenNICamera kinect_camera;
		#if WIN32
			LinLib::WinCamera thermal_camera;
		#else
			LinLib::Camera thermal_camera;
		#endif

	public:
		virtual void Init()
		{
			if (use_color || use_depth)
			{
				kinect_camera.Open();
				if (use_color)
					kinect_camera.ColorStream->Start();
				if (use_depth)
					kinect_camera.DepthStream->Start();
			}

			if (use_thermal)
				thermal_camera.Open();
		}

		virtual void GrabAllImages()
		{
			if (use_color)
				color_frame = kinect_camera.ColorStream->GetFrame();

			if (use_depth)
				depth_frame = kinect_camera.DepthStream->GetFrame();

			if (use_thermal)
				thermal_frame = thermal_camera.GetFrame();

			BufferFrames();
		}
	};

	class CDTEmulator : public CDTSensor
	{
		string path;
		int reading_step, recording_step, recording_step_skip;
		int skip_frames, skip_out_of_frames;

	public:
		CDTEmulator() : CDTSensor()
		{
			reading_step = 0;
			recording_step = 0;
			recording_step_skip = 0;
			skip_frames = 1;
			skip_out_of_frames = 1;
			path = "";
		}

		void SkipFrames(int value) { skip_frames = value; }
		int SkipFrames() { return skip_frames; }
		void SkipOutOfFrames(int value) { skip_out_of_frames = value; }
		int SkipOutOfFrames() { return skip_out_of_frames; }

		void Path(string value)
		{
			path = value;
		}

		string Path()
		{
			return path;
		}

		virtual void GrabAllImages()
		{
			char file_name[256];

			if (use_color)
			{
				sprintf_s(file_name, "%scolor%05d.png", path.c_str(), reading_step);
				color_frame = cv::imread(file_name);
			}
			if (use_depth)
			{
				sprintf_s(file_name, "%sdepth%05d.png", path.c_str(), reading_step);
				depth_frame = cv::imread(file_name);
			}
			if (use_thermal)
			{
				sprintf_s(file_name, "%sthermal%05d.png", path.c_str(), reading_step);
				thermal_frame = cv::imread(file_name);
			}

			BufferFrames();

			reading_step++;
		}

		void SaveImages(const cv::Mat& color_image, const cv::Mat& depth_image, const cv::Mat& thermal_image)
		{
			char file_name[256];

			int skip_steps = (recording_step % skip_out_of_frames);
			if (skip_frames >= skip_steps)
			{
				sprintf_s(file_name, "%scolor%05d.png", path.c_str(), recording_step_skip);
				cv::imwrite(file_name, color_image);
				sprintf_s(file_name, "%sdepth%05d.png", path.c_str(), recording_step_skip);
				cv::imwrite(file_name, depth_image);
				sprintf_s(file_name, "%sthermal%05d.png", path.c_str(), recording_step_skip);
				cv::imwrite(file_name, thermal_image);
				recording_step_skip++;
			}
			recording_step++;
		}
	};
}