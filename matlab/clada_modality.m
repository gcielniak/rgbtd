%modality analysis

dataset_20130627 = load_dataset('D:\\data\\rgbdt\\dataset_20130627\\');
dataset_20130628 = load_dataset('D:\\data\\rgbdt\\dataset_20130628\\');

training_examples = 1:size(dataset_20130627,1);
testing_examples = 1:size(dataset_20130628,1);

labels_1 = dataset_20130627(training_examples,1)+1;
labels_2 = dataset_20130628(testing_examples,1)+1;

%prepare feature combinations representing different modalities
features = [];

%combination of features across the levels
features{1} = (1:2056)+2056*0; % color
features{2} = (1:2056)+2056*1; % depth
features{3} = (1:2056)+2056*2; % thermal
features{4} = [features{1} features{2}]; %cd
features{5} = [features{1} features{3}]; %ct
features{6} = [features{2} features{3}]; %dt
features{7} = 1:6168; % all features

%combination of features on different pyramid level
for i=1:8
    features{8+((i-1)*7)} = (1:257)+2056*0+257*(i-1); % color
    features{9+((i-1)*7)} = (1:257)+2056*1+257*(i-1); % depth
    features{10+((i-1)*7)} = (1:257)+2056*2+257*(i-1); % thermal
    features{11+((i-1)*7)} = [features{8+(i-1)*7} features{9+(i-1)*7}]; %cd
    features{12+((i-1)*7)} = [features{8+(i-1)*7} features{10+(i-1)*7}]; %ct
    features{13+((i-1)*7)} = [features{9+(i-1)*7} features{10+(i-1)*7}]; %dt
    features{14+((i-1)*7)} = [features{8+(i-1)*7} features{9+(i-1)*7} features{10+(i-1)*7}]; %cdt
end

acc = [];
ada = [];

for i = 1:size(features,2)
    
    fprintf('Feature set %d\n',i);

    dataset_1 = dataset_20130627(training_examples,features{i}+1);
    dataset_2 = dataset_20130628(testing_examples,features{i}+1);

    fprintf('Training...\n')
    ada{i} = fitensemble(dataset_1,nominal(labels_1),'AdaBoostM2',100,...
        ClassificationTree.template('MinLeaf',size(dataset_1,1)/50));
    
    fprintf('Testing...\n')
    output = predict(ada{i},dataset_2);
    
    acc(i,:) = 1 - length(find(labels_2-double(output)))/length(labels_2);
end

save('results_modality_ada','acc','ada','-v7.3');

%%
load results_modality_ada;

bar(0:8,reshape(acc(:,1),7,9)'); legend('c','d','t','cd','ct','dt','cdt'); xlabel('resolution (pyramid level)'); ylabel('accuracy');title('modality');set(gca,'XTickLabel',{'all','640x480','320x240','160x120','80x60','40x30','20x15','10x7','5x3'});
