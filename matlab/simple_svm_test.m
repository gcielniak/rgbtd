%simple svm test

dataset_20130627 = load_dataset('D:\\data\\rgbdt\\dataset_20130627\\');
dataset_20130628 = load_dataset('D:\\data\\rgbdt\\dataset_20130628\\');

training_examples = 1:size(dataset_20130627,1);
testing_examples = 1:size(dataset_20130628,1);

labels_1 = dataset_20130627(training_examples,1);
labels_2 = dataset_20130628(testing_examples,1);

dataset_1 = dataset_20130627(training_examples,2:end);
dataset_2 = dataset_20130628(testing_examples,2:end);

%normalise data
[dataset_1, minv, maxv] = scale_svm(dataset_1);
dataset_2 = scale_svm(dataset_2, minv, maxv);
dataset_1(isnan(dataset_1))=0;
dataset_2(isnan(dataset_2))=0;

acc = [];

for i = 1:257
    fprintf('Training... ');
    model = svmtrain(labels_1,dataset_1(:,[i]),'-q');
    fprintf('done\n');
    
    fprintf('Testing... ');
    [p_label, accuracy, dv] = svmpredict(labels_2,dataset_2(:,[i]), model,'-q');
    fprintf('done\n');
    acc(i) = accuracy(1);
end