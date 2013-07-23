#include <iostream>
#include <boost/date_time.hpp>

#include "OpenNICamera.h"
#include "Camera.h"
#include "LBPFeature.h"
#include "CDTSensor.h"
#include "Classifier.h"

using namespace std;
//using namespace boost::posix_time;

// Get current date/time, format is YYYYMMDDTHHmmss
const std::string currentDateTime()
{
	ostringstream ss;
	ss.imbue(std::locale(ss.getloc(), new boost::posix_time::time_facet("%Y%m%dT%H%M%S")));
	ss << boost::posix_time::second_clock::local_time();
	return ss.str();
}

void print_help()
{
	cerr << "CDTApp usage:" << endl;

	cerr << " -ip : input data path (use instead of the real sensor input)" << endl;
	cerr << " -td : thermal device" << endl;
	cerr << " -op : output data path [deafult: .\\data\\dataset_xxx]" << endl;
	cerr << " -si : save images" << endl;
	cerr << " -sf : save features" << endl;
	cerr << " -pd : pyramid depth [deafult: -1, max]" << endl;
	cerr << " -sk : skip x out of xx frame when writing [deafult: 1 1]" << endl;
	cerr << " -vi : visualise images" << endl;
	cerr << " -vf : visualise features" << endl;
	cerr << " -fs : start frame" << endl;
	cerr << " -fe : end frame" << endl;
	cerr << " -co : color off" << endl;
	cerr << " -do : depth off" << endl;
	cerr << " -to : thermal off" << endl;
	cerr << " -vo : verbose off" << endl;
	//additional line
}

int main(int argc, char **argv)
{
	if (1)
	{
	LinLib::Classifier classifier;

	classifier.TrainTest();

	return 0;
	}

	string input_data_path = "";
	string output_data_path = ".\\data\\dataset_" + currentDateTime() + "\\";
	bool save_images = false;
	bool save_features = false;
	bool show_images = false;
	bool show_feature_images = false;
	bool use_color = true, use_depth = true, use_thermal = true;
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
		else if (strcmp(argv[i],"-sf")==0) { save_features = true; }
		else if (strcmp(argv[i],"-vi")==0) { show_images = true; }
		else if (strcmp(argv[i],"-vf")==0) { show_feature_images = true; }
		else if (strcmp(argv[i],"-co")==0) { use_color = false; }
		else if (strcmp(argv[i],"-do")==0) { use_depth = false; }
		else if (strcmp(argv[i],"-to")==0) { use_thermal = false; }
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
		input_device = new LinLib::CDTDevice();
		input_device->ThermalDevice(thermal_device);
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

	cv::Mat color_feature, depth_feature, thermal_feature, feature_matrix;

	int step = 0;

	while (cvWaitKey(1) != 27)
	{
		try { input_device->GrabAllImages(); }
		catch (LinLib::Exception*) { break; }

		if (save_images)
			image_writer.SaveImages(input_device->ColorFrame(), input_device->DepthFrame(), input_device->ThermalFrame());

		if (save_features)
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

			//write in old format (a single file per instance per feature)
			//feature_writer.SaveFeatures(color_feature, depth_feature, thermal_feature);
		}

		if (show_images)
		{
			if (input_device->ColorFrame().data)
				cv::imshow("color", input_device->ColorFrame().clone());
			if (input_device->DepthFrame().data)
				cv::imshow("depth", input_device->DepthFrame()*65535/10000);
			if (input_device->ThermalFrame().data)
				cv::imshow("thermal", input_device->ThermalFrame().clone());
		}

		if (show_feature_images)
		{
			if (input_device->ColorFrame().data)
				cv::imshow("color feature image", feature.GetFeatureImage(input_device->ColorFrame()));
			if (input_device->DepthFrame().data)
				cv::imshow("depth feature image", feature.GetFeatureImage(input_device->DepthFrame()));
			if (input_device->ThermalFrame().data)
				cv::imshow("thermal feature image", feature.GetFeatureImage(input_device->ThermalFrame()));
		}

		if (verbose)
			cerr << "Step " << step++ << " completed." << endl;
	}

	//save all feature data into a matrix
	if (feature_matrix.data)
	{
		LinLib::GTReader gt_reader;
		cv::Mat gt_matrix = gt_reader.ReadFile(input_data_path + "\\classes.txt");

		cv::hconcat(gt_matrix, feature_matrix, gt_matrix);

		cerr << "Saving data matrix... ";
		feature_writer.SaveMatrix(gt_matrix, "dataset");
		cerr << " done." << endl;
	}

	delete input_device;

	return 0;
}
