%example svm training file
load '..\data\dataset_20130627';
load '..\data\dataset_20130628';

labels_1 = dataset_20130627(:,1);
dataset_1 = dataset_20130627(:,2:258);

min_v = min(dataset_1);
max_v = max(dataset_1);

for i=1:size(dataset_1,1)
    dataset_1(i,:) = (dataset_1(i,:)-min_v)./max_v;
end


model = svmtrain(labels_1,dataset_1);

[p_label, accuracy, dv] = svmpredict(labels_1,dataset_1, model);


