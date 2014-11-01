#define NOMINMAX
#include <algorithm>
#include <iostream>
#include <string>
#include <stdio.h>
#include <time.h>

#include <opencv2/opencv.hpp>
#include <opencv2/contrib/contrib.hpp>
#include "OpenNICamera.h"

using namespace std;

void print_help()
{
	cerr << "PersonDetectorApp usage:" << endl;

	cerr << " -ip : input data path (use instead of the real sensor input)" << endl;
	cerr << " -op : output data path [deafult: .\\data\\dataset_xxx]" << endl;
	cerr << " -si : save images" << endl;
	cerr << " -vi : visualise images" << endl;
	cerr << " -vo : verbose off" << endl;
}

void ConnectedComponents(const cv::Mat& image, cv::Mat& labels, int range=0, int min_region=0);

class WeightedQuickUnionUF 
{
private:
	std::vector<int> id;
	std::vector<int> sz;

public:
	void insert()
	{
		id.push_back(id.size());
		sz.push_back(sz.size());
	}

	int find(int p) 
	{
		while (p != id[p])
		{
			id[p] = id[id[p]];//path compression
			p = id[p];
		}
		return p;
	}

	bool connected(int p, int q) 
	{
		return find(p) == find(q);
	}


	void merge(int p, int q) 
	{
		int rootP = find(p);
		int rootQ = find(q);
		if (rootP == rootQ) 
			return;

		// make smaller root point to larger one
		if (sz[rootP] < sz[rootQ])
		{ 
			id[rootP] = rootQ;
			sz[rootQ] += sz[rootP]; 
		}
		else
		{
			id[rootQ] = rootP;
			sz[rootP] += sz[rootQ];
		}
	}

	unsigned int length()
	{
		return id.size();
	}
};

int main(int argc, char **argv)
{
	string input_data_path = "";
	bool save_images = false;
	bool show_images = true;
	bool verbose = true;
	int step = 0;

	for (int i = 1; i < argc; i++)
	{
		if ((strcmp(argv[i],"-ip")==0) && (i < (argc-1))) { input_data_path = argv[++i]; }
		else if (strcmp(argv[i],"-si")==0) { save_images = true; }
		else if (strcmp(argv[i],"-vi")==0) { show_images = true; }
		else if (strcmp(argv[i],"-vo")==0) { verbose = false; }
		else if (strcmp(argv[i],"-h")==0)	{ print_help();	}
	}

	LinLib::OpenNICamera* camera;

	try
	{
		camera = new LinLib::OpenNICamera(0);
		camera->DepthStream->Start();
	}
	catch (LinLib::Exception*) 
	{
		cerr << "Could not initialise the depth camera." << endl;
		return 0;
	}

	cv::Mat depth_image;
	cv::Mat labels;

	while (cvWaitKey(1) != 27)
	{
		try 
		{
			depth_image = camera->DepthStream->GetFrame();
		}
		catch (LinLib::Exception*) 
		{
			break; 
		}

		int64 start = cv::getTickCount();

		for (int i = 0; i < 100; i++)
			ConnectedComponents(depth_image, labels, 50, 1000);


		cerr << "Processing time: " << (double)(cv::getTickCount()-start)*10/(cv::getTickFrequency()) << endl;

		if (show_images)
		{
			cv::imshow("depth", depth_image*65535/10000);

			double min_v, max_v;
			cv::minMaxLoc(labels,&min_v, &max_v);
			cv::Mat labels_rainbow;
			cv::Mat labels8;
			labels.convertTo(labels8, CV_8U, 8);
			cv::applyColorMap(labels8, labels_rainbow, cv::COLORMAP_JET);
			cv::imshow("labels", labels_rainbow);
		}

		//if (verbose)
		//			cerr << "Step " << step++ << " completed." << endl;
	}

	return 0;
}

void ConnectedComponents(const cv::Mat& image, cv::Mat& labels, int range, int min_region)
{
	WeightedQuickUnionUF links;
	links.insert();
	std::vector<int> label_size(1,0);

	labels = cv::Mat::zeros(image.rows, image.cols, CV_32S);

	int next_label = 1;

	for(int i = 0; i < image.rows; i++)
	{
		const ushort *image_pp = image.ptr<ushort>(max(0,i-1));
		const ushort *image_p = image.ptr<ushort>(i);
		int *labels_pp = labels.ptr<int>(max(0,i-1));
		int *labels_p = labels.ptr<int>(i);

		for(int j = 0; j < image.cols; j++)
		{
			//ignore background pixels
			if (!image_p[j])
				continue;

			//find the neighbours (4-way connectivity)
			bool n1 = false;
			bool n2 = false;
			if (j)
				n1 = abs(image_p[j]-image_p[j-1]) <= range;
			if (i)
				n2 = abs(image_p[j]-image_pp[j]) <= range;

			//both neighbours empty
			if (!n1 && !n2)
			{
				label_size.push_back(1);
				labels_p[j] = next_label++;
				links.insert();
			}
			else
			{
				//get labels
				int n1_label = 0;
				int n2_label = 0;			
				if (n1)
					n1_label = labels_p[j-1];
				if (n2)
					n2_label = labels_pp[j];

				//find the min value
				int min_label = std::min(n1_label,n2_label);
				if (!min_label)
					min_label = std::max(n1_label,n2_label);

				//assign the label + update the region size
				labels_p[j] = min_label;
				label_size[min_label]++;

				//union - merge two labels
				if (n1_label && n2_label)
					links.merge(n1_label,n2_label);
			}
		}
	}

	//prepare the label vector
	std::vector<int> new_labels(links.length());
	for (unsigned int i = 0; i < new_labels.size(); i++)
	{
		new_labels[i] = links.find(i);
		if (new_labels[i] != i)
			label_size[new_labels[i]] += label_size[i];
	}

	//reassign the indices so they are labelled from 1-N
	int counter = 0;
	for (unsigned int i = 0; i < new_labels.size(); i++)
	{
		bool detected = false;
		for (unsigned int j = 0; j < new_labels.size(); j++)
		{
			if (new_labels[j] == i)
			{
				detected = true;
				new_labels[j] = counter;
			}
		}
		if (detected)
		{
			label_size[counter] = label_size[i];
			counter++;
		}
	}

	//filter out small objects
	for (int i = 0; i < counter; i++)
	{
		for (unsigned int j = 0; j < label_size.size(); j++)
		{
			if ((new_labels[j]==i) && (label_size[i] < min_region))
				new_labels[j] = 0;
		}
	}

	//fix labels
	for(int i = 0; i < labels.rows; i++)
	{
		int *labels_p = labels.ptr<int>(i);
		for(int j = 0; j < labels.cols; j++)
			labels_p[j] = new_labels[labels_p[j]];
	}
}

