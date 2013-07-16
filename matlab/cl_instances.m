%modality analysis

dataset_20130627 = load_dataset('D:\\data\\rgbdt\\dataset_20130627T164440\\');
dataset_20130628 = load_dataset('D:\\data\\rgbdt\\dataset_20130628T150841\\');

training_examples = 1:1:size(dataset_20130627,1);
testing_examples = 1:1:size(dataset_20130628,1);

labels_1 = dataset_20130627(training_examples,1);
labels_2 = dataset_20130628(testing_examples,1);

%%
%prepare feature combinations representing different modalities
features = [];

features{1} = (1:2056)+2056*0; % color
features{2} = (1:2056)+2056*1; % depth
features{3} = (1:2056)+2056*2; % thermal
features{4} = [features{1} features{2}]; %cd
features{5} = [features{1} features{3}]; %ct
features{6} = [features{2} features{3}]; %dt
features{7} = 1:6168; % all features

acc = [];
result_labels = [];

for i = 1:size(features,2)
    
    fprintf('Feature set %d\n',i);

    dataset_1 = dataset_20130627(training_examples,features{i}+1);
    dataset_2 = dataset_20130628(testing_examples,features{i}+1);

    [dataset_1, minv, maxv] = scale_svm(dataset_1);
    dataset_2 = scale_svm(dataset_2, minv, maxv);
    dataset_1(isnan(dataset_1))=0;
    dataset_2(isnan(dataset_2))=0;
    if isinf(dataset_1) or isinf(dataset_2)
        error 'Inf detected';
    end
    
    fprintf('Training... ');
    model = svmtrain(labels_1,dataset_1,'-q');
    fprintf('done\n');

    fprintf('Testing... ');
    [p_label, accuracy, dv] = svmpredict(labels_2,dataset_2, model,'-q');
    fprintf('done\n');

    acc(i,:) = accuracy;
    result_labels(i,:) = p_label;
end

save results_instances acc result_labels labels_2;

%%
load results_instances;

figure('name','classification results','numbertitle','off');

len = size(result_labels,2);
subplot(1,2,1);
plot3(ones(1,len)*0,1:len,labels_2,...
    ones(1,len)*1,1:len,result_labels(1,1:len)',...
    ones(1,len)*2,1:len,result_labels(2,1:len)',...
    ones(1,len)*3,1:len,result_labels(3,1:len)',...
    ones(1,len)*4,1:len,result_labels(4,1:len)',...
    ones(1,len)*5,1:len,result_labels(5,1:len)',...
    ones(1,len)*6,1:len,result_labels(6,1:len)',...
    ones(1,len)*7,1:len,result_labels(7,1:len)');axis([0 7 0 len 1 4.2]);
zlabel('class');
title('classification');
set(gca,'XTickLabel',{'gt','color','depth','thermal','c+d','c+t','d+t','c+d+t'});
view(gca,[-90 82]);

subplot(1,2,2);
plot3(ones(1,len)*0,1:len,abs(labels_2-labels_2),...
    ones(1,len)*1,1:len,abs(result_labels(1,1:len)'-labels_2),...
    ones(1,len)*2,1:len,abs(result_labels(2,1:len)'-labels_2),...
    ones(1,len)*3,1:len,abs(result_labels(3,1:len)'-labels_2),...
    ones(1,len)*4,1:len,abs(result_labels(4,1:len)'-labels_2),...
    ones(1,len)*5,1:len,abs(result_labels(5,1:len)'-labels_2),...
    ones(1,len)*6,1:len,abs(result_labels(6,1:len)'-labels_2),...
    ones(1,len)*7,1:len,abs(result_labels(7,1:len)'-labels_2));axis([0 7 0 len 0 3]);
zlabel('class');
title('classification errors');
set(gca,'XTickLabel',{'','color','depth','thermal','c+d','c+t','d+t','c+d+t'});
view(gca,[-90 82]);

mrows = 7;
mcols = 10;

figure('name','difficult color examples','numbertitle','off');

cl = 1;
diff_ind = find((result_labels(cl,:)'-labels_2));
im_ind = diff_ind(floor(linspace(1,size(diff_ind,1),mrows*mcols)));

for i=1:mrows*mcols
    subplot(mrows, mcols, i);
    imshow(imread(sprintf('D:\\data\\rgbdt\\dataset_20130628T150841\\images\\color%05d.png',im_ind(i)-1)));
    xlabel(sprintf('%d(%d)',result_labels(1,im_ind(i)),labels_2(im_ind(i))));
end

figure('name','difficult depth examples','numbertitle','off');

cl = 2;
diff_ind = find((result_labels(cl,:)'-labels_2));
im_ind = diff_ind(floor(linspace(1,size(diff_ind,1),mrows*mcols)));

for i=1:mrows*mcols
    subplot(mrows, mcols, i);
    imshow(imread(sprintf('D:\\data\\rgbdt\\dataset_20130628T150841\\images\\depth%05d.png',im_ind(i)-1))*6.55);
    xlabel(sprintf('%d(%d)',result_labels(cl,im_ind(i)),labels_2(im_ind(i))));
end

figure('name','difficult thermal examples','numbertitle','off');

cl = 3;
diff_ind = find((result_labels(cl,:)'-labels_2));
im_ind = diff_ind(floor(linspace(1,size(diff_ind,1),mrows*mcols)));

for i=1:mrows*mcols
    subplot(mrows, mcols, i);
    imshow(imread(sprintf('D:\\data\\rgbdt\\dataset_20130628T150841\\images\\thermal%05d.png',im_ind(i)-1)));
    xlabel(sprintf('%d(%d)',result_labels(cl,im_ind(i)),labels_2(im_ind(i))));
end

figure('name','difficult combined examples','numbertitle','off');

cl = 7;
diff_ind = find((result_labels(cl,:)'-labels_2));
im_ind = diff_ind(floor(linspace(1,size(diff_ind,1),mrows*mcols)));

for i=1:mrows*mcols
    subplot(mrows, mcols, i);
    imshow(imread(sprintf('D:\\data\\rgbdt\\dataset_20130628T150841\\images\\color%05d.png',im_ind(i)-1)));
    xlabel(sprintf('%d(%d)',result_labels(cl,im_ind(i)),labels_2(im_ind(i))));
end





