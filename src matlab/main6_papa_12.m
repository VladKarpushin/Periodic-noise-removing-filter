% 2016-01-04 
% реализован алгоритм удаления трафаретной сетки (растра) при газетной печати
% background cell removing algorithm
% 2018-08-17
% added user interactivness

close all,clc,clear all;

strFolder = 'D:\home\programming\vc\new\6_My home projects\13_Periodic_noise_removing_filter\';
strFileName = strcat(strFolder,'input\papa_1.bmp');

imgA = imread(strFileName);
[h w c] = size(imgA);
if c == 3
    imgA = rgb2gray(imgA);
end

imgA = imadjust(imgA);
figure, imshow(imgA);
title('Original image');

%****************************
%*****PSD calculation********
%****************************

img_fft = fft2(imgA);           %spectrum 
img_fft(1,1) = 0;               %removing of constant component
imgB = img_fft.*conj(img_fft);  %Power spectrum density
imgC = fftshift(255*(imgB -min(min(imgB))) /(max(max(imgB)) - min(min(imgB))));

figure; imshow(imgC);
title('Power spectrum density');

% user input:
%4-element vector of the form [xmin ymin width height]. The initial size of the rectangle is width-by-height pixels. The upper-left corner of the rectangle is at the (x,y) coordinate (xmin,ymin).
h = imrect();
pos = getPosition(h);       

[h w] = size(imgC);
imgD = ones([h w]);

%Array of noise. Enough first eight components
% x1 = [1620 1618 1211 1212 1416 1822 1414 1007 1409 1620 1726 1420 1212 1105 1622 1824 2028 2026 1821 1618 1208 1007 805  806 1010 1215];
% y1 = [ 912 1222 1221  911 757  1068 1377 1066 890  1009 1068 1242 1123 1064 602  757  914  1225 1378 1532 1532 1375 1220 907 754  602];

xmin = pos(1);
ymin = pos(2);
width = pos(3);
height = pos(4);

imgD(ymin:ymin+height, xmin:xmin+width) = 0; %imgD(row, colomn), this is why imgD(y,x) rather than (x,y)
figure; imshow((imgC+imgD)/2);
title('Power spectrum density + mask');

%**********************
%*****Filtering********
%**********************

imgE = ifft2(fftshift(imgD).*img_fft); %filtering
max(max(abs(real(imgE))))
max(max(abs(imag(imgE))))
imgE = real(imgE);  %it needs to get real part because imgE is complex value
imgE = uint8(255*(imgE - min(min(imgE))) /(max(max(imgE)) - min(min(imgE))));
imgE = imadjust(imgE);

%*******************
%*****Output********
%*******************


figure; imshow(imgE);
title('Final image');

figure, imshowpair(imgA, imgE,'montage');
imwrite(imgA,strcat(strFolder,'output\input.bmp'));
imwrite(imgE,strcat(strFolder,'output\output.bmp'));
imwrite(imgC,strcat(strFolder,'output\Power_spectrum_density.bmp'));