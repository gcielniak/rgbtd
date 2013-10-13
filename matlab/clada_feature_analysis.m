load results_modality_ada;

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

%%
modality_labels = {'C','D','T'};
feature_labels = {'all','640x480','320x240','160x120','80x60','40x30','20x15','10x7','5x3'};
feature_cat = {'C','D','T','CD','CT','DT','CDT'};

range = 10;
for i = 1:size(ada,2)
    imp = ada{i}.predictorImportance;
    [y, ind] = sort(imp,'descend');
    modality = floor((features{i}(ind(1:range))-1)/257/8);
    level = mod(floor((features{i}(ind(1:range))-1)/257),8);
    number = mod((features{i}(ind(1:range))-1),257);
    
    fprintf('%8s',feature_labels{floor((i-1)/7)+1});
    fprintf(' %3s: ',feature_cat{mod(i-1,7)+1});
    for j = 1:range
        fprintf('%s', modality_labels{modality(j)+1});
        fprintf('%d', level(j));
        fprintf('_%03d ', number(j));
    end
    fprintf('\n');
    fprintf('             ');
    fprintf(' %6d', features{i}(ind(1:range))-1);
    fprintf('\n');
    if (mod(i,7)==0)
        fprintf('\n');
    end
end

%%

imp = ada{7}.predictorImportance;
[y, ind] = sort(imp,'descend');

modality = floor((features{7}(ind)-1)/257/8);
level = mod(floor((features{7}(ind)-1)/257),8);
type = floor((features{7}(ind)-1)/257);
subplot(3,2,1); hist(type(1:50),24), title('first 50');
subplot(3,2,3); hist(type(1:100),24), title('first 100');
subplot(3,2,5); hist(type(1:200),24), title('first 200');

type_hist_weighted = [];

for i = 1:24
    type_hist_weighted(i) = sum(y(find(type==(i-1))));
end

subplot(3,2,6), bar(type_hist_weighted), title('all weighted');

type_hist_weighted = [];

for i = 1:24
    type_hist_weighted(i) = sum(y(find(type(1:50)==(i-1))));
end

subplot(3,2,2), bar(type_hist_weighted), title('first 50 weighted');

for i = 1:24
    type_hist_weighted(i) = sum(y(find(type(1:100)==(i-1))));
end

subplot(3,2,4), bar(type_hist_weighted), title('first 100 weighted');




