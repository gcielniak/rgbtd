%modality analysis

%example svm training file
load '..\data\dataset_20130627';
load '..\data\dataset_20130628';

acc = [];
unique = [];

labels_1 = dataset_20130627(:,1);
labels_2 = dataset_20130628(:,1);

features = [];

features{1} = (1:2056)+2056*0; % color
features{2} = (1:2056)+2056*1; % depth
features{3} = (1:2056)+2056*2; % thermal
features{4} = [features{1} features{2}]; %cd
features{5} = [features{1} features{3}]; %ct
features{6} = [features{2} features{3}]; %dt
features{7} = 1:6168; % all features

for i=1:8
    features{8+((i-1)*7)} = (1:257)+2056*0+257*(i-1); % color
    features{9+((i-1)*7)} = (1:257)+2056*1+257*(i-1); % depth
    features{10+((i-1)*7)} = (1:257)+2056*2+257*(i-1); % thermal
    features{11+((i-1)*7)} = [features{8+(i-1)*7} features{9+(i-1)*7}]; %cd
    features{12+((i-1)*7)} = [features{8+(i-1)*7} features{10+(i-1)*7}]; %ct
    features{13+((i-1)*7)} = [features{9+(i-1)*7} features{10+(i-1)*7}]; %dt
    features{14+((i-1)*7)} = [features{8+(i-1)*7} features{9+(i-1)*7} features{10+(i-1)*7}]; %cdt
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

bar(0:8,reshape(acc(:,1),7,9)'); legend('c','d','t','cd','ct','dt','cdt'); xlabel('resolution (pyramid level)'); ylabel('accuracy');title('single pyramid');set(gca,'XTickLabel',{'all','640x480','320x240','160x120','80x60','40x30','20x15','10x7','5x3'});
