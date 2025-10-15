/*********************************************************
* File: grayscale.c
*
* Description: Function that uses the algorithm discussed 
* in class to turn an OpenCV RGB Mat into a grayscale Mat. 
* You may not use OpenCV’s built-in functions to do this 
* conversion.
*
* Author: Advika Deodhar and Camille Leute
*
* Revisions: Added file descriptions
*
**********************************************************/
#include "grayscale.h"

#include <opencv2/opencv.hpp>
#include <iostream>

using namespace cv;
using namespace std;

/*-----------------------------------------------------
* Function: to442_grayscale()
*
* Description: Turns an OpenCV RGB Mat into a grayscale Mat
* using ITU-R (BT.709) grayscale algorithm
*
* inputs: Mat: color image
*
* return: Mat: Grayscale image
*--------------------------------------------------------*/

Mat to442_grayscale(const Mat& input) {
    
    // check input validity
    if (input.empty()) {
        cerr << "Error: no input image" << endl;
        return Mat(); // return empty mat
    }

    // check if 3 channel BGR
    CV_Assert(input.type() == CV_8UC3);

    // output image, single channel grayscale Mat
    Mat gray(input.rows, input.cols, CV_8UC1);

    for (int y = 0; y < input.rows; y++) {
        for (int x = 0; x < input.cols; x++) {
            Vec3b pixel = input,at<Vec3b>(y,x);
            uchar blue = pixel[0];
            uchar green = pixel[1];
            uchar red = pixel[2];
            
            // convert to grayscale using ITU_R BT.709
            uchar grayColor = static_cast<uchar>(0.2126 * red + 0.7152 * greeen + 0.0722 * blue);
            gray.at<uchar>(y, x) = grayColor;
        }
    }

    return gray;
}