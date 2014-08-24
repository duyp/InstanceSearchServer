function [dict_words, dict_structure, words, inv_file] = main()
    clear all; close all;
    %% init parameter
    addpath('AKM');
    run('vl_setup.m');
    fprintf('RUNNING.....\n');
    
    datasetDir =    'C:\instance-search-data\dataset\oxford5k\oxford5k-images\';
    featureDir =    'C:\instance-search-data\features\';
    dictionaryDir = 'C:\instance-search-data\dictionaries\';
    wordDir =       'C:\instance-search-data\words\';
    
    isComputeSIFT = 0;
    isLoadSIFT = 1;
    isComputeRootSIFT = 1;
    num_words = 500;
    num_iterations = 5;
    num_trees = 8;
    dim = 128;
    
    end_name = '_5all';
    
    features_name = 'feature-oxford5k';
    
    dict_name = strcat('dict_oursift_',num2str(num_iterations), '_',num2str(num_words));
    coors_name = strcat('coords_oursift_',num2str(num_iterations), '_',num2str(num_words));
    words_name = strcat('words_oursift_',num2str(num_iterations), '_',num2str(num_words));
    %words_name = strcat(words_name, end_name);
    %coords_name = strcat(coords_name, end_name);
    
    files = dir(fullfile(datasetDir, '*.jpg'));
    num_images = length(files);
    
    dict_params =  {num_iterations, 'kdt', num_trees};
    
    %% Compute SIFT features
    if isComputeSIFT == 1
        [features, features_per_image, coords] = sift_compute_all(datasetDir, featureDir, features_name);
    elseif isLoadSIFT == 1
        [features, features_per_image, coords] = sift_load(featureDir, features_name);
    end
    
    %% Compute rootSIFT
    if isComputeRootSIFT
        features = rootsift_compute(features);
    end
    
    if 1
    %% Buid dictionary by running AKM
    dict_file = strcat(dictionaryDir, dict_name,'.mat');

    if exist(dict_file, 'file')
        dict_words = dict_load(dict_file);
    else
        dict_words = dict_build(dict_file, features, num_words, dict_params);
    end

    %% Compute sparse frequency vector
    words_file = strcat(wordDir, words_name,'.mat');
    coords_file = strcat(wordDir, coors_name,'.mat');
    if exist(words_file, 'file')
        [words, dict_structure] = words_load(words_file, dict_words, dict_params);
    else
        [words, dict_structure] = words_compute(words_file, coords_file, ...
                                                features, features_per_image, coords, ...
                                                dict_words, dict_params, num_images);
    end

    %% Create inverted file
    %inv_file = invfile_creat(words, num_words);
    
    %% Query
    %words_removed_file = strcat(featureDir,words_name,'_removed.mat'); 
    %load(words_removed_file);
    %query_groundtruth(dict_words, inv_file, dict_structure, datasetDir);
        
    end
end

