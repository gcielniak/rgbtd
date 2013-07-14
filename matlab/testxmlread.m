data_set = 'D:\\data\\rgbdt\\dataset_20130627T164440\\';

classes = read_class_int(sprintf('%sclasses.txt', data_set));

fid = fopen(sprintf('%sfeatures.xml', data_set));

tic;

step = 0;

while ~feof(fid) && step < 100
    step = step + 1;
    line = fgetl(fid);
    size(line)
end

toc;