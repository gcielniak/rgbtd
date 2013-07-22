#pragma once

namespace LinLib
{
	class Classifier
	{
	public:
		void TrainTest()
		{
			cv::FileStorage file_storage;

			file_storage.open("D:\\data\\rgbdt\\dataset_20130627\\dataset.xml", cv::FileStorage::READ);

			cv::Mat dataset;

			file_storage["dataset"] >> dataset;

			file_storage.release();

			cv::Mat training_set(dataset, cv::Range::all(), cv::Range(1,dataset.cols));
			training_set.convertTo(training_set, CV_32FC1);
//			cv::Mat training_labels(dataset, cv::Range::all(), cv::Range(0,1));
			cv::Mat training_labels_int;
			dataset(cv::Range::all(), cv::Range(0,1)).convertTo(training_labels_int, CV_32SC1);

			//implement normalization

			cv::Mat reduce_min, reduce_max, reduce_range;

			cv::reduce(training_set, reduce_min, 0, CV_REDUCE_MIN);
			cv::reduce(training_set, reduce_max, 0, CV_REDUCE_MAX);
			reduce_range = reduce_max - reduce_min;

//			cerr << training_set << endl;

			for (int i = 0; i < training_set.rows; i++)
			{
				training_set(cv::Range(i,i+1),cv::Range::all()) -= reduce_min;
				training_set(cv::Range(i,i+1),cv::Range::all()) /= reduce_range;
			}
			training_set *= 2;
			training_set -= 1;

//			cerr << training_set << endl;

			// Train the SVM
			CvSVM SVM;

			CvSVMParams params;
			params.svm_type    = CvSVM::C_SVC;
			params.kernel_type = CvSVM::RBF;
			params.term_crit   = cvTermCriteria(CV_TERMCRIT_ITER, 100, 0.001);

			cerr << "Training... ";
			SVM.train(training_set, training_labels_int, cv::Mat(), cv::Mat(), params);
			cerr << "done." << endl;

			cerr << "Testing... ";
			cv::Mat result;
			SVM.predict(training_set, result);
			cerr << "done." << endl;
			cerr << cv::countNonZero(result-dataset(cv::Range::all(), cv::Range(0,1))) << " out of " << dataset.rows << endl;
			cerr << (double)cv::countNonZero(result-dataset(cv::Range::all(), cv::Range(0,1)))/dataset.rows << " % error." << endl;
		}
	};
}