step = 0;
data_set = '..\\data\\data_set_1\\';

classes = read_class_int(sprintf('%sclasses.txt', data_set));

%while 1
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
    
    class;
    
    subplot(1,3,1); stem(A);
    subplot(1,3,2); stem(B);
    subplot(1,3,3); stem(C);
    step = step + 1;
    pause(0.1);
end