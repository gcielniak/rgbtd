load 'D:\data\rgbdt\dataset_20130627T164440\dataset_20130627';

%filter
dataset = dataset_20130627(1:10:end,1:258);

%save
fid = fopen('dataset_20130627.txt','w');

for i=1:size(dataset,1)
    fprintf(fid,'%d ',dataset(i,1));    
    fprintf(fid,'%d:%f ',[1:(size(dataset,2)-1); dataset(i,2:end)]);    
    fprintf(fid,'\n');    
end

fclose(fid);
