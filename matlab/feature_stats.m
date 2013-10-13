%feature statistics

load '..\data\dataset_20130627';
load '..\data\dataset_20130628';

dataset = dataset_20130627(:,2:end);

min_d = min(dataset);
max_d = max(dataset);

diff = max_d-min_d;

mpt = [];

for i=1:3
    for j =1:8
        ii = j+(i-1)*8;
        mpt(i,j) = length(find(diff((ii-1)*257+1:(ii-1)*257+257)==0));
    end
end
