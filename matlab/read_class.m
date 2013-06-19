function output = read_class(file_name)

fid = fopen(file_name,'r');

output = fscanf(fid,'%d');

fclose(fid);