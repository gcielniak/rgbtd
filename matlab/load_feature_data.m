step = 0;
data_set = 'D:\\data\\rgbdt\\dataset_20130627T164440\\';

classes = read_class_int(sprintf('%sclasses.txt', data_set));
my_data = [];

while 1
    file_name_color = sprintf('%sfeatures\\color%05d.xml', data_set, step);
    file_name_depth = sprintf('%sfeatures\\depth%05d.xml', data_set, step);
    file_name_thermal = sprintf('%sfeatures\\thermal%05d.xml', data_set, step);
   
    if isempty(classes)
        file_name_class = sprintf('%sclasses\\class%05d.txt', data_set, step);
        if ~exist(file_name_class,'file')
            break;
        end
        class = read_class(file_name_class);    
    elseif step < length(classes)
        class = classes(step+1);
    else
        break;
    end
    
    
    if ~exist(file_name_color,'file') | ~exist(file_name_depth,'file') | ~exist(file_name_thermal,'file')
        break;
    end
    
    A = opencv_read(file_name_color);
    B = opencv_read(file_name_depth);
    C = opencv_read(file_name_thermal);
    
    step = step + 1;
    
    my_data(step,:) = [class; A; B; C]';
    fprintf('step %d\n', step);
end