%make test video

%modality analysis

dataset_20130627 = load_dataset('D:\\data\\rgbdt\\dataset_20130627\\');
dataset_20130628 = load_dataset('D:\\data\\rgbdt\\dataset_20130628\\');

training_examples = 1:size(dataset_20130627,1);
testing_examples = 1:size(dataset_20130628,1);

labels_1 = dataset_20130627(training_examples,1);
labels_2 = dataset_20130628(testing_examples,1);

dataset_1 = dataset_20130627(training_examples,2:end);
dataset_2 = dataset_20130628(testing_examples,2:end);

[dataset_1, minv, maxv] = scale_svm(dataset_1);
dataset_2 = scale_svm(dataset_2, minv, maxv);
dataset_1(isnan(dataset_1))=0;
dataset_2(isnan(dataset_2))=0;
    
fprintf('Training... ');
model_1 = svmtrain(labels_1,dataset_1,'-q');
fprintf('done\n');

fprintf('Testing... ');
[plabels_2, accuracy, dv] = svmpredict(labels_2,dataset_2, model_1,'-q');
fprintf('done\n');

fprintf('Training... ');
model_2 = svmtrain(labels_2,dataset_2,'-q');
fprintf('done\n');

fprintf('Testing... ');
[plabels_1, accuracy, dv] = svmpredict(labels_1,dataset_1, model_2,'-q');
fprintf('done\n');

save results_video plabels_1 labels_1 plabels_2 labels_2;

%%

load results_video;

plabels = plabels_1;
labels = labels_1;

label_text = {'office','staff area','corridor 1','corridor 2'};
label_color = {[0.2 1.0 0.2], [0.2 0.2 1.0], [1.0 0.2 0.2], [0.6 0.2 0.6]};

mkdir('images');
for i = 1:size(plabels,1)
    A = imread(sprintf('D:\\data\\rgbdt\\dataset_20130627\\images\\color%05d.png',i-1));
    B = imread(sprintf('D:\\data\\rgbdt\\dataset_20130627\\images\\depth%05d.png',i-1));
    C = imread(sprintf('D:\\data\\rgbdt\\dataset_20130627\\images\\thermal%05d.png',i-1));
    subplot(2,2,1); imshow(A); xlabel('color');
    subplot(2,2,2); imshow(B*6.5535); xlabel('depth');
    subplot(2,2,3); imshow(C); xlabel('thermal');
    subplot(2,2,4); imshow(ones(size(A))); xlabel('result');
    text(80,40,sprintf('frame %d',i),'Color',[0.2 0.2 0.2],'FontSize',8);
    text(60,340,sprintf('I think it is %s',label_text{plabels(i)}),'Color',label_color{plabels(i)},'FontSize',10);
    text(60,340+60,sprintf('The correct label is %s',label_text{labels(i)}),'Color',label_color{labels(i)},'FontSize',10);
    text(400,40,'color scheme:','Color',[0 0 0],'FontSize',8);
    for j=1:4
        text(400,40+j*40,label_text{j},'Color',label_color{j},'FontSize',8);
    end
    print(gcf, '-dpng', sprintf('images\\frame%05d.png',i));
    pause(0.01);
end
