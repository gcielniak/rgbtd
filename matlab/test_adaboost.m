%test adaboost

dataset_20130627 = load_dataset('D:/data/rgbdt/dataset_20130627/');
dataset_20130628 = load_dataset('D:/data/rgbdt/dataset_20130628/');

training_examples = 1:size(dataset_20130627,1);
testing_examples = 1:size(dataset_20130628,1);
features = (1:size(dataset_20130627,2)-1)+1;
%features = (1:257)+1;

labels_1 = dataset_20130627(training_examples,1);
labels_2 = dataset_20130628(testing_examples,1);

dataset_1 = dataset_20130627(training_examples, features);
dataset_2 = dataset_20130628(testing_examples, features);

tic;
ens = fitensemble(dataset_1,labels_1,'AdaBoostM2',2000,'Tree');
toc;
%8/2.7 min. for 100 trees 0.37 
%16/5.58 min. for 200 trees error similar 0.37, suggested increased depth
%11 min. for 400 trees error similar 0.33, suggested increased depth
%28 min. for 1000 trees 0.283
%58 mub for 2000 trees 0.281

ens.loss(dataset_2,labels_2)

if 0
ens = [];
err_m = [];

i = 1;
for wl = [1 2 5 10 20 50 100 200 500]
    j = 1;
    for depth = 2:10        
        my_tree = ClassificationTree.template('MinLeaf',size(dataset_1,1)/depth);
        ens{i,j} = fitensemble(dataset_1,labels_1,'AdaBoostM2',wl,my_tree);
        err_m(i,j) = ens{i,j}.loss(dataset_2,labels_2);
        fprintf('WL: %d, D: %d, err: %.2f\n',wl,depth, err_m(i,j));
        j = j + 1;
    end
    i = i + 1;
end
end