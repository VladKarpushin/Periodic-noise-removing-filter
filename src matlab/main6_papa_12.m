% 2016-01-04 
% A periodic noise removing filter. Removes periodic noise, background and interferences in the Fourier domain. Useful for latent fingerprints, 
% capture artifacts and electromagnetic interferences in videos
% You will learn how to remove periodic noise in the Fourier domain
% 2018-08-17
% added user interactivness

close all,clc,clear all;

strFolder = 'D:\home\programming\vc\new\6_My home projects\13_Periodic_noise_removing_filter\';
strFileName = strcat(strFolder,'input\papa_2.bmp');
%strFileName = strcat(strFolder,'input\output_2.bmp');
%strFileName = strcat(strFolder,'input\1.bmp');

%****************************
%*****  input image  ********
%****************************

imgA = imread(strFileName);
%imgA = imresize(imgA, 0.35);
%imgA = imresize(imgA, 0.5);

[h w c] = size(imgA);
if c == 3
    imgA = rgb2gray(imgA);
end

imgA = imadjust(imgA);
figure, imshow(imgA);
title('Original image with a periodic noise');

%****************************
%*****PSD calculation********
%****************************

img_fft = fft2(imgA);           %spectrum 
img_fft(1,1) = 0;               %removing of constant component
imgB = img_fft.*conj(img_fft);  %Power spectrum density
imgC = fftshift(255*(imgB -min(min(imgB))) /(max(max(imgB)) - min(min(imgB))));

figure; imshow(imgC);
title('Power spectrum density of the original image');

pos = getPosition(imrect()); % user input
imgD = SynthesizeFilter(h, w, pos);
pos = getPosition(imrect());
imgD2 = SynthesizeFilter(h, w, pos);
pos = getPosition(imrect());
imgD3 = SynthesizeFilter(h, w, pos);

imgD = imgD & imgD2 & imgD3;

figure; imshow((imgC+imgD)/2);
title('Power spectrum density + restoration filter mask');

%**********************
%*****Filtering********
%**********************

imgE = ifft2(fftshift(imgD).*img_fft); %filtering
fprintf("max(|real|) =  %f\nmax(|imag|) =  %f\n", max(max(abs(real(imgE)))), max(max(abs(imag(imgE)))));

%return;
imgE = real(imgE);  %it needs to get real part because imgE is complex value
imgE = uint8(255*(imgE - min(min(imgE))) /(max(max(imgE)) - min(min(imgE))));
imgE = imadjust(imgE);

%*******************
%*****Output********
%*******************

figure; imshow(imgE);
title('Recovered image');

%figure, imshowpair(imgA, imgE,'montage');
imwrite(imgA,strcat(strFolder,'output\input.jpg'));
imwrite(imgE,strcat(strFolder,'output\output.jpg'));
imwrite(imgC,strcat(strFolder,'output\Power_spectrum_density.jpg'));
imwrite((imgC+imgD)/2,strcat(strFolder,'output\filter.png'));