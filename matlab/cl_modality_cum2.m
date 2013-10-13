%modality analysis

dataset_20130627 = load_dataset('D:\\data\\rgbdt\\dataset_20130627\\');
dataset_20130628 = load_dataset('D:\\data\\rgbdt\\dataset_20130628\\');

training_examples = 1:size(dataset_20130627,1);
testing_examples = 1:size(dataset_20130628,1);

labels_1 = dataset_20130627(training_examples,1);
labels_2 = dataset_20130628(testing_examples,1);

%prepare feature combinations representing different modalities
features = [];

%cumulative combination of features up to a certain pyramid level ascending
for i=1:8
    offset = (i-1)*7;
    features{1+offset} = (1:257*i)+(8-i)*257+2056*0; % color
    features{2+offset} = (1:257*i)+(8-i)*257+2056*1; % depth
    features{3+offset} = (1:257*i)+(8-i)*257+2056*2; % thermal
    features{4+offset} = [features{1+offset} features{2+offset}]; %cd
    features{5+offset} = [features{1+offset} features{3+offset}]; %ct
    features{6+offset} = [features{2+offset} features{3+offset}]; %dt
    features{7+offset} = [features{1+offset} features{2+offset} features{3+offset}]; %cdt
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

save results_modality_cum2 acc;

%%
load results_modality_cum2;

bar(0:7,reshape(acc(:,1),7,8)'); legend('c','d','t','cd','ct','dt','cdt'); xlabel('resolution (pyramid level)'); ylabel('accuracy');title('cumulative modality');set(gca,'XTickLabel',{'5x3','10x7','20x15','40x30','80x60','160x120','320x240','640x480 (all)'});
