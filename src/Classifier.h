#pragma once

namespace LinLib
{
	class Classifier
	{
	public:
		void TrainTest()
		{
			cv::Mat dataset_1, dataset_2;

			cv::FileStorage file_storage;

			file_storage.open("D:\\data\\rgbdt\\dataset_20130627\\dataset.xml", cv::FileStorage::READ);
			file_storage["dataset"] >> dataset_1;
			file_storage.release();

			file_storage.open("D:\\data\\rgbdt\\dataset_20130628\\dataset.xml", cv::FileStorage::READ);
			file_storage["dataset"] >> dataset_2;
			file_storage.release();

			cv::Mat train_set(dataset_1, cv::Range::all(), cv::Range(1,dataset_1.cols));
			train_set.convertTo(train_set, CV_32FC1);
			cv::Mat train_labels(dataset_1, cv::Range::all(), cv::Range(0,1));
			cv::Mat train_labels_int;
			train_labels.convertTo(train_labels_int, CV_32SC1);

			cv::Mat test_set(dataset_2, cv::Range::all(), cv::Range(1,dataset_2.cols));
			test_set.convertTo(test_set, CV_32FC1);
			cv::Mat test_labels(dataset_2, cv::Range::all(), cv::Range(0,1));
			cv::Mat test_labels_int;
			test_labels.convertTo(test_labels_int, CV_32SC1);

			// Train the SVM
			CvSVM SVM;

			CvSVMParams params;
			params.svm_type    = CvSVM::C_SVC;
			params.kernel_type = CvSVM::RBF;
			params.term_crit   = cvTermCriteria(CV_TERMCRIT_ITER, 100, 0.001);

			cerr << "Training... ";
			SVM.train(train_set, train_labels_int, cv::Mat(), cv::Mat(), params);
			cerr << "done." << endl;

			cerr << "Testing on train... ";
			cv::Mat train_result;
			SVM.predict(train_set, train_result);
			cerr << "done." << endl;
			cerr << cv::countNonZero(train_result-train_labels) << " out of " << train_set.rows << endl;
			cerr << (double)cv::countNonZero(train_result-train_labels)*100.0/train_set.rows << " % error." << endl;

			cerr << "Testing on test... ";
			cv::Mat test_result;
			SVM.predict(test_set, test_result);
			cerr << "done." << endl;
			cerr << cv::countNonZero(test_result-test_labels) << " out of " << test_set.rows << endl;
			cerr << (double)cv::countNonZero(test_result-test_labels)*100.0/test_set.rows << " % error." << endl;
		}
	};
}