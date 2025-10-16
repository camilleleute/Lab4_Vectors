/*******************************************************
* File: grayscale.h
*
* Description: grayscale header file
*
* Authors: Advika Deodhar and Camille Leute
*
* Revision history
*
********************************************************/
#ifndef GRAYSCALE_H
#define GRAYSCALE_H

#include <opencv2/opencv.hpp>
#include <iostream>

using namespace cv;
using namespace std;

#define RED_SCALER 0.2126
#define BLUE_SCALER 0.0722 
#define GREEN_SCALER 0.7152

Mat to442_grayscale(const Mat& input);

#endif 
