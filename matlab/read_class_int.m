function output = read_class_int(file_name)

output = [];

fid = fopen(file_name,'r');

while ~feof(fid)
    s = fgetl(fid);
    ind = sscanf(s, '%d %d-%d');    
    if length(ind) == 2
        ind = sscanf(s, '%d %d');
        output(ind(2)+1) = ind(1);
    else
        output((ind(2):ind(3))+1) = ind(1);
    end
end

fclose(fid);