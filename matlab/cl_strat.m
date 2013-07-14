%cl_strat

load '..\data\dataset_20130627';
load '..\data\dataset_20130628';

acc = [];

labels_1 = dataset_20130627(:,1);
labels_2 = dataset_20130628(:,1);

training_indx = [];
classes = unique(labels_1);
h = hist(labels_1,classes);

for i=1:max(h)
    training_indx{i} = stratify_index(labels_1,i);
end

training_indx{max(h)+1} = 1:size(labels_1,1);

features = (1:257)+1;

for i = 1:size(training_indx,2)
    
    fprintf('Feature set %d\n',i);

    dataset_1 = dataset_20130627(training_indx{i},features);
    dataset_2 = dataset_20130628(:, features);

    dataset = [dataset_1; dataset_2];
    dataset = scale_svm(dataset);
    dataset = remove_nan(dataset);
    dataset_1 = dataset(1:size(dataset_1,1),:);
    dataset_2 = dataset(size(dataset_1,1)+1:end,:);

    fprintf('Training... ');
    start = tic;
    model = svmtrain(labels_1(training_indx{i}),dataset_1,'-q');
    fprintf('done in %.2f s\n',toc(start));

    fprintf('Testing... ');
    start = tic;
    [p_label, accuracy, dv] = svmpredict(labels_2,dataset_2, model,'-q');
    fprintf('done in %.2f s\n',toc(start));

    acc(i,:) = accuracy;
end
