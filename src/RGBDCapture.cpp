#include <iostream>
#include <iomanip>
#include <string>
#include <stdio.h>
#include <time.h>
#include <boost/filesystem.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include "OpenNICamera.h"

using namespace std;

const string currentDateTime();
void SaveImages(const cv::Mat&, const cv::Mat&, const cv::Mat&);

int recording_step = 0, recording_step_skip = 0, skip_out_of_frames = 1, skip_frames = 0;
string output_data_path = ".\\data\\dataset_" + currentDateTime() + "\\";

void print_help()
{
	cerr << "RGDBCapture usage:" << endl;

	cerr << " -ld : list devices and their modes" << endl;
	cerr << " -dn : device number" << endl;
	cerr << " -wi : write images" << endl;
	cerr << " -op : output data path [deafult: .\\data\\dataset_xxx]" << endl;
	cerr << " -sk : skip x out of xx frame when writing [deafult: 1 1]" << endl;
	cerr << " -co : color on" << endl;
	cerr << " -do : depth on" << endl;
	cerr << " -io : ir on" << endl;
	cerr << " -cm : color mode" << endl;
	cerr << " -dm : depth mdoe" << endl;
	cerr << " -im : ir mode" << endl;
	cerr << " -si : show images" << endl;
	cerr << " -vo : verbose off" << endl;
}

int main(int argc, char **argv)
{
	bool write_images = false;
	bool show_images = false;
	bool use_color = false, use_depth = false, use_ir = false;
	int color_mode = -1, depth_mode = -1, ir_mode = -1;
	bool verbose = true;
	int device_nr = 0;

	LinLib::OpenNICamera device;

	if (argc == 1)
	{
		print_help();
		exit(0);
	}

	for (int i = 1; i < argc; i++)
	{
		if ((strcmp(argv[i],"-op")==0) && (i < (argc-1))) { output_data_path = strcat(argv[++i], "\\images\\"); }
		else if (strcmp(argv[i],"-wi")==0) { write_images = true; }
		else if (strcmp(argv[i],"-si")==0) { show_images = true; }
		else if (strcmp(argv[i],"-co")==0) { use_color = true; }
		else if (strcmp(argv[i],"-do")==0) { use_depth = true; }
		else if (strcmp(argv[i],"-io")==0) { use_ir = true; }
		else if (strcmp(argv[i],"-vo")==0) { verbose = false; }
		else if (strcmp(argv[i],"-ld")==0) { device.ListDevices(); }
		else if ((strcmp(argv[i],"-dn")==0) && (i < (argc-1))) { device_nr = atoi(argv[++i]); }
		else if ((strcmp(argv[i],"-cm")==0) && (i < (argc-1))) { color_mode = atoi(argv[++i]); }
		else if ((strcmp(argv[i],"-dm")==0) && (i < (argc-1))) { depth_mode = atoi(argv[++i]); }
		else if ((strcmp(argv[i],"-im")==0) && (i < (argc-1))) { ir_mode = atoi(argv[++i]); }
		else if (strcmp(argv[i],"-h")==0)	{ print_help();	}
		else if ((strcmp(argv[i],"-sk")==0) && (i < (argc-2))) 
		{
			skip_frames = atoi(argv[++i]);
			skip_out_of_frames = atoi(argv[++i]);
		}
	}

	if (!use_color && !use_depth && !use_ir)
		return 0;

	try
	{
		device.Open(device_nr);

		if (use_color)
			device.ColorStream->Start();
		if (use_depth)
			device.DepthStream->Start();
		if (use_ir && !use_color)
			device.IrStream->Start();
	}
	catch (LinLib::Exception* exc)
	{ 
		cerr << exc->what() << endl;
		exit(1); 
	}

	if (device.ColorStream->Running() && (color_mode != -1))
		device.ColorStream->SetVideoMode(color_mode); 
	if (device.DepthStream->Running() && (depth_mode != -1))
		device.DepthStream->SetVideoMode(depth_mode); 
	if (device.IrStream->Running() && (ir_mode != -1))
		device.IrStream->SetVideoMode(ir_mode);

	int step = 0;
	cv::Mat color_frame, depth_frame, ir_frame;

	while (cvWaitKey(1) != 27)
	{
		try 
		{ 
			if (device.ColorStream->Running())
				color_frame = device.ColorStream->GetFrame();

			if (device.DepthStream->Running())
				depth_frame = device.DepthStream->GetFrame();

			if (device.IrStream->Running())
				ir_frame = device.IrStream->GetFrame();
		}
		catch (LinLib::Exception*) 
		{
			cerr << "Could not capture sensor data." << endl;
			break;
		}

		if (write_images)
			SaveImages(color_frame, depth_frame, ir_frame);

		if (show_images)
		{
			if (color_frame.data)
				cv::imshow("color", color_frame);

			if (depth_frame.data)
			{
				double depth_scale = 1.0;
				if (device.DepthStream->GetPixelFormat() == openni::PIXEL_FORMAT_DEPTH_1_MM)
					depth_scale = 65535/10000;
				cv::imshow("depth", depth_frame*depth_scale);
			}

			if (ir_frame.data)
			{
				double depth_scale = 1.0;
				if (device.IrStream->GetPixelFormat() == openni::PIXEL_FORMAT_GRAY16)
					depth_scale = 65535/1022;
				cv::imshow("ir", ir_frame*depth_scale);
			}
		}

		if (verbose)
			cerr << "Step " << step++ << " completed." << endl;
	}

	return 0;
}

void SaveImages(const cv::Mat& color_image, const cv::Mat& depth_image, const cv::Mat& ir_image)
{
	if (!boost::filesystem::exists(boost::filesystem::path(output_data_path)))
		boost::filesystem::create_directories(boost::filesystem::path(output_data_path));

	int skip_steps = (recording_step % skip_out_of_frames);
	if (skip_steps >= skip_frames)
	{
		std::stringstream s;
		s << std::setw(7) << std::setfill('0') << recording_step_skip;
		if (color_image.data)
			cv::imwrite(output_data_path + "color" + s.str() + ".png", color_image);
		if (depth_image.data)
			cv::imwrite(output_data_path + "depth" + s.str() + ".png", depth_image);
		if (ir_image.data)
			cv::imwrite(output_data_path + "ir" + s.str() + ".png", depth_image);
		recording_step_skip++;
	}
	recording_step++;
}

// Get current date/time, format is YYYYMMDDTHHmmss
const std::string currentDateTime()
{
	ostringstream ss;
	ss.imbue(std::locale(ss.getloc(), new boost::posix_time::time_facet("%Y%m%dT%H%M%S")));
	ss << boost::posix_time::second_clock::local_time();
	return ss.str();
}

