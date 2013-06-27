% test reading opencv files


step = 0;
data_set = '..\\Debug\\data\\dataset_20130621T171958\\images\\';
%data_set = '..\\Debug\\data\\dataset_20130624T095243\\images\\';
C = imcrop(rgb2gray(imread(strcat(data_set,'thermal02050.png'))),[0 0 640 440]);
D = imcrop(rgb2gray(imread('..\\Debug\\data\\dataset_20130621T171958\\images\\thermal02050.png')),[0 0 640 440]);
CF = fft2(C);
CCF = log(abs(fftshift(CF)));

%for i = 1:1:200
LPF_ideal=lpfilter('ideal', size(C,1), size(C,2), 300);
HPF_ideal=hpfilter('ideal', size(C,1), size(C,2), 150);
filter = 1 - LPF_ideal.*HPF_ideal;
%filter = HPF_ideal;
FRES = CF.*filter;

RES = ifft2(FRES);

subplot(2,2,1), imshow(C);
subplot(2,2,2), imagesc(fftshift(filter));
subplot(2,2,3), imagesc(RES, [0 256]); colormap('gray');
%subplot(2,2,3), imagesc(RES, [0 10]); colormap('gray');
subplot(2,2,4), imagesc(CCF);
i
%pause(0.1)
%end

