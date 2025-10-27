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
#include <arm_neon.h>

using namespace cv;
using namespace std;

// Scale coefficients (BT.709), scaled by 256 for integer math
#define R_COEFF 54    // 0.2126 * 256 ≈ 54
#define G_COEFF 183   // 0.7152 * 256 ≈ 183
#define B_COEFF 18    // 0.0722 * 256 ≈ 18

Mat to442_grayscale(const Mat& input);

#endif 
