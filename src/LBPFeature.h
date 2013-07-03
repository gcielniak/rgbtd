#pragma once

namespace LinLib
{

	enum BorderType
	{
		BORDER_PADD,
		BORDER_NAN
	};

	/// class implementing Local Binary Pattern feature
	/// supports:
	/// - NaN values specified as a specific index value
	/// - two different ways of handling borders: either as NaN values or padded with the nearest pixel
	/// - visualisation of the features in form of an image
	class LBPFeature
	{
		cv::Mat feature, feature_image, feature_pyr;
		BorderType border_type;

		/// calculate lbp for uchar, used by the border filling procedure
		inline int CalcIndex(uchar p, uchar pa, uchar pb, uchar pc, uchar pd, uchar pe, uchar pf, uchar pg, uchar ph, int nan_value)
		{
			uchar a = 1, b = 2, c = 4, d = 8, e = 16, f = 32, g = 64, h = 128;

			//check for nan_values
			if (nan_value != -1)
			{
				if ((p == nan_value) || (pa == nan_value) || (pb == nan_value) || 
					(pc == nan_value) || (pd == nan_value) || (pe == nan_value) || 
					(pf == nan_value) || (pg == nan_value) || (ph == nan_value))
					return 256;
			}

			if ((p - pa) < 0) a = 0;
			if ((p - pb) < 0) b = 0;
			if ((p - pc) < 0) c = 0;
			if ((p - pd) < 0) d = 0;
			if ((p - pe) < 0) e = 0;
			if ((p - pf) < 0) f = 0;
			if ((p - pg) < 0) g = 0;
			if ((p - ph) < 0) h = 0;
			return (a | b | c | d | e | f | g | h);
		}

		/// calculate lbp for ushort, used by the border filling procedure
		inline int CalcIndex(ushort p, ushort pa, ushort pb, ushort pc, ushort pd, ushort pe, ushort pf, ushort pg, ushort ph, int nan_value)
		{
			uchar a = 1, b = 2, c = 4, d = 8, e = 16, f = 32, g = 64, h = 128;

			//check for nan_values
			if (nan_value != -1)
			{
				if ((p == nan_value) || (pa == nan_value) || (pb == nan_value) || 
					(pc == nan_value) || (pd == nan_value) || (pe == nan_value) || 
					(pf == nan_value) || (pg == nan_value) || (ph == nan_value))
					return 256;
			}

			if ((p - pa) < 0) a = 0;
			if ((p - pb) < 0) b = 0;
			if ((p - pc) < 0) c = 0;
			if ((p - pd) < 0) d = 0;
			if ((p - pe) < 0) e = 0;
			if ((p - pf) < 0) f = 0;
			if ((p - pg) < 0) g = 0;
			if ((p - ph) < 0) h = 0;
			return (a | b | c | d | e | f | g | h);
		}

		/// calculate lbp feature, with the specified NaN value (-1 = ignore)
		cv::Mat& Calculate(const cv::Mat &_image, int nan_value=-1)
		{
			uchar p, *_p, *_p1, *_p2;
			ushort pp, *_pp, *_pp1, *_pp2;
			float *_pd;
			bool perform_calculation;

			cv::Mat image = _image;
			cv::Mat image_t;

			feature = cv::Mat::zeros(257, 1, CV_32F);

			switch(image.type())
			{
			case CV_8UC3:
				image_t = cv::Mat(image.rows, image.cols, CV_8UC1);
				cv::cvtColor(image, image_t, CV_RGB2GRAY);
				image = image_t;
			case CV_8UC1:
				_pd = (float*)feature.data;

				for(int i = 1; i < image.rows-1; i++)
				{
					_p = image.ptr<uchar>(i);
					_p1 = image.ptr<uchar>(i-1);
					_p2 = image.ptr<uchar>(i+1);
					for(int j = 1; j < image.cols-1; j++)
					{
						uchar a = 1, b = 2, c = 4, d = 8, e = 16, f = 32, g = 64, h = 128;
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
							if ((p - _p[j+1]) < 0) a = 0;
							if ((p - _p2[j+1]) < 0) b = 0;
							if ((p - _p2[j]) < 0) c = 0;
							if ((p - _p2[j-1]) < 0) d = 0;
							if ((p - _p[j-1]) < 0) e = 0;
							if ((p - _p1[j-1]) < 0) f = 0;
							if ((p - _p1[j]) < 0) g = 0;
							if ((p - _p1[j+1]) < 0) h = 0;
							index = (a | b | c | d | e | f | g | h);
						}

						_pd[index]++;
					}
				}
				break;
			case CV_16UC1:
				_pd = (float*)feature.data;
				for(int i = 1; i < image.rows-1; i++)
				{
					_pp = image.ptr<ushort>(i);
					_pp1 = image.ptr<ushort>(i-1);
					_pp2 = image.ptr<ushort>(i+1);
					for(int j = 1; j < image.cols-1; j++)
					{
						uchar a = 1, b = 2, c = 4, d = 8, e = 16, f = 32, g = 64, h = 128;
						pp = _pp[j];
						perform_calculation = true;

						//check for nan_values
						if (nan_value != -1)
						{
							if ((_pp[j-1] == nan_value) || (pp == nan_value) || (_pp[j+1] == nan_value) || 
								(_pp1[j-1] == nan_value) || (_pp1[j] == nan_value) || (_pp1[j+1] == nan_value) || 
								(_pp2[j-1] == nan_value) || (_pp2[j] == nan_value) || (_pp2[j+1] == nan_value))
							{
								_pd[256]++;
								perform_calculation = false;
							}
						}

						if (perform_calculation)
						{
							if ((pp - _pp[j+1]) < 0) a = 0;
							if ((pp - _pp2[j+1]) < 0) b = 0;
							if ((pp - _pp2[j]) < 0) c = 0;
							if ((pp - _pp2[j-1]) < 0) d = 0;
							if ((pp - _pp[j-1]) < 0) e = 0;
							if ((pp - _pp1[j-1]) < 0) f = 0;
							if ((pp - _pp1[j]) < 0) g = 0;
							if ((pp - _pp1[j+1]) < 0) h = 0;
							_pd[a | b | c | d | e | f | g | h]++;
						}
					}
				}
				break;
			default:
				throw new Exception("LBPFeature::Calculate: unsupported image format.");
			}

			//handle borders
			switch (border_type)
			{
			case BORDER_PADD:
				_pd = (float*)feature.data;

				if (image.type() == CV_8UC1)
				{
					//first row: e p a, e p a, d c b
					_p = image.ptr<uchar>(0);
					_p2 = image.ptr<uchar>(1);
					for(int j = 1; j < image.cols-1; j++)
						_pd[CalcIndex(_p[j], _p[j+1], _p2[j+1], _p2[j], _p2[j-1], _p[j-1], _p[j-1], _p[j], _p[j+1], nan_value)]++;

					//top corner points: p p a, p p a, c c b
					_pd[CalcIndex(_p[0], _p[1], _p2[1], _p2[0], _p2[0], _p[0], _p[0], _p[0], _p[1], nan_value)]++;
					// e p p, e p p, d c c
					_pd[CalcIndex(_p[image.cols-1], _p[image.cols-1], _p2[image.cols-1], _p2[image.cols-1], _p2[image.cols-2], _p[image.cols-2], _p[image.cols-2], _p[image.cols-1], _p[image.cols-1], nan_value)]++;

					//last row: f g h, e p a, e p a
					_p1 = image.ptr<uchar>(image.rows-2);
					_p = image.ptr<uchar>(image.rows-1);
					for(int j = 1; j < image.cols-1; j++)
						_pd[CalcIndex(_p[j], _p[j+1], _p[j+1], _p[j], _p[j-1], _p[j-1], _p1[j-1], _p1[j], _p1[j+1], nan_value)]++;

					//bottom corner points: g g h, p p a, p p a
					_pd[CalcIndex(_p[0], _p[1], _p[1], _p[0], _p[0], _p[0], _p1[0], _p1[0], _p1[1], nan_value)]++;
					// f g g, e p p, e p p
					_pd[CalcIndex(_p[image.cols-1], _p[image.cols-1], _p[image.cols-1], _p[image.cols-1], _p[image.cols-2], _p[image.cols-2], _p1[image.cols-2], _p1[image.cols-1], _p1[image.cols-1], nan_value)]++;

					//first and last column 
					for(int i = 1; i < image.rows-1; i++)
					{
						_p = image.ptr<uchar>(i);
						_p1 = image.ptr<uchar>(i-1);
						_p2 = image.ptr<uchar>(i+1);
						// g g h, p p a, c c b
						_pd[CalcIndex(_p[0], _p[1], _p2[1], _p2[0], _p2[0], _p[0], _p1[0], _p1[0], _p1[1], nan_value)]++;
						// f g g, e p p, d c c
						_pd[CalcIndex(_p[image.cols-1], _p[image.cols-1], _p2[image.cols-1], _p2[image.cols-1], _p2[image.cols-2], _p[image.cols-2], _p1[image.cols-2], _p1[image.cols-1], _p1[image.cols-1], nan_value)]++;
					}
				}
				else if (image.type() == CV_16UC1)
				{
					//first row: e p a, e p a, d c b
					_pp = image.ptr<ushort>(0);
					_pp2 = image.ptr<ushort>(1);
					for(int j = 1; j < image.cols-1; j++)
						_pd[CalcIndex(_pp[j], _pp[j+1], _pp2[j+1], _pp2[j], _pp2[j-1], _pp[j-1], _pp[j-1], _pp[j], _pp[j+1], nan_value)]++;

					//top corner points: p p a, p p a, c c b
					_pd[CalcIndex(_pp[0], _pp[1], _pp2[1], _pp2[0], _pp2[0], _pp[0], _pp[0], _pp[0], _pp[1], nan_value)]++;
					// e p p, e p p, d c c
					_pd[CalcIndex(_pp[image.cols-1], _pp[image.cols-1], _pp2[image.cols-1], _pp2[image.cols-1], _pp2[image.cols-2], _pp[image.cols-2], _pp[image.cols-2], _pp[image.cols-1], _pp[image.cols-1], nan_value)]++;

					//last row: f g h, e p a, e p a
					_pp1 = image.ptr<ushort>(image.rows-2);
					_pp = image.ptr<ushort>(image.rows-1);
					for(int j = 1; j < image.cols-1; j++)
						_pd[CalcIndex(_pp[j], _pp[j+1], _pp[j+1], _pp[j], _pp[j-1], _pp[j-1], _pp1[j-1], _pp1[j], _pp1[j+1], nan_value)]++;

					//bottom corner points: g g h, p p a, p p a
					_pd[CalcIndex(_pp[0], _pp[1], _pp[1], _pp[0], _pp[0], _pp[0], _pp1[0], _pp1[0], _pp1[1], nan_value)]++;
					// f g g, e p p, e p p
					_pd[CalcIndex(_pp[image.cols-1], _pp[image.cols-1], _pp[image.cols-1], _pp[image.cols-1], _pp[image.cols-2], _pp[image.cols-2], _pp1[image.cols-2], _pp1[image.cols-1], _pp1[image.cols-1], nan_value)]++;

					//first and last column 
					for(int i = 1; i < image.rows-1; i++)
					{
						_pp = image.ptr<ushort>(i);
						_pp1 = image.ptr<ushort>(i-1);
						_pp2 = image.ptr<ushort>(i+1);
						// g g h, p p a, c c b
						_pd[CalcIndex(_pp[0], _pp[1], _pp2[1], _pp2[0], _pp2[0], _pp[0], _pp1[0], _pp1[0], _pp1[1], nan_value)]++;
						// f g g, e p p, d c c
						_pd[CalcIndex(_pp[image.cols-1], _pp[image.cols-1], _pp2[image.cols-1], _pp2[image.cols-1], _pp2[image.cols-2], _pp[image.cols-2], _pp1[image.cols-2], _pp1[image.cols-1], _pp1[image.cols-1], nan_value)]++;
					}
				}
				break;
			case BORDER_NAN:
				feature.at<float>(256) = (float)((_image.rows + _image.cols)*2-4); //borders as NaNs
				break;
			default:
				break;
			}
			cv::normalize(feature, feature, 1.0, 0.0, cv::NORM_L1);

			return feature;
		}

	public:
		/// Constructor
		LBPFeature()
			: border_type(BORDER_NAN) {}

		/// Set border type
		void BorderHandling(BorderType type)
		{
			border_type = type;
		}

		/// Get border type 
		BorderType BorderHandling()
		{
			return border_type;
		}

		/// Get the feature with a specified pyramid depth (def: 0) and NaN value (def -1 = no NaN)
		cv::Mat& Get(const cv::Mat &_image, int depth=0, int nan_value=-1)
		{
			feature = Calculate(_image, nan_value);

			if (depth)
			{
				cv::Mat image = _image.clone();
				feature_pyr = feature.clone();

				for (int i = 0; i < depth; i++)
				{
					cv::pyrDown(image, image);
					cv::Mat feature_part = Calculate(image, nan_value);
					cv::vconcat(feature_pyr, feature_part, feature_pyr);
				}

				return feature_pyr;
			}

			return feature;
		}

		/// Visualise features as an image
		cv::Mat& GetFeatureImage(const cv::Mat &_image)
		{
			uchar p, *_p, *_p1, *_p2;
			ushort pp, *_pp, *_pp1, *_pp2;
			uchar *_pd;

			cv::Mat image = _image;
			cv::Mat image_t;

			if ((feature_image.cols != image.cols) || (feature_image.rows != image.rows) || (feature_image.type() != CV_8UC1))
				feature_image = cv::Mat::zeros(image.rows, image.cols, CV_8UC1);

			switch(image.type())
			{
			case CV_8UC3:
				image_t = cv::Mat(image.rows, image.cols, CV_8UC1);
				cv::cvtColor(image, image_t, CV_RGB2GRAY);
				image = image_t;
			case CV_8UC1:
				for(int i = 1; i < image.rows-1; i++)
				{
					_p = image.ptr<uchar>(i);
					_p1 = image.ptr<uchar>(i-1);
					_p2 = image.ptr<uchar>(i+1);
					_pd = feature_image.ptr<uchar>(i);
					for(int j = 1; j < image.cols-1; j++)
					{
						uchar a = 1, b = 2, c = 4, d = 8, e = 16, f = 32, g = 64, h = 128;
						p = _p[j];
						if ((p - _p[j+1]) < 0) a = 0;
						if ((p - _p2[j+1]) < 0) b = 0;
						if ((p - _p2[j]) < 0) c = 0;
						if ((p - _p2[j-1]) < 0) d = 0;
						if ((p - _p[j-1]) < 0) e = 0;
						if ((p - _p1[j-1]) < 0) f = 0;
						if ((p - _p1[j]) < 0) g = 0;
						if ((p - _p1[j+1]) < 0) h = 0;
						_pd[j] = a | b | c | d | e | f | g | h;
					}
				}
				break;
			case CV_16UC1:
				for(int i = 1; i < image.rows-1; i++)
				{
					_pp = image.ptr<ushort>(i);
					_pp1 = image.ptr<ushort>(i-1);
					_pp2 = image.ptr<ushort>(i+1);
					_pd = feature_image.ptr<uchar>(i);
					for(int j = 1; j < image.cols-1; j++)
					{
						uchar a = 1, b = 2, c = 4, d = 8, e = 16, f = 32, g = 64, h = 128;
						pp = _pp[j];
						if ((pp - _pp[j+1]) < 0) a = 0;
						if ((pp - _pp2[j+1]) < 0) b = 0;
						if ((pp - _pp2[j]) < 0) c = 0;
						if ((pp - _pp2[j-1]) < 0) d = 0;
						if ((pp - _pp[j-1]) < 0) e = 0;
						if ((pp - _pp1[j-1]) < 0) f = 0;
						if ((pp - _pp1[j]) < 0) g = 0;
						if ((pp - _pp1[j+1]) < 0) h = 0;
						_pd[j] = a | b | c | d | e | f | g | h;
					}
				}
				break;
			default:
				throw new Exception("LBPFeature::GetFeatureImage: unsupported image format.");
			}

			return feature_image;
		}
	};
}