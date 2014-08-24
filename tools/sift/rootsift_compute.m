function features_result = rootsift_compute(features)
%% Compute rootSIFT from sift features

%features: sift features data

%return:
% - features_result: rootSIFT features

    fprintf('Computing rootSIFT features:\n');
    num_features = size(features, 2);
    %rootSIFT = zeros(dim, num_features);

    matlabpool('open',4);
    for k = 1:5000000:num_features
        eIdx = k+5000000-1;
        if eIdx > num_features
            eIdx = num_features;
        end
        parfor i=k:eIdx
           features(:, i) = sqrt(features(:, i) / sum(features(:,i)));
        end
    end
    matlabpool close;
    features_result = features;
end