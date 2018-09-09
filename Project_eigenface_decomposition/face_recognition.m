%% Reading Images
trainingFolder = 'faces96';
folderList = dir(trainingFolder);
folderList = folderList(3:length(folderList));

%% Creating training and testing vectors
current_folder = strcat(trainingFolder,'/',folderList(1).name);
pgmFiles = dir(sprintf('%s/*.jpg',current_folder));
%pgmFiles = pgmFiles(3:length(pgmFiles));

% read an image to determine dimensions
im = imread(strcat(current_folder,'/',pgmFiles(1).name));
H = size(im,1); %Height of the images
W = size(im,2); %Width of the images
C = size(im,3); %Channels
M = size(pgmFiles,1); %Number of images

% split into training and testing set
training_size = int16(15);
testing_size = int16(M-training_size);

% allocate arrays
training_images = zeros(H,W,C,length(folderList)*training_size);
testing_images = zeros(H,W,C,length(folderList)*testing_size);
training_vec = zeros(H*W*C,length(folderList)*training_size);
testing_vec = zeros(H*W*C,length(folderList)*testing_size);

j = 1;
k = 1;

%global_testing_idx = zeros(length(folderList)*testing_size,1);
%relative_testing_idx = zeros(length(folderList)*testing_size,1);
%global_training_idx = zeros(length(folderList)*training_size,1);
%relative_training_idx = zeros(length(folderList)*training_size,1);
global_testing_idx = [];
relative_testing_idx = [];
global_training_idx = [];
relative_training_idx = [];

for folder_idx = 1:length(folderList)
    % find the training files
    current_folder = strcat(trainingFolder,'/',folderList(folder_idx).name);
    pgmFiles = dir(sprintf('%s/*.jpg',current_folder));
    if(length(pgmFiles) == 0)
        continue;
    end
    %pgmFiles = pgmFiles(3:length(pgmFiles));

    M = size(pgmFiles,1); %Number of images
    training_size = int16(15);
    testing_size = int16(M-training_size);

    training_idx = sort(randperm(M,training_size)');

    for i = 1:M
        if(j <= length(folderList)*training_size && 1 == ismember(i,training_idx))
            % create training vectors
            global_training_idx(j) = M*folder_idx + i;
            relative_training_idx(j) = i;
            training_images(:,:,:,j) = imread(strcat(current_folder,'/',pgmFiles(i).name));
            training_vec(:,j) = reshape(training_images(:,:,:,j),H*W*C,1);
            j = j+1;
        else
            % create testing vectors
            global_testing_idx(k) = M*folder_idx + i;
            relative_testing_idx(k) = i;
            %fprintf('%d', testing_idx(k));
            testing_images(:,:,:,k) = imread(strcat(current_folder,'/',pgmFiles(i).name));
            testing_vec(:,k) = reshape(testing_images(:,:,:,k),H*W*C,1);
            k = k+1;
        end
    end
    disp(['Image vectors created for face: ' num2str(folder_idx) ' of ' num2str(length(folderList))]);
end

%% Training
training_size = j-1;
training_vec = training_vec(:,1:training_size);
% mean face
m = sum(training_vec,2)/double(training_size);

% face space
A = training_vec - repmat(m,1,training_size);
[V,lambda] = eig(A'*A);

% eigenvector of the covariance matrix of A. These are the eigenfaces
U = A*V;

% projection of each vector in the face space A on the eigenfaces
omega = U'*A;
disp(['Training done.']);

mean_face = reshape(m,H,W,C);
figure;
subplot(1,1,1);
imshow(uint8(mean_face));
title('Average "ghost face"')

[V_top3,D] = eigs(A'*A,3);
U_top3 = A*V_top3;
figure;
subplot(1,3,1);
imshow(uint8(reshape(U_top3(:,1),H,W,C)));
title('1st Eigen face');
subplot(1,3,2);
imshow(uint8(reshape(U_top3(:,2),H,W,C)));
title('2nd Eigen face');
subplot(1,3,3);
imshow(uint8(reshape(U_top3(:,3),H,W,C)));
title('3rd Eigen face');

%% Testing
testing_size = k-1;
acc = 0;
for i = 1:testing_size
    disp(['Test Image: ' num2str(i) ' of ' num2str(testing_size)]);
    % subtract mean face
    imd = testing_vec(:,i) - m;
    
    % projection of the test face on the eigenfaces
    om = U'*imd;
    d = repmat(om,1,training_size) - omega;
    
    % find the distance from all training faces
    dist = zeros(training_size,1);
    for j = 1:training_size
        dist(j,1) = norm(d(:,j));
    end

    % index corresponding to the minimum of the distances
    index = min_index(dist);
    
    % extract title of predicted and actual face images
    %fprintf('%d',testing_idx(i));
    predicted_face = strsplit(folderList(int16(floor((global_training_idx(index)-relative_training_idx(index))/M))).name, '.');
    actual_face = strsplit(folderList(int16(floor((global_testing_idx(i)-relative_testing_idx(i))/M))).name, '.');
    
    % if titles match, face recognized correctly
    if(strcmp(char(predicted_face(1)), char(actual_face(1))))
        acc = acc+1;
    end
end
acc = acc/double(testing_size)*100;
fprintf('\n');
disp(['Testing Accuracy = ' num2str(acc)  ' %']);


