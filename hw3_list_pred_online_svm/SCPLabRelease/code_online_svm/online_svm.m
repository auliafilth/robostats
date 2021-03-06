function [accuracy, misclass] = online_svm(class1, class2, iter, ...
                                           class1_name, class2_name)
    label_col = 5;
    feat_col_start = 6;
    
    lambda = 0.1;  % regularization 
    eta = 0.5;     % for learning rate later

    num_class1 = size(class1,1);
    num_class2 = size(class2,1);

    % combine two classes
    pos_ones = ones(num_class1,1);     % make label +1
    neg_ones = -1*ones(num_class2,1);    % make label -1
    tmp_class1 = class1;
    tmp_class1(:,label_col) = pos_ones;
    tmp_class2 = class2;
    tmp_class2(:,label_col) = neg_ones;
    two_classes = [tmp_class1; tmp_class2];
    size(two_classes); 
    % now 1st column is label, 2-11 are features

    num_rows = size(two_classes,1);
    myperm = randperm(num_rows);    % to permute the rows of the classes

    % Extract features columns of the combined,permutated classes and also
    % extract the corresponding label to another vector
    perm_classes = two_classes(myperm,:);
    perm_labels = perm_classes(:,label_col);         % extract the label
    perm_feat = perm_classes(:,feat_col_start:end);  % extract the features

    num_features = size(perm_feat,2);
    wts = ones(1,num_features);
    
    num_err = zeros(num_rows,1);
    err_rate = zeros(num_rows,1);
    predictions = zeros(num_rows,1);
    pred_err = 0;
    % Train
    for i=1:num_rows
        alpha = eta / sqrt(i);  % learning rate
        
        pred = wts * perm_feat(i,:)';
        if sign(pred) ~= perm_labels(i)
            pred_err = pred_err + 1;
        end
        predictions(i) = pred;
        num_err(i) = pred_err;
        err_rate(i) = pred_err / i;
        
        % y * w * f
        svm_update = pred * perm_labels(i); 
        if ((1 - svm_update) > 0)
            delta_l = lambda / num_rows * wts - perm_labels(i) * perm_feat(i,:);
        else
            delta_l = lambda / num_rows * wts;
        end
        wts = wts - alpha * delta_l;  % 1 x num_features
    end        

    predictions(predictions<0) = -1;
    predictions(predictions>=0) = 1;

    misclass = pred_err / num_rows;
    accuracy = 1-misclass;
        
    fname = sprintf('online_svm_log_%d.mat', iter);
    save(fname,'predictions','perm_classes','perm_labels','perm_feat', ...
        'err_rate', 'pred_err', 'num_rows', 'wts', 'class1_name', ...
        'class2_name');    
end