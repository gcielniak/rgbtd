%feature analysis

%example svm training file
load '..\data\dataset_20130627';
load '..\data\dataset_20130628';

acc = [];
step = 1;

labels_1 = dataset_20130627(:,1);
labels_2 = dataset_20130628(:,1);

for j = 1:257
    
features = [];
features{1} = 1:257;

for i = 1:size(features{1},2)
    features{i+1} = features{1}; 
    features{i+1}(i) = [];
end

for i = 1:size(features,2)
    
    fprintf('Feature set %d\n',i);

    dataset_1 = dataset_20130627(:,features{i}+1);
    dataset_2 = dataset_20130628(:,features{i}+1);

    dataset = [dataset_1; dataset_2];
    dataset = scale_svm(dataset);
    dataset = remove_nan(dataset);
    dataset_1 = dataset(1:size(dataset_1,1),:);
    dataset_2 = dataset(size(dataset_1,1)+1:end,:);

    fprintf('Training... ');
    model = svmtrain(labels_1, dataset_1, '-q');
    fprintf('done\n');

    fprintf('Testing... ');
    [p_label, accuracy, dv] = svmpredict(labels_2, dataset_2, model, '-q');
    fprintf('done\n');

    acc(step,:) = accuracy;
    step = step + 1;
end

end