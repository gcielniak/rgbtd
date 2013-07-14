%adaboost

%example svm training file
load '..\data\dataset_20130627';
load '..\data\dataset_20130628';

acc = [];
unique = [];

for i = 1
    fprintf('Feature set %d\n',i);

    features = (2:258)+(i-1)*257;
    
    labels_1 = dataset_20130627(:,1);
    dataset_1 = dataset_20130627(:,features);
    labels_2 = dataset_20130628(:,1);
    dataset_2 = dataset_20130628(:,features);

%    dataset_1 = scale_svm(dataset_1);
%    dataset_1 = remove_nan(dataset_1);
%    dataset_2 = scale_svm(dataset_2);
%    dataset_2 = remove_nan(dataset_2);

    unique(i,1) = size(dataset_1,2);
    unique(i,2) = size(dataset_2,2);
    
    ada = fitensemble(dataset_1,nominal(labels_1),'AdaBoostM2',100,'Tree');
    out = predict(ada,dataset_2);
end

size(find(labels_2-double(out)),1)/size(labels_2,1)
