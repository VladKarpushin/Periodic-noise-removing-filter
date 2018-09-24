/**
* @brief You will learn how to remove periodic noise in the Fourier domain
* @author Karpushin Vladislav, karpushin@ngs.ru, https://github.com/VladKarpushin
*/
#include <iostream>
#include "opencv2/imgproc.hpp"
#include "opencv2/imgcodecs.hpp"

using namespace cv;
using namespace std;

//void calcPSF(Mat& outputImg, Size filterSize, int len, double theta);
void fftshift(const Mat& inputImg, Mat& outputImg);
void filter2DFreq(const Mat& inputImg, Mat& outputImg, const Mat& H);
//void calcWnrFilter(const Mat& input_h_PSF, Mat& output_G, double nsr);
void synthesizeFilter(Size filterSize, Mat& output_H);
void calcPSD(const Mat& inputImg, Mat& outputImg, int flag = 0);

int main()
{
	Mat imgIn = imread("D:\\home\\programming\\vc\\new\\6_My home projects\\13_Periodic_noise_removing_filter\\input\\input.jpg", IMREAD_GRAYSCALE);
    if (imgIn.empty()) //check whether the image is loaded or not
    {
        cout << "ERROR : Image cannot be loaded..!!" << endl;
        return -1;
    }

    Mat imgOut;

//! [main]
    // it needs to process even image only
    Rect roi = Rect(0, 0, imgIn.cols & -2, imgIn.rows & -2);

    //H calculation (start)
    Mat H;
	synthesizeFilter(roi.size(), H);
    //H calculation (stop)

	imgIn.convertTo(imgIn, CV_32F);

	// PSD calculation (start)
	Mat imgPSD;
	calcPSD(imgIn(roi), imgPSD);
	fftshift(imgPSD, imgPSD);
	normalize(imgPSD, imgPSD, 0, 255, NORM_MINMAX);
	// PSD calculation (stop)

    // filtering (start)
    filter2DFreq(imgIn(roi), imgOut, H);
    // filtering (stop)
//! [main]

    imgOut.convertTo(imgOut, CV_8U);
    normalize(imgOut, imgOut, 0, 255, NORM_MINMAX);

	//char  buf[100];
	//sprintf_s(buf, "_LEN = %d_THETA = %4.1f_snr = %d", LEN, THETA, snr);
	//string strOutFileName = strInFileName;
	//strOutFileName.insert(strOutFileName.size() - 4, buf);
	//imwrite(strOutFileName, imgOut);

	//string strOutFileNameTmp = strInFileName;
	//strOutFileNameTmp.insert(strOutFileNameTmp.size() - 4, "_after_edgetaper");
	//imwrite(strOutFileNameTmp, imgIn);
    return 0;
}

////! [calcPSF]
//void calcPSF(Mat& outputImg, Size filterSize, int len, double theta)
//{
//    Mat h(filterSize, CV_32F, Scalar(0));
//    Point point(filterSize.width / 2, filterSize.height / 2);
//    //circle(h, point, R, 255, -1, 8);
//	//ellipse(h, point, Size(0,cvRound(float(len)/2.0)), 90-theta, 0, 360, 255, -1);
//	ellipse(h, point, Size(0, cvRound(float(len) / 2.0)), 90.0 - theta, 0, 360, Scalar(255), FILLED);
//	
//    Scalar summa = sum(h);
//    outputImg = h / summa[0];
//}
////! [calcPSF]

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

////! [calcWnrFilter]
//void calcWnrFilter(const Mat& input_h_PSF, Mat& output_G, double nsr)
//{
//    Mat h_PSF_shifted;
//    fftshift(input_h_PSF, h_PSF_shifted);
//    Mat planes[2] = { Mat_<float>(h_PSF_shifted.clone()), Mat::zeros(h_PSF_shifted.size(), CV_32F) };
//    Mat complexI;
//    merge(planes, 2, complexI);
//    dft(complexI, complexI);
//    split(complexI, planes);
//    Mat denom;
//    pow(abs(planes[0]), 2, denom);
//    denom += nsr;
//    divide(planes[0], denom, output_G);
//}
////! [calcWnrFilter]

void synthesizeFilter(Size filterSize, Mat& output_H)
{
	output_H = Mat(filterSize, CV_32F, Scalar(1));
}

// Function calculates PSD(Power spectrum density) by fft with two flags
// flag = 0 means to return PSD
// flag = 1 means to return log(PSD)
void calcPSD(const Mat& inputImg, Mat& outputImg, int flag)
{
	Mat planes[2] = { Mat_<float>(inputImg.clone()), Mat::zeros(inputImg.size(), CV_32F) };
	Mat complexI;
	merge(planes, 2, complexI);         // Add to the expanded another plane with zeros
	dft(complexI, complexI);            // this way the result may fit in the source matrix
	split(complexI, planes);            // planes[0] = Re(DFT(I), planes[1] = Im(DFT(I))

	//float * p;
	//p = planes[0].ptr<float>(0);
	//p[0] = 0;
	//p = planes[1].ptr<float>(0);
	//p[0] = 0;
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