%function [] = readCSV(sourceDataPath,dataPath)
%%
clc; close all; clear;
% csvname = './CSI1.csv';
disp(csvname);
csi_raw = readmatrix(csvname);

clip = find(csi_raw(:, 1));
csi_clip = csi_raw(clip(1):end, :);
disp(length(find(csi_clip==csi_clip(1, 65))));

a_nan=sum(sum(isnan(csi_clip), 2)~=0);

m = size(csi_clip, 1);
n = size(csi_clip, 2);
new_csi = zeros(m, n);

%%
[col, row, ~] = find(csi_clip'>10000);
row = row(col<n);
col = col(col<n);
csi_clip(row, end) = csi_clip(sub2ind([m n], row, col));
csi_clip(sub2ind([m n], row, col)) = NaN;

%%
csi_wo = csi_clip(:, 1:end-1);
csi_wo = reshape(csi_wo', 1, []);
csi_wo = rmmissing(csi_wo);

%%
count = 0;
discard_count = 0;
discard = false;
i = n-6;
while i < length(csi_wo)
    if all(csi_wo(i:i+5) == csi_clip(1, 65:70))
        if discard == true
            discard_count = discard_count+1;
            discard = false;
        end
        count = count+1;
        new_csi(count, 1:n-1) = csi_wo(i-64:i+5);
        new_csi(count, n) = csi_clip(count+discard_count, end);
        i = i+70;
    else
        discard = true;
        i = i+1;
    end
end

%%
data_mac_time = new_csi(1:count, :);
data_mac_time(all(data_mac_time==0,2),:)=[];
data = data_mac_time(:,[3:27 39:64 71]);
% data(:,27) = [];
data(all(data==0,2),:)=[];
for i=1:length(data)%remove overlarge point, here is 35
    for j=1:51
        if data(i,j)>35
            data(i,j)=0;
        end
    end
end
data(all(data(:,1:51)==0,2),:)=[];

%remove outlier
for p=1:51
    data_1(:,p) = rmoutliers(data(:,p),'movmean',55);
end

%butter filter
for p = 1:51
    num_RSS = data_1(:,p);
    [B,A] = butter(5,0.05,'low');
    RSS_butter(:,p) = filter(B,A,num_RSS);
end
%wavelet滤波
for p = 1:51
    [c,s]=wavedec2(data_1(:,p),4,'db15');
    wavelet = appcoef2(c,s,'db15',4);
    RSS_wave(:,p) = wavelet(:,1);
end
plot(RSS_butter(:,5));
figure;
plot(RSS_butter(:,15));
figure;
plot(RSS_butter(:,25));
figure;
plot(RSS_butter(:,27));
figure;
plot(RSS_butter(:,35));
figure;
plot(RSS_butter(:,45));


