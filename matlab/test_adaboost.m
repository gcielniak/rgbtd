%test adaboost

dataset_20130627 = load_dataset('D:\\data\\rgbdt\\dataset_20130627_fov\\');
dataset_20130628 = load_dataset('D:\\data\\rgbdt\\dataset_20130628_fov\\');

training_examples = 1:size(dataset_20130627,1);
testing_examples = 1:size(dataset_20130628,1);
features = (1:257)+1;

labels_1 = dataset_20130627(training_examples,1);
labels_2 = dataset_20130628(testing_examples,1);

dataset_1 = dataset_20130627(training_examples, features);
dataset_2 = dataset_20130628(testing_examples, features);

ens = fitensemble(dataset_1,labels_1,'AdaBoostM2',100,'Tree');

ens.loss(dataset_1,labels_1)

imp = ens.predictorImportance;
imp(find(imp)); find(imp);