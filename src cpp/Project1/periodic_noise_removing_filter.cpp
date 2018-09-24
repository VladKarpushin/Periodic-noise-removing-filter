﻿/**
* @brief You will learn how to remove periodic noise in the Fourier domain
* @author Karpushin Vladislav, karpushin@ngs.ru, https://github.com/VladKarpushin
*/
#include <iostream>
#include "opencv2/imgcodecs.hpp"

using namespace cv;
using namespace std;

void fftshift(const Mat& inputImg, Mat& outputImg);
void filter2DFreq(const Mat& inputImg, Mat& outputImg, const Mat& H);
void synthesizeFilter(Mat& inputOutput_H, Rect roi);
void calcPSD(const Mat& inputImg, Mat& outputImg, int flag = 0);

int main()
{
	Mat imgIn = imread("D:\\home\\programming\\vc\\new\\6_My home projects\\13_Periodic_noise_removing_filter\\input\\input.jpg", IMREAD_GRAYSCALE);
    if (imgIn.empty()) //check whether the image is loaded or not
    {
        cout << "ERROR : Image cannot be loaded..!!" << endl;
        return -1;
    }

	imgIn.convertTo(imgIn, CV_32F);

//! [main]
    // it needs to process even image only
    Rect roi = Rect(0, 0, imgIn.cols & -2, imgIn.rows & -2);
	imgIn = imgIn(roi);

	// PSD calculation (start)
	Mat imgPSD;
	calcPSD(imgIn, imgPSD);
	fftshift(imgPSD, imgPSD);
	normalize(imgPSD, imgPSD, 0, 255, NORM_MINMAX);
	// PSD calculation (stop)

	//H calculation (start)
	Mat H = Mat(roi.size(), CV_32F, Scalar(1));
	const int w = 40;
	Rect roiA(684, 447, w, w);
	Rect roiB(829, 373, w, w);
	Rect roiC(960, 304, w, w);
	synthesizeFilter(H, roiA);
	synthesizeFilter(H, roiB);
	synthesizeFilter(H, roiC);
	Mat imgHPlusPSD = imgPSD + H*255;
	normalize(imgHPlusPSD, imgHPlusPSD, 0, 255, NORM_MINMAX);
	imgHPlusPSD.convertTo(imgHPlusPSD, CV_8U);
	//H calculation (stop)
	
	// filtering (start)
	Mat imgOut;
	fftshift(H, H);
	filter2DFreq(imgIn, imgOut, H);
    // filtering (stop)
//! [main]

    imgOut.convertTo(imgOut, CV_8U);
    normalize(imgOut, imgOut, 0, 255, NORM_MINMAX);
	imwrite("result.jpg", imgOut);

	imgIn.convertTo(imgIn, CV_8U);
	normalize(imgIn, imgIn, 0, 255, NORM_MINMAX);
	imwrite("input.jpg", imgIn);

	imgPSD.convertTo(imgPSD, CV_8U);
	normalize(imgPSD, imgPSD, 0, 255, NORM_MINMAX);
	imwrite("PSD.jpg", imgPSD);

	imwrite("imgHPlusPSD.jpg", imgHPlusPSD);
	return 0;
}

//! [fftshift]
void fftshift(const Mat& inputImg, Mat& outputImg)
{
    outputImg = inputImg.clone();
    int cx = outputImg.cols / 2;
    int cy = outputImg.rows / 2;
    Mat q0(outputImg, Rect(0, 0, cx, cy));
    Mat q1(outputImg, Rect(cx, 0, cx, cy));
    Mat q2(outputImg, Rect(0, cy, cx, cy));
    Mat q3(outputImg, Rect(cx, cy, cx, cy));
    Mat tmp;
    q0.copyTo(tmp);
    q3.copyTo(q0);
    tmp.copyTo(q3);
    q1.copyTo(tmp);
    q2.copyTo(q1);
    tmp.copyTo(q2);
}
//! [fftshift]

//! [filter2DFreq]
void filter2DFreq(const Mat& inputImg, Mat& outputImg, const Mat& H)
{
    Mat planes[2] = { Mat_<float>(inputImg.clone()), Mat::zeros(inputImg.size(), CV_32F) };
    Mat complexI;
    merge(planes, 2, complexI);
    dft(complexI, complexI, DFT_SCALE);

    Mat planesH[2] = { Mat_<float>(H.clone()), Mat::zeros(H.size(), CV_32F) };
    Mat complexH;
    merge(planesH, 2, complexH);
    Mat complexIH;
    mulSpectrums(complexI, complexH, complexIH, 0);

    idft(complexIH, complexIH);
    split(complexIH, planes);
    outputImg = planes[0];

	// filterrring check (start)
	Mat abs0 = abs(planes[0]);
	Mat abs1 = abs(planes[1]);
	double MaxReal, MinReal;
	minMaxLoc(abs0, &MinReal, &MaxReal, NULL, NULL);
	double MaxIm, MinIm;
	minMaxLoc(abs1, &MinIm, &MaxIm, NULL, NULL);
	cout << "MaxReal = " << MaxReal << "; MinReal = " << MinReal << endl;
	cout << "MaxIm = " << MaxIm << "; MinIm = " << MinIm << endl;
	// filterrring check (stop)
}
//! [filter2DFreq]

void synthesizeFilter(Mat& inputOutput_H, Rect roi)
{
	Rect roiA2 = roi, roiA3 = roi, roiA4 = roi;
	roiA2.y = inputOutput_H.rows - roi.y - roi.height + 1;
	roiA3.x = inputOutput_H.cols - roi.x - roi.width + 1;
	roiA4.x = roiA3.x;
	roiA4.y = roiA2.y;
	
	inputOutput_H(roi) = 0;
	inputOutput_H(roiA2) = 0;
	inputOutput_H(roiA3) = 0;
	inputOutput_H(roiA4) = 0;
}

// Function calculates PSD(Power spectrum density) by fft with two flags
// flag = 0 means to return PSD
// flag = 1 means to return log(PSD)
void calcPSD(const Mat& inputImg, Mat& outputImg, int flag)
{
	Mat planes[2] = { Mat_<float>(inputImg.clone()), Mat::zeros(inputImg.size(), CV_32F) };
	Mat complexI;
	merge(planes, 2, complexI);
	dft(complexI, complexI);
	split(complexI, planes);            // planes[0] = Re(DFT(I)), planes[1] = Im(DFT(I))

	planes[0].at<float>(0) = 0;
	planes[1].at<float>(0) = 0;

	// compute the PSD and switch to logarithmic scale
	// => log(1 + sqrt(Re(DFT(I))^2 + Im(DFT(I))^2))
	Mat imgPSD;
	magnitude(planes[0], planes[1], imgPSD);		//imgPSD = sqrt(Power spectrum density)
	pow(imgPSD, 2, imgPSD);							//it needs ^2 in order to get PSD
	outputImg = imgPSD;

	if (flag)
	{
		Mat imglogPSD;
		imglogPSD = imgPSD + Scalar::all(1);		//switch to logarithmic scale
		log(imglogPSD, imglogPSD);					//imglogPSD = log(PSD)
		outputImg = imglogPSD;
	}
}