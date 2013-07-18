function dataset = load_dataset(dataset_path)

try
    load(strcat(dataset_path,'dataset'));
catch
    dataset = readxml_fast(sprintf('%sdataset.xml', dataset_path));
    save(strcat(dataset_path,'dataset'), 'dataset');
end

