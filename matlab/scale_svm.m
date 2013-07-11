function output = scale_svm(input)
% scale columns to [-1, 1] range

min_v = min(input); % min vector
max_v = max(input); % max vector

norm_v = max_v - min_v; % norm vector
norm_v = repmat(norm_v, [length(input) 1]); %change into matrix form
min_v = repmat(min_v, [length(input) 1]); %change into matrix form

% scale to [0, 1]
output = input - min_v;
output = output./norm_v;
%scale to [-1, 1]
output = output*2 - 1;