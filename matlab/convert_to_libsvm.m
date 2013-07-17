path = 'D:\\data\\rgbdt\\dataset_20130628\\';
dataset = load_dataset(path);
%save
fid = fopen(strcat(path,'dataset_libsvm.txt'),'w');

for i=1:size(dataset,1)
    fprintf(fid,'%d ',dataset(i,1));    
    fprintf(fid,'%d:%f ',[1:(size(dataset,2)-1); dataset(i,2:end)]);    
    fprintf(fid,'\n');    
end

fclose(fid);
