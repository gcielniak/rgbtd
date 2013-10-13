%adaboost

%example svm training file
%load '..\data\dataset_20130627';
%load '..\data\dataset_20130628';

dataset_20130627 = load_dataset('D:\\data\\rgbdt\\dataset_20130627\\');
dataset_20130628 = load_dataset('D:\\data\\rgbdt\\dataset_20130628\\');

acc = [];
unique = [];

features = (1:257)+1;

labels_1 = dataset_20130627(:,1)+1;
dataset_1 = dataset_20130627(:,features);
labels_2 = dataset_20130628(:,1)+1;
dataset_2 = dataset_20130628(:,features);

train_res = [];
test_res = [];
telapsed = [];
k = 1;

for i = 1
    fprintf('Training step %d...\n',i);
    tstart = tic;
    ada = fitensemble(dataset_1,nominal(labels_1),'AdaBoostM2',100,...
        ClassificationTree.template('MinLeaf',size(dataset_1,1)/50));
    telapsed(k) = toc(tstart);
    
    fprintf('Testing...\n')
    out_1 = predict(ada,dataset_1);
    out_2 = predict(ada,dataset_2);
    
    train_res(k) = length(find(labels_1-double(out_1)))/length(labels_1);
    test_res(k) = length(find(labels_2-double(out_2)))/length(labels_2);
    
    fprintf('Training error: %.3f\n',train_res(k));
    fprintf('Testing error: %.3f\n',test_res(k));
    k = k + 1;
end
