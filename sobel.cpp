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

//create test matrix to test on
Mat test_matrix(){
    Mat mat;
    mat.create(4, 4, CV_8UC1); //1 channel because greyscale

    //! TEST CODE CHAT CHAT CHAT
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

// sobel multithreading
// Thread worker
Mat sobel_multithread(const Mat &gray) {

    // check if input is valid
    if (gray.empty()) {
        cerr << "empty input while threading\n";
        return Mat();
    }
    
    CV_Assert(gray.type() == CV_8UC1);

    // make life easier, take off extra row bits if not divisible by 4
    int16_t rows = gray.rows - (rows % 4);
    int16_t cols = gray.cols;

    // zero filled empty output image
    Mat sobel_full = Mat::zeros(rows, cols, CV_8UC1);

   // create the threads
    vector<thread> threads;

    // Lambda for threaded processing
    //  get the slice area and apply sobel to it
    //  then put sobel slice back in
    auto process_quarter = [&](int start_row, int end_row) {
        int16_t sobel_start = 0;
        int16_t sobel_end = rows;
        if (start_row != 0) {
            sobel_start = start_row - 1;
        }

        if (end_row != rows) {
            sobel_end = end_row + 1;
        }

        Mat slice = gray(Range(sobel_start, sobel_end), Range::all());
        Mat sobel_slice = to442_sobel(slice);
        sobel_slice.copyTo(sobel_full(Range(start_row, end_row), Range::all()));
    };

    // Compute slice boundaries
    int quarter = rows / 4;
    for (int i = 0; i < 4; i++) {
        int start_row = i * quarter;
        int end_row = (i + 1) * quarter;
        threads.emplace_back(process_quarter, start_row, end_row);
    }

    // Wait for all threads to finish
    for (auto &t : threads)
        t.join();

    // Remove the edges so no inverse vignette
    Mat cropped = sobel_full(Range(1, rows - 1), Range(1, cols - 1)).clone();

    return cropped;
    
}