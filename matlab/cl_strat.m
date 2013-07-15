%cl_strat

%feature analysis

load '..\data\dataset_20130627';
load '..\data\dataset_20130628';

testing_examples = 1:10:size(dataset_20130628,1);
labels_2 = dataset_20130628(testing_examples,1);

acc = [];

training_examples = [];
classes = unique(labels_1);
h = hist(labels_1,classes);

for i=1:max(h)
    training_examples{i} = stratify_index(labels_1,i);
end

training_examples{max(h)+1} = 1:size(dataset_20130627,1);

features = (1:257)+1;

for i = 1:size(training_examples,2)
    
    fprintf('Feature set %d\n',i);

    dataset_1 = dataset_20130627(training_examples{i},features);
    dataset_2 = dataset_20130628(testing_examples, features);

    [dataset_1, minv, maxv] = scale_svm(dataset_1);
    dataset_2 = scale_svm(dataset_2, minv, maxv);
    dataset_1(isnan(dataset_1))=0;
    dataset_2(isnan(dataset_2))=0;

    fprintf('Training... ');
    start = tic;
    model = svmtrain(dataset_20130627(training_examples{i},1),dataset_1,'-q');
    fprintf('done in %.2f s\n',toc(start));

    fprintf('Testing... ');
    start = tic;
    [p_label, accuracy, dv] = svmpredict(labels_2,dataset_2, model,'-q');
    fprintf('done in %.2f s\n',toc(start));

    acc(i,:) = accuracy;
end
