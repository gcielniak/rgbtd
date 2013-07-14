function [output,varargout] = scale_svm(input, varargin)
% scale columns to [-1, 1] range

if nargin == 3
    minv = varargin{1}; % min vector
    maxv = varargin{2}; % max vector        
elseif nargin == 1
    minv = min(input); % min vector
    maxv = max(input); % max vector        
else
    error('scale_svm', 'Wrong number of input arguments');
end

varargout{1} = minv;
varargout{2} = maxv;

normvm = maxv - minv; % norm vector
normvm = repmat(normvm, [size(input,1) 1]); %change into matrix form
minvm = repmat(minv, [size(input,1) 1]); %change into matrix form

% scale to [0, 1]
output = input - minvm;
output(find(output<0))=0;
output = output./normvm;
output(find(output>1))=1;

%scale to [-1, 1]
output = output*2 - 1;
