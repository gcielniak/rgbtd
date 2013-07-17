%speed test

dataset_20130627 = load_dataset('D:\\data\\rgbdt\\dataset_20130627\\');
dataset_20130628 = load_dataset('D:\\data\\rgbdt\\dataset_20130628\\');

labels_1 = dataset_20130627(:,1);
labels_2 = dataset_20130628(:,1);

dataset_1 = dataset_20130627(:,2:end);
dataset_2 = dataset_20130628(:,2:end);

[dataset_1, minv, maxv] = scale_svm(dataset_1);
dataset_2 = scale_svm(dataset_2, minv, maxv);
dataset_1(isnan(dataset_1))=0;
dataset_2(isnan(dataset_2))=0;

start = tic;
fprintf('Training... ');
model = svmtrain(labels_1,dataset_1,'-q');
fprintf('done in %.2f s\n',toc(start));

start = tic;
fprintf('Testing... ');
[p_label, accuracy, dv] = svmpredict(labels_2,dataset_2, model,'-q');
fprintf('done in %.2f s\n',toc(start));
accuracy(1)