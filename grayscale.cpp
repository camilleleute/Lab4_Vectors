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
        const uchar* src = input.ptr<uchar>(y);
        uchar* dst = gray.ptr<uchar>(y);

        int x = 0;
        for (; x <= input.cols - 16; x += 16){
             // Load 16 pixels of each channel
            uint8x16x3_t rgb = vld3q_u8(src + x * 3);
            
            // Widen to 16-bit for math
            uint16x8_t r_low = vmovl_u8(vget_low_u8(rgb.val[2]));
            uint16x8_t g_low = vmovl_u8(vget_low_u8(rgb.val[1]));
            uint16x8_t b_low = vmovl_u8(vget_low_u8(rgb.val[0]));

            uint16x8_t r_high = vmovl_u8(vget_high_u8(rgb.val[2]));
            uint16x8_t g_high = vmovl_u8(vget_high_u8(rgb.val[1]));
            uint16x8_t b_high = vmovl_u8(vget_high_u8(rgb.val[0]));

            // Apply grayscale
            uint16x8_t gray_low = vmlaq_n_u16(vmlaq_n_u16(vmulq_n_u16(r_low, R_COEFF),g_low, G_COEFF),b_low, B_COEFF);

            uint16x8_t gray_high = vmlaq_n_u16(vmlaq_n_u16(vmulq_n_u16(r_high, R_COEFF),g_high, G_COEFF),b_high, B_COEFF);

            // Normalize/divide by 256
            gray_low = vshrq_n_u16(gray_low, 8);
            gray_high = vshrq_n_u16(gray_high, 8);

            // Narrow back to 8-bit and combine
            uint8x16_t gray_vec = vcombine_u8(vqmovn_u16(gray_low), vqmovn_u16(gray_high));

            // Store result
            vst1q_u8(dst + x, gray_vec);
            
        }
        // Handle leftover pixels
        for (; x < input.cols; x++) {
            uchar b = src[x * 3 + 0];
            uchar g = src[x * 3 + 1];
            uchar r = src[x * 3 + 2];
            dst[x] = static_cast<uchar>((r * R_COEFF + g * G_COEFF + b * B_COEFF) >> 8);
        }
    }

    return gray;
}