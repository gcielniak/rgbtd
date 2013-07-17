%modality analysis

dataset_20130627 = load_dataset('D:\\data\\rgbdt\\dataset_20130627_fov\\');
dataset_20130628 = load_dataset('D:\\data\\rgbdt\\dataset_20130628_fov\\');

training_examples = 1:size(dataset_20130627,1);
testing_examples = 1:size(dataset_20130628,1);

labels_1 = dataset_20130627(training_examples,1);
labels_2 = dataset_20130628(testing_examples,1);

%prepare feature combinations representing different modalities
features = [];

features{1} = (1:1542)+1542*0; % color
features{2} = (1:1542)+1542*1; % depth
features{3} = (1:1542)+1542*2; % thermal
features{4} = [features{1} features{2}]; %cd
features{5} = [features{1} features{3}]; %ct
features{6} = [features{2} features{3}]; %dt
features{7} = 1:4626; % all features

for i=1:6
    features{8+((i-1)*7)} = (1:257)+1542*0+257*(i-1); % color
    features{9+((i-1)*7)} = (1:257)+1542*1+257*(i-1); % depth
    features{10+((i-1)*7)} = (1:257)+1542*2+257*(i-1); % thermal
    features{11+((i-1)*7)} = [features{8+(i-1)*7} features{9+(i-1)*7}]; %cd
    features{12+((i-1)*7)} = [features{8+(i-1)*7} features{10+(i-1)*7}]; %ct
    features{13+((i-1)*7)} = [features{9+(i-1)*7} features{10+(i-1)*7}]; %dt
    features{14+((i-1)*7)} = [features{8+(i-1)*7} features{9+(i-1)*7} features{10+(i-1)*7}]; %cdt
end

acc = [];

for i = 1:size(features,2)
    
    fprintf('Feature set %d\n',i);

    dataset_1 = dataset_20130627(training_examples,features{i}+1);
    dataset_2 = dataset_20130628(testing_examples,features{i}+1);

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

    acc(i,:) = accuracy;
end

save results_modality_fov acc;

%%
load results_modality_fov;

bar(0:6,reshape(acc(:,1),7,7)'); legend('c','d','t','cd','ct','dt','cdt'); xlabel('resolution (pyramid level)'); ylabel('accuracy');title('modality');set(gca,'XTickLabel',{'all','240x180','120x90','60x45','30x22','15x11','7x5'});
