function out = readxml_fast(filename)
%quick and dirty way of reading opencv generated files, good for large
% matrices

fid = fopen(filename);

%ignore first three rows
fgetl(fid); fgetl(fid); fgetl(fid);

%get row count
fscanf(fid, '%6s', 1); rows = fscanf(fid, '%d'); fgetl(fid);
%get col count
fscanf(fid, '%6s', 1); cols = fscanf(fid, '%d'); fgetl(fid);

%ignore further two rows
fgetl(fid);fgetl(fid);

%read and reshape matrix values
out= reshape(fscanf(fid, '%f '),cols,rows);

fclose(fid);


