/*******************************************************
* File: main.h
*
* Description: main header file
*
* Authors: Advika Deodhar and Camille Leute
*
* Revision history
*
********************************************************/
#ifndef MAIN_H
#define MAIN_H

//- #defines go here
#include "grayscale.h"
#include "sobel.h"
#include <opencv2/opencv.hpp>
#include <iostream>
#include <thread>
#include <opencv2/videoio.hpp>

cv::Mat src_frame;
cv::Mat output_frame;

#endif 
