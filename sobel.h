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


extern cv::Mat src_frame;
extern cv::Mat output_frame;

//make a struct for reading and writing rows
struct ThreadBounds{
    int read_start;
    int read_end;
    int write_start;
    int write_end;
};

std::array<int, 9> find_neighbors(cv::Mat& matrix, int curr_pixel_row, int curr_pixel_col);
int16_t calculate_Gx(std::array<int, 9>& neighbors);
int16_t calculate_Gy(std::array<int, 9>& neighbors);
uint8_t sum_and_clamp(int16_t Gx_sum, int16_t Gy_sum);
cv::Mat to442_sobel(cv::Mat& matrix);
std::array<ThreadBounds, 4> find_chunk(int total_rows);
int thread_task(cv::Mat& matrix, std::array<ThreadBounds, 4>& bounds, int threadId);


#endif 
