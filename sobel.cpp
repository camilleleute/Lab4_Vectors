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

   //! TEST CODE CHAT CHAT CHAT
//create test matrix to test on
Mat test_matrix(){
    Mat mat;
    mat.create(4, 4, CV_8UC1); //1 channel because greyscale

    // Fill the matrix with values for testing
    for (int r = 0; r < mat.rows; r++) {
        for (int c = 0; c < mat.cols; c++) {
            mat.at<uchar>(r, c) = r * mat.cols + c + 1; // numbers 1..16
        }
    }

    //prints matrix to stdout
    cout << "test matrix:\n" << mat << endl;
    return mat;
}

//itearte through the input matrix and find the neighbors of the current
//return an array
//find the neighbors of the current pixel and return an array of them
//if its on the edge, return an array of all zeros
array<int, 9> find_neighbors(Mat& matrix, int curr_pixel_row, int curr_pixel_col) {
    //make an aray of 9 neighbors and start them empty
    array<int,9> neighbors{};

    //get total rows and collumns
    int rows = matrix.rows;
    int cols = matrix.cols;

    //check if image is on the border
    if (curr_pixel_row == 0 || curr_pixel_row == rows - 1 || curr_pixel_col == 0 || curr_pixel_col == cols - 1) {
        return neighbors;
    }
    else {
        //start in left upper corner and end in right bottom corner
        int start_pixel_row = curr_pixel_row - 1;
        int end_pixel_row = curr_pixel_row + 1;
        int start_pixel_col = curr_pixel_col-1;
        int end_pixel_col = curr_pixel_col + 1;

        int final_arr_idx = 0;

        for (int i = start_pixel_row; i <= end_pixel_row; i++){
            //grab the entire row for faster mem access
            const uchar* rowptr = matrix.ptr<uchar>(i);
            for (int j = start_pixel_col; j <= end_pixel_col; j++){
                //add the current pixel to the neighbors matrix
                neighbors[final_arr_idx] = rowptr[j];
                final_arr_idx++;
            }
        }
    }

    return neighbors;
}

//take in an array and return one value
//multiply the current neighbors array with the Gx array and sum it all together
//all values are signed and 16 bit when doing the math
int16_t calculate_Gx(array<int, 9>& neighbors){
    const array<int,9> Gx_weights{-1, 0, 1, 
                                  -2, 0, 2, 
                                  -1, 0, 1 };

    int16_t total_sum = 0;
    for (int i = 0; i < 9; i ++){
        total_sum += neighbors[i] * Gx_weights[i];
    }
    return total_sum;
}

//take in an array and return one value
//same thing for Gy
//all values are signed and 16 bit when doing the math
int16_t calculate_Gy(array<int, 9>& neighbors){
    const array<int,9> Gy_weights{1, 2, 1, 
                                  0, 0, 0, 
                                  -1, -2, -1};

    int16_t total_sum = 0;
    for (int i = 0; i < 9; i ++){
        total_sum += neighbors[i] * Gy_weights[i];
    }
    return total_sum;
}

//take in the signed 16 bit value 
//do the absolute value calculation and then clamp it
//all values are signed and 16 bit when doing the math
//after clamping, store it back into a uint8
//static_cast<type>(expression) is how you cast in c++
uint8_t sum_and_clamp(int16_t Gx_sum, int16_t Gy_sum){
    int16_t absolute_sum = abs(Gx_sum) + abs(Gy_sum);
    if(absolute_sum <= 254){
        return static_cast<uint8_t>(absolute_sum);
    }
    return 255;
}

//now just call those all together
//takes in the greyscale matrix and then does this for every pixel in it
Mat to442_sobel(Mat& matrix) {
    int total_rows = matrix.rows;
    int total_cols = matrix.cols;

    Mat sobeled_matrix(total_rows, total_cols, CV_8UC1);

    for (int i = 0; i < matrix.rows; i++){
        for (int j = 0; j < matrix.cols; j++){
            array<int, 9> nb = find_neighbors(matrix, i, j);
            int16_t xsum = calculate_Gx(nb);
            int16_t ysum = calculate_Gy(nb);
            uint8_t final_val = sum_and_clamp(xsum, ysum);
            sobeled_matrix.at<uchar>(i, j) = final_val;
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
    // Read the chunk (includes padding rows for neighbor calculations)
    Mat chunk = matrix(Range(bounds[threadId].read_start, bounds[threadId].read_end + 1), Range::all());
    
    // Apply Sobel to the chunk
    Mat sobeled_chunk = to442_sobel(chunk);
    
    // Calculate which rows from sobeled_chunk to write
    // If read_start == write_start (top border), we write from row 0
    // Otherwise, we skip the first row (which is padding and got zeroed)
    int start_row_in_chunk = (bounds[threadId].read_start == bounds[threadId].write_start) ? 0 : 1;
    
    // Similarly for the end - if read_end == write_end (bottom border), include the last row
    // Otherwise, exclude it
    int num_rows_to_write = bounds[threadId].write_end - bounds[threadId].write_start + 1;
    
    // Copy the valid portion to the output frame
    Mat chunk_result = sobeled_chunk(Range(start_row_in_chunk, start_row_in_chunk + num_rows_to_write), Range::all());
    Mat chunk_to_write = output_frame(Range(bounds[threadId].write_start, bounds[threadId].write_end + 1), Range::all());
    chunk_result.copyTo(chunk_to_write);

    return 0;
}

//!CHAT WRITTEN TEST FUNC
// void print_thread_bounds(const std::array<ThreadBounds, 4>& bounds, int total_rows) {
//     std::cout << "TOTAL ROWS BEFORE SPLITTING: " << total_rows << std::endl;
//     std::cout << "=== THREAD ROW ASSIGNMENTS ===" << std::endl;

//     for (int i = 0; i < 4; i++) {
//         std::cout << "Thread " << i << ":" << std::endl;
//         std::cout << "  Write Start: " << bounds[i].write_start
//                   << "  Write End: "   << bounds[i].write_end << std::endl;
//         std::cout << "  Read Start: "  << bounds[i].read_start
//                   << "  Read End: "    << bounds[i].read_end  << std::endl;
//         std::cout << "---------------------------------" << std::endl;
//     }
// }

