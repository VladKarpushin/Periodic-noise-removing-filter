% 2016-01-04 
% реализован алгоритм удаления трафаретной сетки (растра) при газетной печати
% background cell removing algorithm
% 2018-08-16
% ###added user interactivness

close all,clc,clear all;

imgA = imread('papa_1.bmp');
imgA = rgb2gray(imgA); %color->gray
figure; imshow(imgA);
title('Original image');

% user input:
% %h = imrect();
% pos = getPosition(h);

img_fft = fft2(imgA);   %spectrum 
img_fft(1,1) = 0;   %removing of constant component
imgB = img_fft.*conj(img_fft);  %Power spectrum density
imgC = fftshift(255*(imgB -min(min(imgB))) /(max(max(imgB)) - min(min(imgB))));
figure; imshow(imgC);
title('Power spectrum density');
imwrite(imgC,'output\Power_spectrum_density.bmp');

[h w] = size(imgC);
imgD = ones([h w]);

%Array of noise. Enough first eight components
x1 = [1620 1618 1211 1212 1416 1822 1414 1007 1409 1620 1726 1420 1212 1105 1622 1824 2028 2026 1821 1618 1208 1007 805  806 1010 1215];
y1 = [ 912 1222 1221  911 757  1068 1377 1066 890  1009 1068 1242 1123 1064 602  757  914  1225 1378 1532 1532 1375 1220 907 754  602];

n = 25; %size of removing spot. 30 - optimum
for k = 1:size(x1') % k - index of noise component
    for i =-n:n
         for j =-n:n
                imgD(y1(k)+i,x1(k)+j) = 0; %imgD(row, colomn), this is why imgD(y,x) rather than (x,y)
         end;
     end;
end;
figure; imshow((imgC+imgD)/2);
title('Power spectrum density + mask');

imgE = ifft2(fftshift(imgD).*img_fft); %filtering
imgE = real(imgE);  %it needs to get real part because imgE is complex value
imgF = 255*(imgE - min(min(imgE))) /(max(max(imgE)) - min(min(imgE)));
imgFF = uint8(imgF);    %in order to convert double to uint8
figure; imshow(imgFF);
title('Final image');
% imtool(imgA,[70 217]);
% imtool(imgFF,[46 178]);clo

imgAA = imadjust(imgA,[70 217]/255);
figure; imshow(imgAA);
imwrite(imgAA,'output\Contrasted_original_image.bmp');
%figure; imshow(imgA,[70 217]);
title('Contrasted original image');

imgFFF = imadjust(imgFF,[58 170]/255);
figure; imshow(imgFFF);
imwrite(imgFFF,'output\Contrasted_Final_image.bmp');
%figure; imshow(imgFF,[58 170]);
title('Contrasted Final image');

figure, imshowpair(imgAA, imgFFF,'montage');
figure, imshowpair(imgA, imgFF,'montage');