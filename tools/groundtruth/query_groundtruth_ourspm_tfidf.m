function time = query_groundtruth_ourspm_tfidf(dict_words, inv_file, dict_structure, groundtruthDir, datasetDir, pparams)
%% Query groundtruth images: compute mean Average Precision (mAP) for dataset

% dict_words:               dictionary
% inv_file:                 inverted file index 
% dict_structure:           dict structure
% datasetDir:               dataset directory

%returns: after ranking, this function will run script to compute mAP from result directory

    fprintf('query...');
    verbose = 0;
    files = dir(fullfile(datasetDir, '*.jpg'));
    q_files = dir(fullfile(groundtruthDir, '*query.txt'));
    disp(length(q_files));
    %ntop = 0;
    
    %k = 0;
    % load query image
    %while k >= 0
    time = zeros(1,length(q_files));
    for k = 1:length(q_files)
        %k = input('Nhap k: \n');
        %% extract query info
        fprintf('\nQuerying file: %s\n',q_files(k).name);
        [image_name, points] = groundtruth_extractinfo(strcat(groundtruthDir, q_files(k).name));
        file = strcat(datasetDir, image_name(6:end), '.jpg');
        
        %% compute rootSIFT
        I = im2single(rgb2gray(imread(file)));
        [h, w, ~] = size(I);
        [root_sift, coords] = compute_rootsift_image(I, points);
        
        %% compute words
        q_words = cell(1,1);
        q_words{1} = ccvBowGetWords(dict_words, root_sift, [], dict_structure);
        
        % extract spatial pyramid
        q_words_spm =  pyramid_extract_word(q_words{1},coords,w,h,pparams);
        
        %% searching candidates from inverted file index and ranking
        tic;
        [ranked_ids, ~] = inv_search_spm_c(inv_file,q_words_spm);
        time(k) = toc;
        fprintf('size: %d\n',length(ranked_ids));
        
        %ranked_ids = indexes{1};
        
        %% visualize
        if verbose ==1
            close all;
            hold on; subplot(3,5,3); imshow(I);
            title(image_name(1:end));
        end
        
        fid = fopen(strcat(groundtruthDir, 'rank_list.txt'), 'wt');
        for i=1:size(ranked_ids,2)
           %i
                    % Show only 10 highest score images
            if verbose == 1 && i<=10
                %i
                subplot(3, 5, 5+i);
                %fprintf('%s\n',files(ranked_ids(i)).name);
                imshow(imread(strcat(datasetDir, files(ranked_ids(i)).name)));
                title(files(ranked_ids(i)).name);    
            end
            name = files(ranked_ids(i)).name(1:end-4);
            fprintf(fid, '%s\n', name);
        end
        fclose(fid);
        
        %fprintf('name: %s\n image_name: %s',q_files(k).name(1:end-10), image_name(6:end));
        %% run script to compute mAP
        fprintf('computing mAP...\n');
        script_file = sprintf('%sTest.exe %s', groundtruthDir, groundtruthDir);
        script = [script_file, ...
            q_files(k).name(1:end-10), ... %q_files(k).name(1:end-10)
            sprintf(' %srank_list.txt', groundtruthDir),...
            ' >result\oxford5k\new-propose-tfidf-nhi\ourspm-tfidf-nhi-lv1\', image_name(6:end), '_result.txt']; %q_files(k).name(1:end-10)
        system(script);
        if verbose==1
            pause;
        end
        
    end
    % clear inv file
    %ccvInvFileClean(inv_file);
end