#pragma once

namespace LinLib
{
	class LBPFeature2
	{
	public:
		static void Calculate(const cv::Mat &input_image, cv::Mat &feature, int nan_value=-1, BorderType border_type=BorderType::BORDER_NAN)
		{
			uchar p, *_p, *_p1, *_p2;
			ushort pp, *_pp, *_pp1, *_pp2;

			cv::Mat image = input_image;
			cv::Mat image_grey;

			//init a feature vector (256 + 1 for NaN in depth images)
			feature = cv::Mat::zeros(1, 257, CV_32F);
			float *_pf = (float*)feature.data;

			switch(image.type())
			{
				//perform conversion from color to greyscale
			case CV_8UC3:
				image_grey = cv::Mat(image.rows, image.cols, CV_8UC1);
				cv::cvtColor(image, image_grey, CV_RGB2GRAY);
				image = image_grey;
				//single channel uint8 image
			case CV_8UC1:
				for(int i = 1; i < image.rows-1; i++)
				{
					_p = image.ptr<uchar>(i);
					_p1 = image.ptr<uchar>(i-1);
					_p2 = image.ptr<uchar>(i+1);
					for(int j = 1; j < image.cols-1; j++)
					{
						p = _p[j];
						int index = 0;

						//check for nan_values
						if (nan_value != -1)
						{
							if ((_p[j-1] == nan_value) || (p == nan_value) || (_p[j+1] == nan_value) || 
								(_p1[j-1] == nan_value) || (_p1[j] == nan_value) || (_p1[j+1] == nan_value) || 
								(_p2[j-1] == nan_value) || (_p2[j] == nan_value) || (_p2[j+1] == nan_value))
								index = 256;
						}

						if (index != 256)
						{
							index |= ((p - _p[j+1]) >= 0);
							index |= ((p - _p2[j+1]) >= 0) << 1;
							index |= ((p - _p2[j]) >= 0) << 2;
							index |= ((p - _p2[j-1]) >= 0) << 3;
							index |= ((p - _p[j-1]) >= 0) << 4;
							index |= ((p - _p1[j-1]) >= 0) << 5;
							index |= ((p - _p1[j]) >= 0) << 6;
							index |= ((p - _p1[j+1]) >= 0) << 7;
						}

						_pf[index]++;
					}
				}
				break;
				//single channel uint16 image
			case CV_16UC1:
				for(int i = 1; i < image.rows-1; i++)
				{
					_pp = image.ptr<ushort>(i);
					_pp1 = image.ptr<ushort>(i-1);
					_pp2 = image.ptr<ushort>(i+1);
					for(int j = 1; j < image.cols-1; j++)
					{
						pp = _pp[j];
						int index = 0;

						//check for nan_values
						if (nan_value != -1)
						{
							if ((_pp[j-1] == nan_value) || (pp == nan_value) || (_pp[j+1] == nan_value) || 
								(_pp1[j-1] == nan_value) || (_pp1[j] == nan_value) || (_pp1[j+1] == nan_value) || 
								(_pp2[j-1] == nan_value) || (_pp2[j] == nan_value) || (_pp2[j+1] == nan_value))
							{
								index = 256;
							}
						}

						if (index != 256)
						{
							index |= ((pp - _pp[j+1]) >= 0);
							index |= ((pp - _pp2[j+1]) >= 0) << 1;
							index |= ((pp - _pp2[j]) >= 0) << 2;
							index |= ((pp - _pp2[j-1]) >= 0) << 3;
							index |= ((pp - _pp[j-1]) >= 0) << 4;
							index |= ((pp - _pp1[j-1]) >= 0) << 5;
							index |= ((pp - _pp1[j]) >= 0) << 6;
							index |= ((pp - _pp1[j+1]) >= 0) << 7;
						}

						_pf[index]++;
					}
				}
				break;
			default:
				throw new Exception("LBPFeature2::Calculate: unsupported image format.");
			}

			//handle borders
			switch (border_type)
			{
			case BORDER_NAN:
				feature.at<float>(256) = (float)((image.rows + image.cols)*2-4); //borders as NaNs
				break;
			case BORDER_PADD:
				break;
			default:
				break;
			}
	
			//normalisation
			cv::normalize(feature, feature, 1.0, 0.0, cv::NORM_L1);
		}
	};
}
