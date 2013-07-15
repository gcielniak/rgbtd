%feature analysis

load '..\data\dataset_20130627';
load '..\data\dataset_20130628';

training_examples = 1:10:size(dataset_20130627,1);
testing_examples = 1:10:size(dataset_20130628,1);

labels_1 = dataset_20130627(training_examples,1);
labels_2 = dataset_20130628(testing_examples,1);

acc = [];
step = 1;
max_indicies = [];
max_values = [];
ft_sets = [];
ft_weak = [];
ft_step = 1;

features_orig = 1:257;

features = features_orig;
dataset_1 = dataset_20130627(training_examples,features+1);
dataset_2 = dataset_20130628(testing_examples,features+1);

[dataset_1, minv, maxv] = scale_svm(dataset_1);
dataset_2 = scale_svm(dataset_2, minv, maxv);
dataset_1(isnan(dataset_1))=0;
dataset_2(isnan(dataset_2))=0;

fprintf('Training... ');
model = svmtrain(labels_1,dataset_1,'-q');
fprintf('done\n');

fprintf('Testing... ');
[p_label, accuracy, dv] = svmpredict(labels_2,dataset_2, model,'-q');
fprintf('done\n');

max_indicies(ft_step) = 0;
max_values(ft_step) = accuracy(1);
ft_sets{ft_step} = features_orig;
ft_step = ft_step + 1; 

while length(features_orig)
    ind_max = -1;
    value_max = -Inf;
    for i = 1:length(features_orig)
        features = features_orig;
        features(i) = [];

        dataset_1 = dataset_20130627(training_examples,features+1);
        dataset_2 = dataset_20130628(testing_examples,features+1);

        [dataset_1, minv, maxv] = scale_svm(dataset_1);
        dataset_2 = scale_svm(dataset_2, minv, maxv);
        dataset_1(isnan(dataset_1))=0;
        dataset_2(isnan(dataset_2))=0;
    
        fprintf('Training... ');
        model = svmtrain(labels_1,dataset_1,'-q');
        fprintf('done\n');

        fprintf('Testing... ');
        [p_label, accuracy, dv] = svmpredict(labels_2,dataset_2, model,'-q');
        fprintf('done\n');

        if accuracy(1) > value_max
            ind_max = i;
            value_max = accuracy(1);            
        end
    end
    max_indicies(ft_step) = ind_max;
    max_values(ft_step) = value_max;
    ft_weak(ft_step) = features_orig(ind_max);
    features_orig(ind_max) = [];
    ft_sets{ft_step} = features_orig;
    ft_step = ft_step + 1; 
end

plot(max_values);
%[mv, mi] = max(max_values);
%ft_sets{mi}
%ft_weak
