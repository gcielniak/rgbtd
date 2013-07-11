function output = remove_nan(input)
output = input;
output(:, all(isnan(output), 1)) = [];