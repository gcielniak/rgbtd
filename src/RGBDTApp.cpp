#include <iostream>
#include <string>
#include <stdio.h>
#include <time.h>

#include "OpenNICamera.h"
#include "Camera.h"
#include "LBPFeature.h"
#include "LBPFeature2.h"
#include "CDTSensor.h"
#include "Classifier.h"

using namespace std;

// Get current date/time, format is  YYYYMMDDTHHmmss
const std::string currentDateTime() {
	time_t     now = time(0);
    struct tm  tstruct;
    char       buf[80];
    tstruct = *localtime(&now);
    strftime(buf, sizeof(buf), "%Y%m%dT%H%M%S", &tstruct);

    return buf;
}

void print_help()
{
	cerr << "CDTApp usage:" << endl;

	cerr << " -ip : input data path (use instead of the real sensor input)" << endl;
	cerr << " -td : thermal device" << endl;
	cerr << " -op : output data path [deafult: .\\data\\dataset_xxx]" << endl;
	cerr << " -si : save images" << endl;
	cerr << " -sf : save features (raw format)" << endl;
	cerr << "-sfx : save features (xml format)" << endl;
	cerr << " -pd : pyramid depth [deafult: -1, max]" << endl;
	cerr << " -sk : skip x out of xx frame when writing [deafult: 1 1]" << endl;
	cerr << " -vi : visualise images" << endl;
	cerr << " -vf : visualise features" << endl;
	cerr << " -fs : start frame" << endl;
	cerr << " -fe : end frame" << endl;
	cerr << " -co : color on" << endl;
	cerr << " -do : depth on" << endl;
	cerr << " -to : thermal on" << endl;
	cerr << " -vo : verbose off" << endl;
}

int main(int argc, char **argv)
{
	string input_data_path = "";
	string output_data_path = ".\\data\\dataset_" + currentDateTime() + "\\";
	bool save_images = false;
	bool save_features_raw = false;
	bool save_features_xml = false;
	bool show_images = false;
	bool show_feature_images = false;
	bool save_feature_image = false;
	bool use_color = false, use_depth = false, use_thermal = false;
	int pyramid_depth = -1;
	int thermal_device = -1;
	int start_frame = 0, end_frame = -1;
	bool verbose = true;

	LinLib::CDTFile image_writer, feature_writer;

	if (argc == 1)
	{
		print_help();
		exit(0);
	}

	for (int i = 1; i < argc; i++)
	{
		if ((strcmp(argv[i],"-ip")==0) && (i < (argc-1))) { input_data_path = argv[++i]; }
		else if ((strcmp(argv[i],"-op")==0) && (i < (argc-1))) { output_data_path = argv[++i]; }
		else if ((strcmp(argv[i],"-td")==0) && (i < (argc-1))) { thermal_device = atoi(argv[++i]); }
		else if ((strcmp(argv[i],"-pd")==0) && (i < (argc-1))) { pyramid_depth = atoi(argv[++i]); }
		else if ((strcmp(argv[i],"-fs")==0) && (i < (argc-1))) { start_frame = atoi(argv[++i]); }
		else if ((strcmp(argv[i],"-fe")==0) && (i < (argc-1))) { end_frame = atoi(argv[++i]); }
		else if (strcmp(argv[i],"-si")==0) { save_images = true; }
		else if (strcmp(argv[i],"-sf")==0) { save_features_raw = true; }
		else if (strcmp(argv[i],"-sfx")==0) { save_features_xml = true; }
		else if (strcmp(argv[i],"-sfi")==0) { save_feature_image = true; }
		else if (strcmp(argv[i],"-vi")==0) { show_images = true; }
		else if (strcmp(argv[i],"-vf")==0) { show_feature_images = true; }
		else if (strcmp(argv[i],"-co")==0) { use_color = true; }
		else if (strcmp(argv[i],"-do")==0) { use_depth = true; }
		else if (strcmp(argv[i],"-to")==0) { use_thermal = true; }
		else if (strcmp(argv[i],"-vo")==0) { verbose = false; }
		else if (strcmp(argv[i],"-h")==0)	{ print_help();	}
		else if ((strcmp(argv[i],"-sk")==0) && (i < (argc-2))) 
		{
			image_writer.SkipFrames(atoi(argv[++i]));
			feature_writer.SkipFrames(atoi(argv[i]));
			image_writer.SkipOutOfFrames(atoi(argv[++i]));
			feature_writer.SkipOutOfFrames(atoi(argv[i]));
		}
	}

	LinLib::CDTSensor *input_device;

	if (input_data_path.compare("")!=0) //path speficied use images instead of the sensor
	{
		input_device = new LinLib::CDTFile();
		((LinLib::CDTFile*)input_device)->Path(input_data_path + "\\images\\");
		((LinLib::CDTFile*)input_device)->StartFrame(start_frame);
		((LinLib::CDTFile*)input_device)->EndFrame(end_frame);
	}
	else // use sensor
	{
		try
		{
			input_device = new LinLib::CDTDevice();
			input_device->ThermalDevice(thermal_device);
		}
		catch (LinLib::Exception* exc)
		{ 
			cerr << exc->what() << endl;
			exit(1); 
		}
	}

	input_device->UseColor(use_color);
	input_device->UseDepth(use_depth);
	input_device->UseThermal(use_thermal);

	image_writer.Path(output_data_path + "\\images\\");
	feature_writer.Path(output_data_path);

	LinLib::LBPFeature feature;
	feature.BorderHandling(LinLib::BORDER_PADD);

	try { input_device->Init(); }
	catch (LinLib::Exception*) { cerr << "Could not initialise the input device." << endl; exit(1); }

	if (use_color)
		((LinLib::CDTDevice*)input_device)->SetVideoMode(openni::SENSOR_COLOR, 4); 
	if (use_depth) //use 100 um scale
		((LinLib::CDTDevice*)input_device)->SetVideoMode(openni::SENSOR_DEPTH, 5); 

	cv::Mat color_feature, depth_feature, thermal_feature, feature_matrix;

	int step = 0;

	while (cvWaitKey(1) != 27)
	{
		try 
		{ 
			input_device->GrabAllImages(); 
		}
		catch (LinLib::Exception*) 
		{
			if (!input_device->ColorFrame().data && !input_device->DepthFrame().data && !input_device->ThermalFrame().data)
				break; 
		}

		//test speed

		if (0)
		{
			int iterations = 1000;
			DWORD start = ::GetTickCount();

			for (int i = 0; i < iterations; i++)
				LinLib::LBPFeature2::Calculate(input_device->ColorFrame(), color_feature);
			cerr << "Feature calculated in " << double(::GetTickCount() - start)/iterations << " ms" << endl;
		}

		if (0)
		{
			int iterations = 1000;
			DWORD start = ::GetTickCount();

			for (int i = 0; i < iterations; i++)
				color_feature = feature.Get(input_device->ColorFrame(), pyramid_depth);
			cerr << "Feature calculated in " << double(::GetTickCount() - start)/iterations << " ms" << endl;
		}

		if (save_images)
			image_writer.SaveImages(input_device->ColorFrame(), input_device->DepthFrame(), input_device->ThermalFrame());

		if (save_features_raw || save_features_xml)
		{
			cv::Mat feature_vector;

			if (input_device->ColorFrame().data)
			{
				color_feature = feature.Get(input_device->ColorFrame(), pyramid_depth).clone();
				if (feature_vector.data)
					cv::hconcat(feature_vector, color_feature, feature_vector);
				else
					feature_vector = color_feature.clone();
			}
			if (input_device->DepthFrame().data)
			{
				depth_feature = feature.Get(input_device->DepthFrame(), pyramid_depth, 0).clone();
				if (feature_vector.data)
					cv::hconcat(feature_vector, depth_feature, feature_vector);
				else
					feature_vector = depth_feature.clone();
			}
			if (input_device->ThermalFrame().data)
			{
				thermal_feature = feature.Get(input_device->ThermalFrame(), pyramid_depth).clone();
				if (feature_vector.data)
					cv::hconcat(feature_vector, thermal_feature, feature_vector);
				else
					feature_vector = thermal_feature.clone();
			}

			//prepare a single feature matrix with concatenated features
			if (feature_matrix.data && feature_vector.data)
				cv::vconcat(feature_matrix, feature_vector, feature_matrix);
			else if (feature_vector.data)
				feature_matrix = feature_vector.clone();
		}

		if (show_images)
		{
			if (input_device->ColorFrame().data)
				cv::imshow("color", input_device->ColorFrame().clone());
			if (input_device->DepthFrame().data)
//				cv::imshow("depth", input_device->DepthFrame()*65535/10000);
				cv::imshow("depth", input_device->DepthFrame());
			if (input_device->ThermalFrame().data)
				cv::imshow("thermal", input_device->ThermalFrame().clone());
		}

		if (show_feature_images || save_feature_image)
		{
			cv::Mat color_feature_image, depth_feature_image, thermal_feature_image;

			if (input_device->ColorFrame().data)
			{
				color_feature_image = feature.GetFeatureImage(input_device->ColorFrame()).clone();
				if (show_feature_images)
					cv::imshow("color feature image", color_feature_image);
			}

			if (input_device->DepthFrame().data)
			{
				depth_feature_image = feature.GetFeatureImage(input_device->DepthFrame()).clone();
				if (show_feature_images)
					cv::imshow("depth feature image", depth_feature_image);
			}

			if (input_device->ThermalFrame().data)
			{
				thermal_feature_image = feature.GetFeatureImage(input_device->ThermalFrame()).clone();
				if (show_feature_images)
					cv::imshow("thermal feature image", thermal_feature_image);
			}

			if (save_feature_image)
				image_writer.SaveFeatureImages(color_feature_image, depth_feature_image, thermal_feature_image);
		}

		if (verbose)
			cerr << "Step " << step++ << " completed." << endl;
	}

	//save all feature data into a matrix
	if (feature_matrix.data)
	{
		//read ground truth data
		LinLib::GTReader gt_reader;
		cv::Mat gt_matrix;

		try { gt_matrix = gt_reader.ReadFile(input_data_path + "\\classes.txt"); }
		//if failed, assign -1 (unknown class) to all entries
		catch (LinLib::Exception*) { gt_matrix = cv::Mat(cv::Size(1,feature_matrix.rows),CV_32F,-1); }

		cv::hconcat(gt_matrix, feature_matrix, gt_matrix);

		if (verbose)
			cerr << "Writing data in xml format... ";
		
		feature_writer.SaveMatrix(gt_matrix, "features");

		if (verbose)
			cerr << " done." << endl;

		if (verbose)
			cerr << "Writing data in raw format... ";

		ofstream file;
		file.open(string(output_data_path + "\\features.txt").c_str());
        for (int j = 0; j < gt_matrix.rows; j++)
        {
            file << (int)gt_matrix.at<float>(0,0) << " ";
            for (int i = 1; i < gt_matrix.cols; i++)
                file << gt_matrix.at<float>(0,i) << " ";
            file << endl;
        }
		file.close();

		if (verbose)
			cerr << "done." << endl;
	}

	delete input_device;

	return 0;
}
