function dataset = load_dataset(dataset_path)

try
    load(strcat(dataset_path,'dataset'));
catch
    dataset = readxml_fast(sprintf('%sfeatures.xml', dataset_path));
    classes = readclass(sprintf('%sclasses.txt', dataset_path));
    dataset = [classes' dataset];
    save(strcat(dataset_path,'dataset'), 'dataset');
end

