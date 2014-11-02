#include <iostream>
#include <iomanip>
#include <string>
#include <stdio.h>
#include <time.h>
#include <boost/filesystem.hpp>

#include "OpenNICamera.h"

using namespace std;


const string currentDateTime();
void print_help();
void SaveImages(const cv::Mat& color_image, const cv::Mat& depth_image);

int recording_step = 0, recording_step_skip = 0, skip_out_of_frames = 1, skip_frames = 0;
string output_data_path = ".\\data\\dataset_" + currentDateTime() + "\\";

int main(int argc, char **argv)
{
	bool save_images = false;
	bool show_images = false;
	bool use_color = false, use_depth = false, use_ir = false;
	bool verbose = true;

	if (argc == 1)
	{
		print_help();
		exit(0);
	}

	for (int i = 1; i < argc; i++)
	{
		if ((strcmp(argv[i],"-op")==0) && (i < (argc-1))) { output_data_path = strcat(argv[++i], "\\images\\"); }
		else if (strcmp(argv[i],"-si")==0) { save_images = true; }
		else if (strcmp(argv[i],"-vi")==0) { show_images = true; }
		else if (strcmp(argv[i],"-co")==0) { use_color = true; }
		else if (strcmp(argv[i],"-do")==0) { use_depth = true; }
		else if (strcmp(argv[i],"-to")==0) { use_ir = true; }
		else if (strcmp(argv[i],"-vo")==0) { verbose = false; }
		else if (strcmp(argv[i],"-h")==0)	{ print_help();	}
		else if ((strcmp(argv[i],"-sk")==0) && (i < (argc-2))) 
		{
			skip_frames = atoi(argv[++i]);
			skip_out_of_frames = atoi(argv[++i]);
		}
	}

	LinLib::OpenNICamera kinect_camera;

	try
	{
		kinect_camera.Open();
		if (use_color)
			kinect_camera.ColorStream->Start();
		if (use_depth)
			kinect_camera.DepthStream->Start();	
	}
	catch (LinLib::Exception* exc)
	{ 
		cerr << exc->what() << endl;
		exit(1); 
	}

	if (use_color)
		kinect_camera.SetVideoMode(openni::SENSOR_COLOR, 4); 
	if (use_depth) //use 100 um scale
		kinect_camera.SetVideoMode(openni::SENSOR_DEPTH, 5); 

	int step = 0;
	cv::Mat color_frame, depth_frame;

	while (cvWaitKey(1) != 27)
	{
		try 
		{ 
			if (use_color)
				color_frame = kinect_camera.ColorStream->GetFrame();

			if (use_depth)
				depth_frame = kinect_camera.DepthStream->GetFrame();
		}
		catch (LinLib::Exception*) 
		{
			cerr << "Could not capture sensor data." << endl;
			break;
		}

		if (save_images)
			SaveImages(color_frame, depth_frame);

		if (show_images)
		{
			if (color_frame.data)
				cv::imshow("color", color_frame);
			if (depth_frame.data)
				cv::imshow("depth", depth_frame);
		}

		if (verbose)
			cerr << "Step " << step++ << " completed." << endl;
	}

	return 0;
}

void SaveImages(const cv::Mat& color_image, const cv::Mat& depth_image)
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
		recording_step_skip++;
	}
	recording_step++;
}

void print_help()
{
	cerr << "RGDBCapture usage:" << endl;

	cerr << " -op : output data path [deafult: .\\data\\dataset_xxx]" << endl;
	cerr << " -si : save images" << endl;
	cerr << " -sk : skip x out of xx frame when writing [deafult: 1 1]" << endl;
	cerr << " -vi : visualise images" << endl;
	cerr << " -co : color on" << endl;
	cerr << " -do : depth on" << endl;
	cerr << " -vo : verbose off" << endl;
}

// Get current date/time, format is  YYYYMMDDTHHmmss
const std::string currentDateTime() {
	time_t     now = time(0);
	struct tm  tstruct;
	char       buf[80];
	tstruct = *localtime(&now);
	strftime(buf, sizeof(buf), "%Y%m%dT%H%M%S", &tstruct);

	return buf;
}

