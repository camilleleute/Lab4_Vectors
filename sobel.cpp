/*********************************************************
* File: sobel.c
*
* Description: function that takes a grayscale Mat and 
* returns a Mat with the Sobel filter applied using the 
* algorithm and approximations discussed in class. Do not 
* use any built-in Sobel functions from OpenCV, you must 
* write the code to traverse the Mat manually.
*
* Author: Advika Deodhar and Camille Leute
*
* Revisions: Added file descriptions
*
**********************************************************/
#include "sobel.h"

using namespace cv;
using namespace std;

//takes in the greyscale matrix and then does this for every pixel in it
Mat to442_sobel(Mat& matrix) {

    //make sure the input matrix is a multiple of 8 before beginning
    Mat sobeled_matrix(matrix.rows, matrix.cols, CV_8UC1, Scalar(0));
    //dont include the first or the last row since we are skipping borders here
    for (int i = 1; i < matrix.rows-1; i++){
        //mat.ptr returns a pointer to the whole row of the matrix 
        //?do i need to check if its continuous before doing that??
        //get all the row neighbors (the top row, middle row, bottom row)
        //i-1 gives row above, i is the current row, i + 1 is the row below
        const uchar* top_row = matrix.ptr<uchar>(i-1);
        const uchar* mid_row = matrix.ptr<uchar>(i);
        const uchar* bot_row = matrix.ptr<uchar>(i+1);
        //now that we have a pointer to each of the rows, iterate through those via column
        //need to grab in chunks of 8 so have it move forward 8 every time
        //skip first and last col as well since we dont want border
        for (int j = 1; j < matrix.cols-8; j+=8){
            //vld1_u8(const uint8_t* ptr) -> uint8x8_t top_left;
            //make the vectors for all the neighbors by passing in the row pointer and which column to go to
            uint8x8_t top_left_8 = vld1_u8(&top_row[j-1]);
            uint8x8_t top_8 = vld1_u8(&top_row[j]);
            uint8x8_t top_right_8 = vld1_u8(&top_row[j+1]);

            uint8x8_t mid_left_8 = vld1_u8(&mid_row[j-1]);
            //uint8x8_t mid_8 = vld1_u8(&mid_row[j]);
            uint8x8_t mid_right_8 = vld1_u8(&mid_row[j+1]);

            uint8x8_t bot_left_8 = vld1_u8(&bot_row[j-1]);
            uint8x8_t bot_8 = vld1_u8(&bot_row[j]);
            uint8x8_t bot_right_8 = vld1_u8(&bot_row[j+1]);

            //now to do the math, need to make them all uint16 to deal with overflow 
            //this will use up the full 128 bit vector register
            //uint16x8_t vmovl_u8(uint8x8_t a);
            uint16x8_t top_left_16 = vmovl_u8(top_left_8);
            uint16x8_t top_16 = vmovl_u8(top_8);
            uint16x8_t top_right_16 = vmovl_u8(top_right_8);

            uint16x8_t mid_left_16 = vmovl_u8(mid_left_8);
            //uint16x8_t mid_16 = vmovl_u8(mid_8);
            uint16x8_t mid_right_16 = vmovl_u8(mid_right_8);

            uint16x8_t bot_left_16 = vmovl_u8(bot_left_8);
            uint16x8_t bot_16 = vmovl_u8(bot_8);
            uint16x8_t bot_right_16 = vmovl_u8(bot_right_8);

            //now that its all been 16 bitted,make it signed
            int16x8_t top_left_16_S = vreinterpretq_s16_u16(top_left_16);
            int16x8_t top_16_S = vreinterpretq_s16_u16(top_16);
            int16x8_t top_right_16_S = vreinterpretq_s16_u16(top_right_16);

            int16x8_t mid_left_16_S = vreinterpretq_s16_u16(mid_left_16);
            //int16x8_t mid_16_S = vreinterpretq_s16_u16(mid_16);
            int16x8_t mid_right_16_S = vreinterpretq_s16_u16(mid_right_16);

            int16x8_t bot_left_16_S = vreinterpretq_s16_u16(bot_left_16);
            int16x8_t bot_16_S = vreinterpretq_s16_u16(bot_16);
            int16x8_t bot_right_16_S = vreinterpretq_s16_u16(bot_right_16);

            //scalar multiply all the vectors by each value
            //int16x8_t vmulq_n_s16(int16x8_t a, int16_t b);
            //int16x8_t vaddq_s16(int16x8_t a, int16x8_t b);
            // {-1, 0, 1, 
            //  -2, 0, 2, 
            //  -1, 0, 1 };
            int16x8_t gx = vmulq_n_s16(top_left_16_S, -1);
            //gx = vaddq_s16(gx,vmulq_n_s16(top_16_S, 0));
            gx = vaddq_s16(gx,vmulq_n_s16(top_right_16_S, 1));

            gx = vaddq_s16(gx,vmulq_n_s16(mid_left_16_S, -2));
            //gx = vaddq_s16(gx,vmulq_n_s16(mid_16_S, 0));
            gx = vaddq_s16(gx,vmulq_n_s16(mid_right_16_S, 2));

            gx = vaddq_s16(gx,vmulq_n_s16(bot_left_16_S, -1));
            //gx = vaddq_s16(gx,vmulq_n_s16(bot_16_S, 0));
            gx = vaddq_s16(gx,vmulq_n_s16(bot_right_16_S, 1));

            //now the same for gy
            // {1, 2, 1, 
            //  0, 0, 0, 
            // -1, -2, -1};
            int16x8_t gy = vmulq_n_s16(top_left_16_S, 1);
            gy = vaddq_s16(gy,vmulq_n_s16(top_16_S, 2));
            gy = vaddq_s16(gy,vmulq_n_s16(top_right_16_S, 1));

            //!is this necessary if i multiply it by zero? summing would just give me zero right
            //gy = vaddq_s16(gy,vmulq_n_s16(mid_left_16_S, 0));
            //gy = vaddq_s16(gy,vmulq_n_s16(mid_16_S, 0));
            //gy = vaddq_s16(gy,vmulq_n_s16(mid_right_16_S, 0));

            gy = vaddq_s16(gy,vmulq_n_s16(bot_left_16_S, -1));
            gy = vaddq_s16(gy,vmulq_n_s16(bot_16_S, -2));
            gy = vaddq_s16(gy,vmulq_n_s16(bot_right_16_S, -1));

            //clamp and sum it all
            //first i want to absolute value everything, then sum it all, then clamp it
            int16x8_t gx_abs = vabsq_s16(gx);
            int16x8_t gy_abs = vabsq_s16(gy);
            //sum those two vectors together
            //int16x8_t vaddq_s16(int16x8_t a, int16x8_t b);
            int16x8_t sum = vaddq_s16(gx_abs, gy_abs);
            //clamp it and make it back into uint8
            //uint8x8_t vqmovn_u16(uint16x8_t a);
            //this intrinsic automatically makes it into the max 8-bit value if it goes over
            uint16x8_t sum_unsigned = vreinterpretq_u16_s16(sum);
            uint8x8_t result = vqmovn_u16(sum_unsigned);
            //write it to the output
            //void vst1_u8(__transfersize(8) uint8_t * ptr, uint8x8_t val);
            //printf("writing pixels to output matrix\n");
            vst1_u8(&sobeled_matrix.ptr<uchar>(i)[j], result);
        }
    }
    return sobeled_matrix;
}

//first need function to know which rows to give each chunk
//take in the entire matrix size and make it divisible by 4
//then decide the read and write rows for that specific frame
//put that into a struct
array<ThreadBounds, 4> find_chunk(int total_rows){

    //make an empty array to hold those objects
    array<ThreadBounds,4> bounds{};

    int rows_per_thread = total_rows / 4;
    int remainder = total_rows % 4;

    int start = 0;

    for(int i = 0; i < 4; i ++){
        int section_rows = rows_per_thread;

        //if its not evenly split then go through 
        if(remainder != 0) {
            section_rows += 1;
            remainder-= 1;
        }

        int end = start + section_rows - 1;

        //tell it where to write from (from its start till its end)
        bounds[i].write_start = start;
        bounds[i].write_end   = end;

        //if there is one row above start reading from there if not start from the 0 (top row)
        bounds[i].read_start = start - 1;
        if (bounds[i].read_start < 0) {
            bounds[i].read_start = 0;
        }
        //if there is one row below the end row then read until there if not then stop at last row (bottom)
        bounds[i].read_end = end + 1;
        if (bounds[i].read_end > total_rows - 1) {
            bounds[i].read_end = total_rows - 1;
        }

        //the start becomes the end of the last thread 
        start = end + 1;
    }
    return bounds;

}

//function for the thread to do
//tell it which row to start reading from and where to end reading
//does the reading, does the math, then writes it to its spot
//this gets assigned in the main thread
int thread_task(Mat& matrix, array<ThreadBounds, 4>& bounds, int threadId) {
    //read the right chunk out
    Mat chunk = matrix(Range(bounds[threadId].read_start, bounds[threadId].read_end + 1), Range::all());
    
    //sobel that hoe
    Mat sobeled_chunk = to442_sobel(chunk);
    
    //this takes care of the boundery issues
    //calculate which rows to write from the sobeled chunk since the borders will be gone
    //if we are reading from where we write to, we are at the top border so write from row zero
    //else, we skip the first row and write from the rest since the top border will be zeroed out
    int start_row_in_chunk;
    if (bounds[threadId].read_start == bounds[threadId].write_start) {
        start_row_in_chunk = 0;
    } else {
        start_row_in_chunk = 1;
    }
    
    //do the same thing for the end
    //if the read end is the same to where we write, its the bottom border and include the last row
    int num_rows_to_write = bounds[threadId].write_end - bounds[threadId].write_start + 1;
    
    //copy to output frame
    Mat chunk_result = sobeled_chunk(Range(start_row_in_chunk, start_row_in_chunk + num_rows_to_write), Range::all());
    Mat chunk_to_write = output_frame(Range(bounds[threadId].write_start, bounds[threadId].write_end + 1), Range::all());
    chunk_result.copyTo(chunk_to_write);

    return 0;
}

