dataset_name = 'oscar_split';

dataset = load_dataset(strcat('D:\\data\\nexus4\\',dataset_name,'\\'));

%save
fid = fopen(strcat(dataset_name,'_raw.txt'),'w');

for i=1:size(dataset,1)
    fprintf(fid,'%d ',dataset(i,1));    
    fprintf(fid,'%f ',dataset(i,2:end));    
    fprintf(fid,'\n');    
end

fclose(fid);
