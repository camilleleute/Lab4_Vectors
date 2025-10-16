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


/*-----------------------------------------------------
* Function: to442_sobel()
*
* Description: Takes a grayscale Mat and returns a Mat 
* with the Sobel filter applied to it.
*
* inputs: datatype: brief description
*
* return: datatype
*--------------------------------------------------------*/

//to442_sobel() {

//}

// sobel multithreading
// Thread worker

static void sobel_worker(const Mat &cropped_gray,
                         Mat &output_full,
                         int cols,
                         int region_start_row,        // inclusive in the cropped image coords
                         int region_end_row,          // inclusive in the cropped image coords
                         int proc_roi_start_row,      // inclusive start row of ROI used for convolution
                         int proc_roi_end_row,        // inclusive end row of ROI used for convolution
                         atomic<int> &done_count,
                         mutex &cv_m,
                         condition_variable &cv_done)
{
    // ROI in cropped_gray is [proc_roi_start_row, proc_roi_end_row] inclusive
    // We'll compute sobel for each target row t in [region_start_row, region_end_row].
    // For each target row t we need neighbors t-1 and t+1 (so ROI must include them).
    // For columns we compute x in [1, cols-2]. cols must be >= 3 to compute sobel.

    const int roi_row_offset = proc_roi_start_row; // to convert absolute row -> roi row index

    // If width too small, nothing to do
    if (cols < 3) {
        done_count.fetch_add(1);
        cv_done.notify_one();
        return;
    }

    for (int trow = region_start_row; trow <= region_end_row; ++trow) {
        // y index in roi that corresponds to trow
        int y_in_roi = trow - roi_row_offset;
        // valid neighbor rows exist because ROI included padding above/below (except edge cases at image bounds)
        // compute for x from 1 to cols-2
        for (int x = 1; x <= cols - 2; ++x) {
            // compute gx, gy using 3x3 neighborhood in cropped_gray (roi coords)
            // neighbor positions in roi:
            // (y_in_roi-1, x-1)  (y_in_roi-1, x)  (y_in_roi-1, x+1)
            // (y_in_roi  , x-1)  (y_in_roi  , x)  (y_in_roi  , x+1)
            // (y_in_roi+1, x-1)  (y_in_roi+1, x)  (y_in_roi+1, x+1)
            int gx =
                -1 * cropped_gray.at<uchar>(y_in_roi - 1, x - 1) + 1 * cropped_gray.at<uchar>(y_in_roi - 1, x + 1)
                + -2 * cropped_gray.at<uchar>(y_in_roi    , x - 1) + 2 * cropped_gray.at<uchar>(y_in_roi    , x + 1)
                + -1 * cropped_gray.at<uchar>(y_in_roi + 1, x - 1) + 1 * cropped_gray.at<uchar>(y_in_roi + 1, x + 1);

            int gy =
                -1 * cropped_gray.at<uchar>(y_in_roi - 1, x - 1) + -2 * cropped_gray.at<uchar>(y_in_roi - 1, x) + -1 * cropped_gray.at<uchar>(y_in_roi - 1, x + 1)
                +  1 * cropped_gray.at<uchar>(y_in_roi + 1, x - 1) +  2 * cropped_gray.at<uchar>(y_in_roi + 1, x) +  1 * cropped_gray.at<uchar>(y_in_roi + 1, x + 1);

            // magnitude
            int mag = static_cast<int>(std::round(std::hypot(gx, gy)));
            output_full.at<uchar>(trow, x) = clampToUchar(mag);
        }
        // leave column 0 and cols-1 alone (0) for now
    }

    // mark done & notify
    int prev = done_count.fetch_add(1) + 1;
    {
        lock_guard<mutex> lk(cv_m);
        // notify main waiting thread
    }
    cv_done.notify_one();
}

Mat apply_sobel_multithread(const Mat &gray) {

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

    // copy gray so OG isnt modded
    Mat cropped_gray = gray.rowRange(0, new_rows);

   // create the threads
    vector<thread> threads;

    // Lambda for threaded processing
    //  get the slice area and apply sobel to it
    //  then put sobel slice back in
    auto process_quarter = [&](int start_row, int end_row) {
        int16_t sobel_start = 0;
        int16_t sobel_end = rows;
        if (start_row != 0) {
            sobel_start = start_row - 1
        }

        if (end_row != rows) {
            sobel_end = end_row + 1;
        }

        Mat slice = gray(Range(start_row, end_row), Range::all());
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