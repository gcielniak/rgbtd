%example svm training file
load '..\data\dataset_20130627';
load '..\data\dataset_20130628';

acc = [];
unique = [];

labels_1 = dataset_20130627(:,1);
labels_2 = dataset_20130628(:,1);

features = [];

features{1} = 1:6168; % all features
features{2} = (1:2056)+2056*0; % color
features{3} = (1:2056)+2056*1; % depth
features{4} = (1:2056)+2056*2; % thermal
features{5} = [features{2} features{3}]; %cd
features{6} = [features{2} features{4}]; %ct
features{7} = [features{3} features{4}]; %dt
%pyramid-wise
for i = 1:24
    features{8+(i-1)} = (1:257)+257*(i-1);
end

for i = 1:8
    features{32+(i-1)} = [features{8+(i-1)} features{8+8+(i-1)}];%cd
end

for i = 1:8
    features{40+(i-1)} = [features{8+(i-1)} features{8+16+(i-1)}];%ct
end

for i = 1:8
    features{48+(i-1)} = [features{8+8+(i-1)} features{8+16+(i-1)}];%dt
end

for i = 1:size(features,1)
    
    fprintf('Feature set %d\n',i);

    dataset_1 = dataset_20130627(:,features{i}+1);
    dataset_2 = dataset_20130628(:,features{i}+1);

    dataset = [dataset_1; dataset_2];
    dataset = scale_svm(dataset);
    dataset = remove_nan(dataset);
    dataset_1 = dataset(1:size(dataset_1,1),:);
    dataset_2 = dataset(size(dataset_1,1)+1:end,:);

    unique(i,1) = size(dataset_1,2);
    unique(i,2) = size(dataset_2,2);

    fprintf('Training... ');
    model = svmtrain(labels_1,dataset_1,'-q');
    fprintf('done\n');

    fprintf('Testing... ');
    [p_label, accuracy, dv] = svmpredict(labels_2,dataset_2, model,'-q');
    fprintf('done\n');

    acc(i,:) = accuracy;
end

acc
