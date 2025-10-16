/*****************************************************
* Filename: main.c
******************************************************/
#include "main.h"
#include "sobel.h"

//! main for image to greyscale
int main(int argc, char* argv[]){
    // make sure that there are enough arguments provided
    if (argc != 2) {
        std::cerr << "Error: not enough arguments" << std::endl;
        return 1;
    }

    // open the file
    std::string filename = argv[1];
    // returns a matrix with the image loaded into it in BGR-8bit format
    Mat image = imread(filename, IMREAD_COLOR);

    // check if file couldn't be read
    if(image.empty())
    {
        std::cout << "Could not read the image: " << filename << std::endl;
        return 1;
    }
    
    Mat gray_img = to442_grayscale(image);
    Mat sobel_img = to442_sobel(gray_img);

    // Write result
    imwrite("output_gray.jpg", gray_img);
    imwrite("output_sobel.jpg", sobel_img);

     
    // title of the window and what image to show
    imshow("Display Window", sobel_img);
    waitKey(0);
     
    return 0;
}


<<<<<<< HEAD


// --- VIDEO TESTING MAIN --- //
// int main(int argc, char* argv[]){
=======
//!testing the sobel stuff
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


//? not done yet
// --- VIDEO TESTING MAIN --- //
//int main(int argc, char* argv[]){
>>>>>>> main

//     // make sure that there are enough arguments provided
//     if (argc != 2) {
//         std::cerr << "Error: Not enough arguments" << std::endl;
//         return 1;
//     }

//     // Open file
//     std::string videoPath = argv[1];
//     cv::VideoCapture cap(videoPath);

<<<<<<< HEAD
//     // Check if the video file was opened successfully
//     if (!cap.isOpened()) {
//         std::cerr << "Error: Could not open or not a valid video file: " << std::endl;
//         return 1;
//     }


//     return 0;

// }
=======
    // // Check if the video file was opened successfully
    // if (!cap.isOpened()) {
    //     std::cerr << "Error: Could not open or not a valid video file: " << std::endl;
    //     return 1;
    // }
    

 //   return 0;

//}
>>>>>>> main
