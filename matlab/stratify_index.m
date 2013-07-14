function out_index = stratify_index(in_index, varargin)

%determine number of classes
classes = unique(in_index);
%count instances for all classes
h = hist(in_index,classes);

%either set the number of items per class to the minimum class count
%or value specified by the user
if nargin == 1
    minv = min(h);
else
    minv = varargin{1};
end

out_index = [];

%for each class find the new set of indexes at linearly spaced locations
for i = 1:length(classes)
    idx_out = find(in_index==classes(i));
    out_index = cat(1, out_index, idx_out(round(linspace(1, h(i), minv))));
end



