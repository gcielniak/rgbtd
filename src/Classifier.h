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


			CvSVMParams params;
			params.svm_type    = CvSVM::C_SVC;
			params.kernel_type = CvSVM::RBF;
			params.term_crit   = cvTermCriteria(CV_TERMCRIT_ITER, 100, 0.001);

			cv::Mat training_set(dataset, cv::Range::all(), cv::Range(1,dataset.cols));
			cv::Mat training_labels(dataset, cv::Range::all(), cv::Range(1,1));


			// Train the SVM
			CvSVM SVM;
			SVM.train(training_set, training_labels, cv::Mat(), cv::Mat(), params);
		}
	};
}