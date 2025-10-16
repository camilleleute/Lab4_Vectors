/*******************************************************
* File: sobel.h
*
* Description: sobel header file
*
* Authors: Advika Deodhar and Camille Leute
*
* Revision history
*
********************************************************/
#ifndef sobel_H
#define sobel_H

#include <iostream>
#include <opencv2/core.hpp> 
#include <array>
#include <cstdint>
#include <thread>
#include <vector>
#include <atomic>
#include <mutex>
#include <condition_variable>
#include <cmath>



cv::Mat test_matrix();
std::array<int, 9> find_neighbors(cv::Mat& matrix, int curr_pixel_row, int curr_pixel_col);
int16_t calculate_Gx(std::array<int, 9>& neighbors);
int16_t calculate_Gy(std::array<int, 9>& neighbors);
uint8_t sum_and_clamp(int16_t Gx_sum, int16_t Gy_sum);
cv::Mat to442_sobel(cv::Mat& matrix);



//- #defines go here

//- function prototypes go here

#endif 
