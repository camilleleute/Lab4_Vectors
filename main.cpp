/*****************************************************
* Filename: main.c
******************************************************/
#include "main.h"

//! main for image to greyscale
int main(int argc, char* argv[]){
    // make sure that there are enough arguments provided
    if (argc != 2) {
        std::cerr << "Error: not enough arguments" << std::endl;
        return 1;
    }

    // open the file
    std::string filename = argv[1];
    VideoCapture frame(filename);

    if(!frame.isOpened())
    {
        std::cout << "Could not open the video: " << filename << std::endl;
        return 1;
    }
    
    Mat src_frame;

    //while there are still more frames to read
    while(frame.read(src_frame) == true){
        //greyscale that hoe
        Mat gray_img = to442_grayscale(src_frame);
        //make the frame that the children threads are going to write to.
        output_frame.create(gray_img.rows, gray_img.cols, CV_8UC1);
        //calculate the threadbounds
        array<ThreadBounds, 4> bounds = find_chunk(gray_img.rows);
        //print_thread_bounds(bounds, gray_img.rows);

        //now make the children and assign them the task
        thread t0(thread_task, ref(gray_img), ref(bounds), 0);
        thread t1(thread_task, ref(gray_img), ref(bounds), 1);
        thread t2(thread_task, ref(gray_img), ref(bounds), 2);
        thread t3(thread_task, ref(gray_img), ref(bounds), 3);

        //wait for all the threads to complete
        t0.join();
        t1.join();
        t2.join();
        t3.join();
        //cout << "threads joined" << endl;

        //stop it if u press the escape key
        imshow("sobby filty", output_frame);
        if(waitKey(1) == 27) {
            cout << "stopped the video" << endl;
            break;
        }
    }

    frame.release();
    destroyAllWindows();
    return 0;
}

 //! was for testing the right reading and writing chunk splitting prints
    // cout << "TOTAL ROWS BEFORE SPLITTING: " << gray_img.rows << endl;
    // cout << "=== THREAD ROW ASSIGNMENTS ===" << endl;
    // for (int i = 0; i < 4; i++) {
    //     cout << "Thread " << i << ":" << endl;
    //     cout << "  Write Start: " << bounds[i].write_start
    //      << "  Write End: "   << bounds[i].write_end << endl;
    //     cout << "  Read Start: "  << bounds[i].read_start
    //      << "  Read End: "    << bounds[i].read_end  << endl;
    //     cout << "---------------------------------" << endl;
    // }
    //!just for displaying test
    //Mat sobel_img = to442_sobel(gray_img);
    //Write result
    //imwrite("output_gray.jpg", gray_img);
    //imwrite("output_sobel.jpg", sobel_img);
    //title of the window and what image to show
    //imshow("Display Window", sobel_img);


// !testing the sobel stuff
// int main(int argc, char* argv[]){
//     //TESTING SOBEL
//     Mat test = test_matrix();

//     // 2. Pick a pixel that is NOT on the border
//     int row = 2;
//     int col = 1;

//     cout << "Picked point: " << (int)test.at<uchar>(row, col) << endl;

//     // 3. Call find_neighbors on that pixel
//     array<int, 9> nb = find_neighbors(test, row, col);

//     // now multiply by x weight

//     int16_t xsum = calculate_Gx(nb);
//     int16_t ysum = calculate_Gy(nb);

//     cout << "Gx sum : " << xsum << endl;
//     cout << "Gy sum : " << ysum << endl;

//     uint8_t final_val = sum_and_clamp(xsum, ysum);
//     cout << "Pixel value after Sobel: " << (int)final_val << endl;

//     Mat sobel = to442_sobel(test);
//     cout << "sobel matrix:\n" << sobel << endl;

//     return 0;
// }

// --- VIDEO TESTING MAIN --- //
//int main(int argc, char* argv[]){

//     // make sure that there are enough arguments provided
//     if (argc != 2) {
//         std::cerr << "Error: Not enough arguments" << std::endl;
//         return 1;
//     }

//     // Open file
//     std::string videoPath = argv[1];
//     cv::VideoCapture cap(videoPath);

    // // Check if the video file was opened successfully
    // if (!cap.isOpened()) {
    //     std::cerr << "Error: Could not open or not a valid video file: " << std::endl;
    //     return 1;
    // }
    

 //   return 0;

//}
