% 2016-01-04 
% реализован алгоритм удаления трафаретной сетки (растра) при газетной печати
% background cell removing algorithm
% 2018-08-17
% added user interactivness

close all,clc,clear all;

strFolder = 'D:\home\programming\vc\new\6_My home projects\13_Periodic_noise_removing_filter\';
strFileName = strcat(strFolder,'input\papa_2.bmp');
%strFileName = strcat(strFolder,'input\output_2.bmp');
%strFileName = strcat(strFolder,'input\1.bmp');

imgA = imread(strFileName);
%imgA = imresize(imgA, 0.35);
%imgA = imresize(imgA, 0.5);

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

%[h w] = size(imgC);
pos = getPosition(imrect()); % user input
imgD = SynthesizeFilter(h, w, pos);
pos = getPosition(imrect());
imgD2 = SynthesizeFilter(h, w, pos);
pos = getPosition(imrect());
imgD3 = SynthesizeFilter(h, w, pos);

imgD = imgD & imgD2 & imgD3;

figure; imshow((imgC+imgD)/2);
title('Power spectrum density + mask');

%**********************
%*****Filtering********
%**********************

imgE = ifft2(fftshift(imgD).*img_fft); %filtering
disp('real = ')
max(max(abs(real(imgE))))
disp('imag = ')
max(max(abs(imag(imgE))))
%return;
imgE = real(imgE);  %it needs to get real part because imgE is complex value
imgE = uint8(255*(imgE - min(min(imgE))) /(max(max(imgE)) - min(min(imgE))));
imgE = imadjust(imgE);

%*******************
%*****Output********
%*******************

figure; imshow(imgE);
title('Final image');

figure, imshowpair(imgA, imgE,'montage');
imwrite(imgA,strcat(strFolder,'output\input.jpg'));
imwrite(imgE,strcat(strFolder,'output\output.jpg'));
imwrite(imgC,strcat(strFolder,'output\Power_spectrum_density.jpg'));
imwrite((imgC+imgD)/2,strcat(strFolder,'output\filter.png'));